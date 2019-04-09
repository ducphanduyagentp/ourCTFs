from PIL import Image
import collections
import sys

fname = sys.argv[1]
img = Image.open(fname)
px = img.load()
sz = img.size
mag = 2
print sz
fnew = 'fixed_' + fname

for i in range(sz[1]):
    d = collections.deque()
    for j in range(sz[0]):
        d.append(px[j, i])
    d.rotate(mag * i)
    for j in range(sz[0]):
        px[j, i] = d[j]
img.save(fnew, "BMP")
