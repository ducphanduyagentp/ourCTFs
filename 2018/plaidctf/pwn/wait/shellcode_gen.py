from pwn import *

def send_bytes(b):
    global s
    b = b.strip().split(' ')
    for x in b:
        print s.recv()
        s.sendline(x)

context.arch = 'amd64'
s = remote('wwdsm.chal.pwning.xxx', 6615)
print s.recv()


