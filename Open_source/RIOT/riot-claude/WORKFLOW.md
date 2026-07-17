# RIOT-Claude 工作流程

## 总览

五个阶段，十三个步骤。核心原则：**Claude 只管读代码写代码，不跑测试不编译**。所有验证由 Python 框架在独立容器完成。

---

## 准备阶段

### Step 1 — 仓库重置

**做什么**：`git reset --hard HEAD` + `git clean -fd -e "riot-claude" -e ".claude_home"`

**为什么**：正常流程不会动原始仓库（Claude 操作的是 copytree 出去的副本），但上一次 task 崩溃后残留状态会污染下一轮。每次起飞前检查跑道。

**注意**：排除整个 `riot-claude/` 目录和 `.claude_home/`（不能只排除子目录，否则 riot-claude/ 下的 Python 源码、Dockerfile、文档等会被清掉）。git 命令失败抛 `RuntimeError`，不在脏仓库上继续执行。

运行前用 `--check` 验证所有外部依赖：
```bash
wsl bash -c "cd /home/carl/RIOT/riot-claude && python3 runner.py --check"
```
检查项：Docker daemon、Docker 镜像、tree-sitter grammar、Claude settings、RIOT 仓库结构、已验证数据集文件（`.verified.jsonl`）。

### Step 2 — 工作区准备

**做什么**：为每个 task 创建隔离的源码副本，mask 目标函数体，锁定权限。

流程：
1. `shutil.copytree` 复制 RIOT 源码（`ignore_patterns` 排除 `.git` 和 `riot-claude/`）
2. 显式验证 `.git` 不在工作区——漏了 Claude 就能用 git 还原被 mask 的代码
3. 删除 `tests/` `docs/` `examples/` `samples/`——防止 Agent 看到测试断言和参考实现
4. tree-sitter mask 目标函数体（替换为 `/* MASKED - implement this function */`）
6. 复制 Claude settings 到 `claude_home/`
7. 权限锁定：目录 `chmod 555`，文件 `chmod a-w`，目标文件 `chmod 644`（唯一可写）

**为什么**：
- 隔离：每个 task 独立目录
- 防作弊：多层防护（删 tests/docs/examples+ 权限锁定）
- 每次全新副本：避免跨 task 残留

### Step 3 — 环境快照

**做什么**：记录 git HEAD、Docker 镜像 digest、model、prompt SHA256、环境变量、构建文件 hash。

**为什么**：可复现性。日后某个 task 结果异常时能回溯到当时的版本。

### Step 4 — 文件快照

**做什么**：`find -type f -printf` 扫描工作区所有文件的（路径, 大小, mtime）。

**为什么**：Claude 运行前的基准。权限虽然锁定了非目标文件，但快照对比是便宜的保险——万一权限设置有 bug，snapshot 对比能发现。

---

## 执行阶段

### Step 5 — 执行 Claude

**做什么**：Docker 容器里跑 Claude Code CLI，stdin pipe 传入 prompt，`readline()` 实时监视输出。

**为什么**：
- 容器隔离：`--cap-drop ALL`、非 root 用户、资源限制
- 每次新容器（`--rm`）：不跨 task 复用，避免 session 状态泄漏
- `-p "$(cat)"` 从 stdin 读取 prompt：避免命令行注入和 shell 转义
- `claude-on-completion` hook 输出 `=== RIOT_TASK_COMPLETED ===`——hook 是 CLI 机制不由 LLM 控制，比依赖模型自觉输出标记更可靠
- `--tmpfs /tmp:size=1g,exec`——`exec` 标志必须加，否则编译产物无法执行

**注意**：
- Claude 容器需要网络（调 API），不加 `--network none`
- stdout 用 `readline()` 阻塞读取，不用 `communicate()`——后者等进程结束才返回
- 环境变量从 Windows 侧 `settings.json` 的 `env` 块读取，白名单通过后传入 Docker
- `WATCHDOG_TIMEOUT` (8h) 是安全保险丝，只在 Claude CLI 死锁、Docker 卡死、pipe 挂起等异常触发，不是任务预算

