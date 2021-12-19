#!/usr/bin/env python3
# transform_template.py
from pathlib import Path
import os
import sys
from typing import List, Tuple, Dict

macro_sizes: Dict[str, int] = {}


def transform_interpolation(raw_value_str: str) -> Tuple[str, int]:
    """
    return: (new_code_str, estimated_size)
    """
    # most of these sizes are just an arbitrary guess
    for macro_prefix, estimated_size in macro_sizes.items():
        if raw_value_str.startswith(macro_prefix):
            return raw_value_str, estimated_size
    if raw_value_str.startswith('%'):
        fmt, value = raw_value_str.split(' ', 1)
        return f'response->printf("{fmt}", {value});', 8
    if raw_value_str.startswith('code'):
        unused, code_str = raw_value_str.split(' ', 1)
        return code_str, 8
    return f'response->print({raw_value_str});', 8


def transform_literal_content_to_code(literal_content: str) -> str:
    if not literal_content:
        return ''
    escaped = (literal_content
               .replace('"', '\\"')
               .replace('\n', '\\n'))
    if escaped.endswith('\\n'):
        return f'response->println("{escaped[:-2]}");'
    else:
        return f'response->print("{escaped}");'


def remove_line_whitespace(s: str) -> str:
    if not s:
        return s
    leading = '' if s.lstrip() == s else ' '
    trailing = '' if s.rstrip() == s else ' '
    inner = ' '.join(line.strip() for line in s.strip().split('\n'))
    out = leading + inner + trailing
    if out == '  ':
        return ' '
    else:
        return out




def partial_transform_template_to_code(template_str: str) -> Tuple[str, str, int]:
    """
    return: (remaining_template_str, new_code_str, estimated_size)
    """
    if ('{{' in template_str and '}}' in template_str) and (template_str.index('{{') > template_str.index('}}')):
        assert False, "template string has mis-ordered double-curly-brackets!"
    if ('{{' in template_str) != ('}}' in template_str):
        assert False, "template string has unbalanced double-curly-brackets!"

    open_bracket_index = template_str.index('{{') if '{{' in template_str else len(template_str)+1
    begin_macro_index = template_str.index('#macro ') if '#macro ' in template_str else len(template_str)+1

    if template_str.startswith('\n'):
        return partial_transform_template_to_code(template_str[1:])
    if template_str.startswith('{{'):
        raw_value_str, remaining_template_str = template_str[2:].split('}}', 1)
        new_code_str, estimated_size = transform_interpolation(raw_value_str)
        return remaining_template_str, new_code_str, estimated_size

    elif template_str.startswith('#macro '):
        macro_begin_line, remaining_template_str = template_str.split('\n', 1)
        macro_lines: List[str] = [macro_begin_line]
        while macro_lines[-1] != '#endmacro':
            # this can happen if there is no trailing newline in the file
            if remaining_template_str == '#endmacro':
                macro_lines.append(remaining_template_str)
                remaining_template_str = ''
                break
            next_line, remaining_template_str = remaining_template_str.split('\n', 1)
            macro_lines.append(next_line)
            if next_line == '#endmacro':
                break
        new_code_lines, estimated_size = transform_template_to_code_body('\n'.join(macro_lines[1:-1]))
        macro_declaration = macro_begin_line.split(' ', 1)[1]
        # print(f'{macro_declaration} {estimated_size=}', file=sys.stderr)
        macro_name_with_parenthesis = macro_declaration.split('(')[0] + '('
        macro_sizes[macro_name_with_parenthesis] = estimated_size
        new_code_str = (
            '#define ' + macro_declaration + ' \\\n    '
            + ' \\\n    '.join(new_code_lines)
        )
        return remaining_template_str, new_code_str, 0  # macro has 0 size until it's invoked

    elif '#macro ' in template_str and begin_macro_index < open_bracket_index:
        literal_content, remaining_template_str_incomplete = template_str.split('#macro ', 1)
        # strip whitespace from lines of literal content
        literal_content = remove_line_whitespace(literal_content)
        return (
            ('#macro ' + remaining_template_str_incomplete),
            transform_literal_content_to_code(literal_content),
            len(literal_content)
        )

    elif '{{' in template_str:
        literal_content, remaining_template_str_incomplete = template_str.split('{{', 1)
        # strip whitespace from lines of literal content
        literal_content = remove_line_whitespace(literal_content)
        return (
            ('{{' + remaining_template_str_incomplete),
            transform_literal_content_to_code(literal_content),
            len(literal_content)
        )

    else:
        # template processing is done
        return '', transform_literal_content_to_code(remove_line_whitespace(template_str)), len(template_str)


def transform_template_to_code_body(template_str: str) -> Tuple[List[str], int]:
    """
    return: (code_lines, total_size_guess)
    """
    code_lines: List[str] = []
    remaining_template_str = template_str
    total_size_guess = 0
    while remaining_template_str:
        remaining_template_str, new_code_str, size_guess = partial_transform_template_to_code(remaining_template_str)
        code_lines.append(new_code_str)
        total_size_guess += size_guess
    return code_lines, total_size_guess


def transform_template_to_code(template_str: str, name: str) -> str:
    code_lines, total_size_guess = transform_template_to_code_body(template_str)

    all_macros = True
    for code_line in code_lines:
        if not code_line.strip():
            continue
        if code_line.isspace():
            continue
        if not code_line.startswith('#define '):
            all_macros = False
            break
    if all_macros:
        return '\n'.join(code_lines) + '\n'

    output_lines = []
    output_lines.append(f'#define {name.upper()}_SIZE_ESTIMATE {total_size_guess}')
    output_lines.append(f'void write_{name}(AsyncResponseStream* response) {{')
    for code_line in code_lines:
        if not code_line:
            continue
        if code_line.startswith('#define '):
            output_lines.append('\n' + code_line + '\n')
        else:
            output_lines.append('   ' + code_line.replace('\n', '\n        '))
    output_lines.append('}')
    output_lines.append('')

    return '\n'.join(output_lines)


if __name__ == '__main__':
    print('// this is generated code, do not edit!')
    print('// generated by web/transform_template.py')
    print('')
    for arg in sorted(sys.argv[1:]):
        inpath = Path(arg)
        name = inpath.stem
        code = transform_template_to_code(inpath.read_text(), name)
        print(f'// source: {inpath.name}')
        print(code)
