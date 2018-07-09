import string

s2 = open('s2', 'r').readline()
flag = ''
for i in range (32):
    for c in string.printable:
        n = ord(c)
        v1 = 2 * (n & 0x55) | (n >> 1) & 0x55
        v1 &= 0xff
        v2 = 4 * (v1 & 0x33) | (v1 >> 2) & 0x33
        n = 16 * v2 | (v2 >> 4)
        n = ~n
        n &= 0xff
        if n == ord(s2[i]):
            flag += c
print flag[::-1]
