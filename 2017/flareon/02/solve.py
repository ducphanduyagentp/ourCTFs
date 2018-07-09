toCMP = "\r&IE*\x17xD+l]^E\x12/\x17+DonV\t_EGs&\n\r\x13\x17HB\x01@M\f\x02i\x00"
toCMP = toCMP[:-1][::-1]

flag = ''
xorChar = 4
for c in toCMP:
    flag += chr(xorChar ^ ord(c))
    xorChar = xorChar ^ ord(c)
flag = flag[::-1]
print flag
