from pwn import *

# data -> 0x1238
# data2 -> 0x1268
data_7c00 = open('data_7c00').read()
data_7d90 = open('data_7d90').read()

def getXMMWORD(offset, data):
    xmmword = data[offset:offset + 16]
    n = 0
    for i in range(len(xmmword)):
        n |= ord(xmmword[i]) << (i * 8)
    return n

def psadbw(dest, src):
    dest = hex(dest).strip('L')[2:].rjust(32, '0').decode('hex')
    src = hex(src).strip('L')[2:].rjust(32, '0').decode('hex')
    diff = [abs(ord(a) - ord(b)) for a,b in zip(dest, src)]
    sum1 = sum(diff[:8])
    sum2 = sum(diff[8:])
    result = (sum1 << 64) | sum2
    return result

def pshufw(data, encoding=0x1e):
#    print str(hex(data))[2:]
    words = [(data >> (i * 32)) & (0x100000000 - 1) for i in range(4)]
#    print [hex(n) for n in words]
    data = words[0]
    data <<= 32
    data |= words[1]
    data <<= 32
    data |= words[-1]
    data <<= 32
    data |= words[-2]
    return data

def check(data):
    data = getXMMWORD(0, data)
    print hex(data)
    global data_7c00
    global data_7d90
    xmm0 = data
    xmm5 = getXMMWORD(0, data_7c00)
    xmm0 = pshufw(xmm0)
    for si in range(8, 0, -1):
        xmm2 = xmm0
        xmm2 &= getXMMWORD(si, data_7d90) # turn off 2 bytes
        xmm5 = psadbw(xmm5, xmm2)
        edi = xmm5 >> 64
        edi <<= 16
        edi |= xmm5 & (0x100000000 - 1)
        print hex(edi)
        toCMP = getXMMWORD(0x7da8 - 0x7d90 + 4 * (si - 1), data_7d90)
        toCMP &= (0x100000000 - 1)
        print hex(toCMP)

        if (edi >> 16) != (toCMP >> 16):
            return 1
    print ">>> CORRECT <<<"
    return 0

def brute(flag):
    if len(flag) == 8:
        if check(flag) == 0:
            print flag
            return 0
        return 1
    for c in (string.letters + string.digits + string.punctuation):
        if brute(flag + c) == 0:
            return 0
    return 1

brute('{')
