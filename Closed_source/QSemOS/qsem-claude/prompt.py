"""
Prompt 模板 —— 发给 Claude Code CLI 的任务描述。
"""

TASK_PROMPT = """Implement the missing function `{sut_function}` in `{source_path}`.

The function body is currently replaced with a placeholder comment. Read the surrounding
code and header files to understand the expected behavior, then complete the implementation.

RULES:
- Only edit the file {source_path}
- Do NOT modify any other files
- Do NOT delete or modify any existing code outside the target function body
"""
