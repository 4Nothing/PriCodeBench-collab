"""
日志工具 —— 将 Claude 会话输出、diff、违规记录等追加写入轨迹文件。

轨迹文件位于 trajectory/task_{task_id}.log，
每次 pipeline 运行的不同阶段都会往里 append 内容。
"""
from pathlib import Path


def append_trajectory(path, content):
    """追加模式写入轨迹日志文件。

    自动创建父目录（如果不存在）。
    每次写入后追加两个换行，分隔不同阶段的记录。

    Args:
        path:  日志文件路径
        content: 要写入的内容
    """
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, 'a', encoding='utf-8') as f:
        f.write(content)
        f.write('\n\n')
