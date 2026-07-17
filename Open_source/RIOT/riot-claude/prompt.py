"""
Prompt 模板 —— 发给 Claude Code CLI 的任务描述。

通过 str.format() 注入函数名和文件路径，让 Claude 知道要补全哪个函数。
"""

TASK_PROMPT = """Implement the missing function `{sut_function}` in `{source_path}`.

The function body is currently replaced with a TODO comment.

RULES:
- Only edit the file {source_path}
- Do NOT modify any other files (they are read-only)
"""