---

## 校验阶段

### Step 6 — 文件完整性检查

**做什么**：对比 Claude 运行前后的文件快照，列出创建/修改/删除的文件（排除目标文件）。

**为什么**：先确认改了什么，再决定读什么——顺序反了没意义。

### Step 7 — 读取最终源码

**做什么**：读取 Claude 编辑后的目标文件。

### Step 8 — 生成 Diff

**做什么**：`difflib.unified_diff(masked, final)`。

**为什么**：精确记录哪些字节变了——后续人工审查和结果分析的基础。

**注意**：diff 区分不了"Claude reformat 了整个文件但只改了函数体"和"改了别的东西"。前者合法后者违规，需要 Step 9 做语义判断。

### Step 9 — AST 完整性校验

**做什么**：两层检查。

第一层：`normalize_except_body()` 将目标函数体替换为占位符、参数名替换为 `p0, p1, ...`，比较 masked 和 final 归一化后是否一致。一致 = 只改了函数体。

第二层：`check_body_integrity()` 检查函数体是否包含 `#define`、`#undef`、`#include`、`typedef` 等禁止关键字。

**为什么**：
- 归一化对比：diff 看字节，AST 看语义。两者互补
- 参数名泛型化：防止 `len → str_len` 这种合法重命名被误杀
- 关键字检查：防止预处理指令作弊（`#define result 0; return result;` 编译能过但本质耍赖）

**注意**：AST 解析失败时 fail-open。RIOT 有非常规 C 宏会让 tree-sitter 解析失败，fail-close 会导致大量误杀。

---

## 测试阶段

### Step 10 — 编译与测试

**做什么**：独立断网容器里 `make clean && make && run_test`。

流程：
1. 挂载原始 repo 的 `tests/` 到 `/riot_tests:ro`
2. `cp -r /riot_tests /workspace/tests`（物理复制，不能用符号链接——否则 Makefile.include 的相对路径 `../../Makefile.include` 解析会断裂）
3. `make -C tests/unittests BOARD=native64 clean` 清除旧编译产物
4. 执行 task 的 `run_command`
5. 分析输出：编译失败 → 崩溃 → 目标测试失败，逐级判断

**为什么**：
- 独立容器：与 Claude 容器物理隔离
- 断网：测试不需要网络，有网是变量
- 三级判断：逐级递进，准确定位失败原因
- 只看目标测试：评判标准聚焦"task 是否完成"

---

## 收尾阶段

### Step 11 — 轨迹日志

**做什么**：将 Claude 输出、diff、违规信息写入 `trajectory/task_{id}.log`。

**注意**：Claude 输出可能很长（几 MB），超过 `MAX_OUTPUT_LENGTH` 时截断为头尾各半。

### Step 12 — 结构化结果

**做什么**：构造 JSON 写入 `results/results.jsonl`，包含：通过状态、耗时、违规、文件篡改、diff、环境快照、清理错误。

### Step 13 — 清理与验证

**做什么**：删工作区 → 验证清理 → git reset。

两级回退：
1. `shutil.rmtree(workspace_dir)` — 正常情况直接删除
2. 若 PermissionError → Docker 容器以 root `chmod -R a+rwX` 然后 `rm -rf` — 处理 Docker 创建的 root 所有文件
3. 回退失败时检查 returncode + 目录是否残留，残留抛 `RuntimeError`

验证清理检查三项：目录删了没、容器有没有残留、git 干不干净。每个失败打印 `[CLEANUP WARNING]` 并写入 `cleanup_errors`——不能静默跳过，否则状态泄漏无迹可循。

最后 `hard_reset_repo(repo_root, clean=True)` 确保下一个 task 从干净代码开始。
