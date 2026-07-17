"""
tree-sitter C 函数 AST 操作

功能：
  1. 查找目标函数 → apply_mask() 将函数体替换为 /* MASKED */
  2. 归一化等价性检查 → 比较 mask 版本和最终版本是否只有目标函数体变化
  3. 关键字检查 → 防止 #define / #include / extern / typedef 等作弊

注意 byte offset 问题：
  tree-sitter 返回字节偏移，Python 字符串用字符偏移。
  对含多字节 UTF-8 的源文件（如 RIOT 中的 ä/å 字符），直接切片会错位。
  _char_offset() 通过 encode/decode 做转换。
"""
from os import fspath
from pathlib import Path
import re
from typing import Optional
from tree_sitter import Language, Parser
from config import BENCH_ROOT


def _char_offset(text: str, byte_offset: int) -> int:
    """将 tree-sitter 的字节偏移转换为 Python 字符偏移。

    tree-sitter C 库返回的是 UTF-8 字节偏移，
    但 Python 字符串索引是 Unicode 字符偏移。
    对于含多字节字符的源文件，两者不一致。
    本函数先把 text 编码为 UTF-8 字节串，
    取前 byte_offset 字节后解码回 Python 字符串，
    用其长度作为字符偏移。
    """
    return len(text.encode('utf-8')[:byte_offset].decode('utf-8', 'replace'))


def _name_at(source: str, node) -> str:
    """从 AST 节点按字节偏移提取标识符名称。"""
    return source[_char_offset(source, node.start_byte):_char_offset(source, node.end_byte)]


_LIB = BENCH_ROOT / "build" / "my-languages.so"
_C_LANG = Language(fspath(_LIB), 'c')
_PARSER = Parser()
_PARSER.set_language(_C_LANG)


def _parser_smoke_test():
    """用最简 C 函数验证 parser：解析 → 找函数 → 找 body。"""
    test_src = b'int foo(void) { return 1; }'
    tree = _PARSER.parse(test_src)
    root = tree.root_node
    fn_nodes = [child for child in root.children if child.type == 'function_definition']
    if not fn_nodes:
        raise RuntimeError(
            f"tree-sitter parser smoke test FAILED: could not parse "
            f"'int foo(void) {{ return 1; }}'. Grammar: {_LIB}, Language: {_C_LANG}"
        )
    fn = fn_nodes[0]
    body = fn.child_by_field_name('body')
    if body is None:
        raise RuntimeError(
            'tree-sitter parser smoke test FAILED: parsed function but found no body node.'
        )


_parser_smoke_test()


def _find_declarator_identifier(node):
    """递归遍历 declarator 链，找到函数名 identifier 节点。

    C 语言的函数声明 declarator 可能嵌套多层：
      int *f(int)   → pointer → function_declarator → identifier("f")
      int f[10](int)→ array → function_declarator → identifier("f")
    """
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
    """在源码的 AST 中搜索指定名称的函数定义节点。

    遍历 AST 所有 function_definition 节点，
    提取 declarator 中的 identifier 名称做匹配。

    Args:
        source: C 源码字符串
        fn_name: 目标函数名
    Returns:
        function_definition 节点或 None
    """
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
    """将文件中指定函数的函数体替换为 /* MASKED */ 注释。

    优先只替换函数体（保留签名），因为 RIOT 大量使用宏/属性修饰函数声明，
    整体替换会破坏这些修饰。
    如果找不到 body 节点（如仅有声明），回退为用 masked_code 整体替换函数定义。

    Args:
        path: 源文件路径
        fn_name: 要 mask 的函数名
        masked_code: body 不存在时的回退函数签名
    """
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
    """从源码中提取目标函数的函数体文本（不含外层大括号）。

    Args:
        source: C 源码字符串
        fn_name: 函数名
    Returns:
        函数体字符串，或 None（函数未找到/无 body）
    """
    fn_node = _find_function(source, fn_name)
    if fn_node is None:
        return None
    body = fn_node.child_by_field_name('body')
    if body is None:
        return None
    start = _char_offset(source, body.start_byte + 1)
    end = _char_offset(source, body.end_byte - 1)
    return source[start:end].strip()


def normalize_except_body(source: str, fn_name: str) -> str:
    """归一化目标函数：替换函数体 + 重命名参数 + 签名空白归一化。

    用于归一化等价性检查：对 masked 版本和 final 版本分别做归一化，
    再比较两者是否一致。如果一致，说明目标函数之外没有代码被修改。

    三阶段归一化：
      1. 函数体 → { /* BENCHMARK BODY */ }
      2. 参数名 → p0, p1, ...（仅重命名标识符，保留类型）
      3. 签名空白归一化（所有连续空白字符 → 单空格）
    """
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
    """归一化等价性检查：校验最终代码是否只修改了目标函数体和参数名。

    原理：
      1. 对 masked 版本做归一化（函数体 → 占位符，参数名 → 泛型，签名空白归一化）
      2. 对 final 版本做归一化
      3. 比较两者是否完全一致

    如果 masked_norm != final_norm，说明目标函数之外还有代码被修改了
    （函数签名语义、全局变量、其他函数等）。
    """
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
    """检查函数体是否包含禁止的关键字。

    防止 Agent 通过预处理指令或声明技巧作弊。
    例如：在函数体内 #define 一个值当返回值，或用 typedef 声明新类型。

    Args:
        body: 函数体文本（不含大括号）
    Returns:
        True = 无违规，False = 包含禁止关键字
    """
    for pattern in _FORBIDDEN_BODY_PATTERNS:
        if re.search(pattern, body, re.MULTILINE):
            return False
    return True
