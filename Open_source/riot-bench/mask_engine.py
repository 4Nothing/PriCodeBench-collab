def mask_function(func_text: str):
    lines = func_text.split("\n")

    if len(lines) < 1:
        return None, None

    body_start = -1
    for i, line in enumerate(lines):
        if '{' in line:
            body_start = i
            break

    if body_start == -1:
        return None, None

    body_end = -1
    brace_count = 0
    for i, line in enumerate(lines):
        if '{' in line:
            brace_count += line.count('{')
        if '}' in line:
            brace_count -= line.count('}')
            if brace_count == 0:
                body_end = i
                break

    if body_end == -1 or body_end <= body_start:
        return None, None

    # 保留函数签名和 { 之前的内容
    result_lines = lines[:body_start + 1]
    # 添加单个 TODO
    result_lines.append("    /* TODO(agent) */")
    # 添加 } 所在的行
    result_lines.append(lines[body_end])

    masked = "\n".join(result_lines)
    target = func_text

    return masked, target
