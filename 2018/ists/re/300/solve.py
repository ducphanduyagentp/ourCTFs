
def encChar(c, idx):
    n1 = c + idx
    n1 = (n1 ^ 0x16)
    n1 = n1 & 0xff
    n1 *= 0x202020202
    n1 &= 0x10884422010
    n1 %= 1023
    n1 &= 0xff
    return n1


buff = 'DEE9B4C4020242FA'.decode('hex')[::-1]
buff += '1E36F6DE3E362646'.decode('hex')[::-1]
buff += '393916062686F649'.decode('hex')[::-1]
buff += '0AC1'.decode('hex')[::-1]

flag = ''

for i in range(len(buff)):
    for k in range(0, 256):
        if ord(buff[i]) == encChar(k, i):
            flag += chr(k)

print flag
