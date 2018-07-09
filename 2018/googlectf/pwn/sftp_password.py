src = 0x8DFA
dst = 0x5417

found = False

def DFS(v4, h):
    global found
    if h >= 15:
        return None
    if v4 == dst and h == 14:
        found = True
        return None
    x = v4 / 2
    for v3 in xrange(0x20, 0x7f):
        print "Checking {} - {}".format(v3, h)
        if (x ^ v3) % 2 == 0:
            DFS(x ^ v3, h + 1)
            if found:
                print v3
                return

DFS(src, 0)
print "Done"
