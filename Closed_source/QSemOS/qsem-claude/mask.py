"""
tree-sitter C 函数 AST 操作

功能：
  1. 查找目标函数 → apply_mask() 将函数体替换为 /* MASKED */
  2. 归一化等价性检查 → 比较 mask 版本和最终版本是否只有目标函数体变化
  3. 关键字检查 → 防止 #define / #include / extern / typedef 等作弊
"""
from pathlib import Path
import re
from typing import Optional
from tree_sitter import Language, Parser
import tree_sitter_c


def _char_offset(text: str, byte_offset: int) -> int:
    return len(text.encode('utf-8')[:byte_offset].decode('utf-8', 'replace'))


def _name_at(source: str, node) -> str:
    return source[_char_offset(source, node.start_byte):_char_offset(source, node.end_byte)]


_C_LANG = Language(tree_sitter_c.language())
_PARSER = Parser(_C_LANG)


def _parser_smoke_test():
    test_src = b'int foo(void) { return 1; }'
    tree = _PARSER.parse(test_src)
    root = tree.root_node
    fn_nodes = [child for child in root.children if child.type == 'function_definition']
    if not fn_nodes:
        raise RuntimeError(
            "tree-sitter parser smoke test FAILED: could not parse "
            "'int foo(void) { return 1; }'."
        )
    fn = fn_nodes[0]
    body = fn.child_by_field_name('body')
    if body is None:
        raise RuntimeError(
            'tree-sitter parser smoke test FAILED: parsed function but found no body node.'
        )


_parser_smoke_test()


def _find_declarator_identifier(node):
    if node.type == 'identifier':
        return node
    child = node.child_by_field_name('declarator')
    if child:
        return _find_declarator_identifier(child)
    for child in node.children:
        result = _find_declarator_identifier(child)
        if result is not None:
            return result
    return None


def _find_function(source: str, fn_name: str):
    tree = _PARSER.parse(bytes(source, 'utf8'))
    root = tree.root_node
    stack = [root]
    while stack:
        node = stack.pop()
        if node.type == 'function_definition':
            declarator = node.child_by_field_name('declarator')
            if declarator:
                id_node = _find_declarator_identifier(declarator)
                if id_node is not None:
                    name = _name_at(source, id_node)
                    if name == fn_name:
                        return node
        stack.extend(node.children)
    return None


def apply_mask(path: Path, fn_name: str, masked_code: str):
    """将文件中指定函数的函数体替换为 /* MASKED */ 注释。"""
    source = path.read_text()
    fn_node = _find_function(source, fn_name)
    if fn_node is None:
        raise RuntimeError(f'Function not found: {fn_name}')

    body = fn_node.child_by_field_name('body')
    if body is not None:
        start = _char_offset(source, body.start_byte + 1)
        end = _char_offset(source, body.end_byte - 1)
        stub = '\n    /* MASKED - implement this function */\n'
        new_source = source[:start] + stub + source[end:]
    else:
        new_source = (
            source[:_char_offset(source, fn_node.start_byte)]
            + masked_code
            + source[_char_offset(source, fn_node.end_byte):]
        )

    path.write_text(new_source)


def extract_function_body(source: str, fn_name: str) -> Optional[str]:
    """从源码中提取目标函数的函数体文本（不含外层大括号）。"""
    fn_node = _find_function(source, fn_name)
    if fn_node is None:
        return None
    body = fn_node.child_by_field_name('body')
    if body is None:
        return None
    start = _char_offset(source, body.start_byte + 1)
    end = _char_offset(source, body.end_byte - 1)
    return source[start:end]


def normalize_except_body(source: str, fn_name: str) -> str:
    """归一化目标函数：替换函数体 + 重命名参数 + 签名空白归一化。"""
    try:
        fn_node = _find_function(source, fn_name)
    except Exception:
        return source

    if fn_node is None:
        return source

    body = fn_node.child_by_field_name('body')
    if body is None:
        return source

    fn_start = _char_offset(source, fn_node.start_byte)
    body_start = _char_offset(source, body.start_byte)
    body_end = _char_offset(source, body.end_byte)

    replacements = []
    replacements.append((body_start, body_end, '{ /* BENCHMARK BODY */ }'))

    declarator = fn_node.child_by_field_name('declarator')
    if declarator is not None:
        params = declarator.child_by_field_name('parameters')
        if params is not None:
            param_idx = 0
            for child in params.children:
                if child.type == 'parameter_declaration':
                    type_node = child.child_by_field_name('type')
                    is_void = False
                    if type_node is not None:
                        type_text = source[
                            _char_offset(source, type_node.start_byte):
                            _char_offset(source, type_node.end_byte)
                        ]
                        is_void = type_text == 'void'

                    id_node = _find_declarator_identifier(child)
                    if id_node is not None:
                        start = _char_offset(source, id_node.start_byte)
                        end = _char_offset(source, id_node.end_byte)
                        replacements.append((start, end, f'p{param_idx}'))
                    elif not is_void:
                        end = _char_offset(source, child.end_byte)
                        replacements.append((end, end, f' p{param_idx}'))

                    param_idx += 1

    replacements.sort(key=lambda x: x[0], reverse=True)

    result = source
    for start, end, text in replacements:
        result = result[:start] + text + result[end:]

    body_placeholder = '{ /* BENCHMARK BODY */ }'
    new_body_pos = result.find(body_placeholder)
    if new_body_pos == -1:
        return result

    signature = result[fn_start:new_body_pos]
    normalized_sig = re.sub(r'\s+', ' ', signature)
    result = result[:fn_start] + normalized_sig + result[new_body_pos:]

    return result


def check_ast_integrity(masked_source: str, final_source: str, fn_name: str) -> bool:
    """归一化等价性检查：校验最终代码是否只修改了目标函数体和参数名。"""
    try:
        masked_norm = normalize_except_body(masked_source, fn_name)
        final_norm = normalize_except_body(final_source, fn_name)
    except Exception:
        return False
    return masked_norm == final_norm


_FORBIDDEN_BODY_PATTERNS = [
    r'#\s*define',
    r'#\s*undef',
    r'#\s*include',
    r'\btypedef\b',
    r'\b__asm__\b',
    r'\b__asm\b',
    r'\b_Generic\b',
]


def check_body_integrity(body: str) -> bool:
    """检查函数体是否包含禁止的关键字。"""
    for pattern in _FORBIDDEN_BODY_PATTERNS:
        if re.search(pattern, body, re.MULTILINE):
            return False
    return True
