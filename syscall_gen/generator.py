from urllib.request import urlopen
from bs4 import BeautifulSoup

import sys

DATABASE_URL = 'https://filippo.io/linux-syscall-table'

CONST_QUALIFIER = 'const'
SYMBOLS = {
    'int':              'int_',
    's32':              'int_',
    'key_serial_t':     'int_',
    'clockid_t':        'int_',
    'pid_t':            'int_',
    'key_t':            'int_',

    'unsigned':         'uint_',
    'unsigned int':     'uint_',
    'u32':              'uint_',
    'uid_t':            'uint_',
    'qid_t':            'uint_',
    'gid_t':            'uint_',
    'umode_t':          'uint_',

    'long':             'long_',
    'off_t':            'long_',
    'loff_t':           'long_',

    'unsigned long':    'ulong_',
    'u64':              'ulong_',
    'size_t':           'ulong_',
    'timer_t':          'ulong_',
    'mqd_t':            'ulong_',
    'aio_context_t':    'ulong_',

    'void *':           'pointer_',
    'cap_user_header_t':'pointer_',
    'cap_user_data_t':  'pointer_',

    'char *':           'string_',

    'char **':          'array_',
}
UNKNOWN_SYM = '__unknown_type'

def strip_type(type):
    # Handle pointers
    if '*' in type:
        if 'char' in type:
            return 'char {}'.format('*' * type.count('*'))
        else:
            return 'void *'
    # Remove const qualifiers
    if type.startswith(CONST_QUALIFIER):
        type = type[len(CONST_QUALIFIER) + 1:]
    # Remove __
    if type.startswith('__'):
        type = type[2:]
    return type

def type_to_sym(type):
    stripped = strip_type(type)
    try:
        return SYMBOLS[stripped]
    except KeyError:
        print('unmapped type: {} ({})'.format(stripped, type), file=sys.stderr)
        return UNKNOWN_SYM

with urlopen(DATABASE_URL) as response:
    html = response.read()
    soup = BeautifulSoup(html, 'html.parser')
    table = soup.find(class_='tbls-table')
    entries = table.find_all(class_='tbls-entry-collapsed')

    for entry in entries:
        defines = entry.find_all('td')
        name = defines[1].string
        return_type = 'long' # Cannot think of a clean way to fetch this info
        args_row = entry.next_sibling
        args = []
        if 'tbls-arguments-collapsed' in args_row.attrs.get('class'):
            types_row = args_row.find_all('tr')[1]
            type_defines = types_row.find_all('td')
            for define in type_defines:
                type = define.find('strong')
                args.append(type.string)

        syscall = '{{ "{}", {}, {}, {{ {} }} }},'.format(
            name,
            type_to_sym(return_type),
            len(args),
            ', '.join(map(type_to_sym, args))
        )
        print(syscall)
