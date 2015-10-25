from urllib.request import urlopen
from bs4 import BeautifulSoup

DATABASE_URL = 'https://filippo.io/linux-syscall-table'

with urlopen(DATABASE_URL) as response:
    html = response.read()
    soup = BeautifulSoup(html, 'html.parser')
    table = soup.find(class_='tbls-table')
    entries = table.find_all(class_='tbls-entry-collapsed')

    for entry in entries:
        defines = entry.find_all('td')
        name = defines[1].string
        args_row = entry.next_sibling
        args = []
        if 'tbls-arguments-collapsed' in args_row.attrs.get('class'):
            types_row = args_row.find_all('tr')[1]
            type_defines = types_row.find_all('td')
            for define in type_defines:
                type = define.find('strong')
                args.append(type.string)

        print('{%s, %s}' % (name, ', '.join(args)))

