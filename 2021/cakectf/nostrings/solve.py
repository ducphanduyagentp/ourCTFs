data = open('dump.dat').read()
for i in range(58):
    for b in range(0x21, 0x7f):
        x = data[127 * b + i]
        y = chr(b)
        if x == y:
            print(x, end='')
