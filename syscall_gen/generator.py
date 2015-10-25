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
        print(defines[1].string)

