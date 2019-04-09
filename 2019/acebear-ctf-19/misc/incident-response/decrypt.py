from __future__ import print_function
from scapy.all import *

k = '%$&^(*!@'
packets = rdpcap('data.pcapng')
d = [p for p in packets if p.haslayer(TCP)]
d = [p for p in d if p[IP].dst == '13.229.248.109']
d = [str(p[TCP].payload) for p in d if len(p[TCP].payload) > 0]

start_idx = []
for i in range(len(d)):
    if d[i].startswith('\x31\x00\x0b\x00'):
        start_idx.append(i)

start_idx.append(len(d))

print(start_idx)

for i in range(len(start_idx) - 1):
    fname = d[start_idx[i]][5 : 4 + 11] + '.bmp'
    fout = open(fname, 'wb')
    d1 = d[start_idx[i] + 1 : start_idx[i + 1]]
    x = ''
    for p in d1:
        if p.startswith('\x32\x00\x00\x04'):
            x += p[4:]
        else:
            x += p
    x1 = ''
    for i in range(len(x)):
        x1 += chr(ord(x[i]) ^ ord(k[i%len(k)]))
    fout.write(x1)
    fout.close()
    print('Done ' + fname)
