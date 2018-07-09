from pwn import *
from string import printable

def get_char(c):
    global trans_tbl
    for i in range(0xfe + 1):
        if trans_tbl[i * 2] == c:
            return trans_tbl[i * 2 + 1]
    return 0

trans_tbl = open('trans_tbl').read()
password = ''
password += p64(0xb1e711f59d73b327)
password += p64(0x30f4f9f9b399beb3)
password += p64(0xb19965237399711b)
password += p64(0xf9279923be111165)
password += p64(0x65059923)
password = password.strip('\x00')

flag = ''
for i in range(len(password)):
    for c in printable:
        if get_char(c) == password[i]:
            flag += c
            break
print flag

