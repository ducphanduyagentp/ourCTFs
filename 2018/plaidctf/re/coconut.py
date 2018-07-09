from pwn import *

s = remote('coconut.chal.pwning.xxx', 6817)

def hax():
    global s
    print s.recvuntil('Function to optimize:\n')
    code = s.recvuntil('ret\n')
#    print code
    s.recvuntil('>=')
    rr = s.recvuntil(':').strip(':')
    rr = rr.replace('=', '').replace('>','').replace('<', '').split('and')
    rr[0] = int(rr[0])
    rr[1] = int(rr[1])
    if rr[0] == 6:
        f = open('code.asm', 'w')
        f.write(code)
        f.close()
#        print code
    print rr
    print s.recv()

    code = code.split('\n')
    for i in xrange(len(code)):
        code[i] = code[i].replace('\t', ' ').split(' ')
        for j in xrange(len(code[i])):
            code[i][j] = code[i][j].strip(',')

#    code = code[4:len(code) - 3][::-1]
    keep = set()
    keep.add('%eax')
    ke = ['%eax']
    remove = []
    for i in xrange(rr[1] - 1, rr[0] - 2, -1):
        k = code[i]
 #       print k
        if k[-1] in keep:
            if k[1].startswith('mov'):
                keep.remove(k[-1])
            if ',' not in k[-2]:
                keep.add(k[-2])
            else:
                tmp = k[-2].replace('(', '').replace(')', '').split(',')
                keep.add(tmp[0].replace('r', 'e'))
                keep.add(tmp[0])
                keep.add(tmp[1].replace('r', 'e'))
                keep.add(tmp[1])
        else:
            remove.append(int(k[0]))
#    print remove
    if len(remove) > 0:
        st = remove[0]
        cur = st
        for i in xrange(1, len(remove)):
            if remove[i] == cur - 1:
                cur = remove[i]
            else:
                print '{}-{}'.format(remove[i - 1], st)
                s.sendline('{}-{}'.format(remove[i - 1], st))
                st = remove[i]
                cur = st
        print '{}-{}'.format(remove[-1], st)
        s.sendline('{}-{}'.format(remove[-1], st))
#        s.sendline(str(c))
    for c in code:
        if not rr[0] <= c[0] <= rr[1]:
            continue
        if c[0] in remove:
            code.remove(c)

############

    keep = set()
    keep.add('%eax')
    ke = ['%eax']
    remove = []
    for i in xrange(len(code) - 1, -1, -1):
        k = code[i]
        if not rr[0] <= k[0] <= rr[1]:
            continue
 #       print k
        if k[-1] in keep:
            if k[1].startswith('mov'):
                keep.remove(k[-1])
            if ',' not in k[-2]:
                keep.add(k[-2])
            else:
                tmp = k[-2].replace('(', '').replace(')', '').split(',')
                keep.add(tmp[0].replace('r', 'e'))
                keep.add(tmp[0])
                keep.add(tmp[1].replace('r', 'e'))
                keep.add(tmp[1])
        else:
            remove.append(int(k[0]))
#    print remove
    if len(remove) > 0:
        st = remove[0]
        cur = st
        for i in xrange(1, len(remove)):
            if remove[i] == cur - 1:
                cur = remove[i]
            else:
                print '{}-{}'.format(remove[i - 1], st)
                s.sendline('{}-{}'.format(remove[i - 1], st))
                st = remove[i]
                cur = st
        print '{}-{}'.format(remove[-1], st)
        s.sendline('{}-{}'.format(remove[-1], st))
#        s.sendline(str(c))

############

    s.sendline('#')
    print 'LEN: {}'.format(len(code) - len(remove))
#    print s.recvuntil('Result:')

    print s.recvuntil('\n')
    print s.recvuntil('\n')
    print s.recvuntil('\n')
    print s.recvuntil('\n')

while True:
    hax()
s.close()
