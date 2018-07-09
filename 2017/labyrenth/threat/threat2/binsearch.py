import subprocess

try:
	subprocess.call('ls -rS labyrenth > filenames.txt', shell=True)
except:
	print 'Error getting filenames'

prefix = 'labyrenth/'
filenames = [prefix + line.strip() for line in open('filenames.txt')]
files = []
for filename in filenames:
	print filename
	files.append([])
	with open(filename, 'rb') as inp:
		byt = inp.read(1)
		while byt != '':
			files[-1].append(format(ord(byt), '02x')[0])
			files[-1].append(format(ord(byt), '02x')[1])
			byt = inp.read(1)
	files[-1] = ''.join(files[-1])
	inp.close()

for start in range(len(files[0])):
	st = start
	end = len(files[0])
	while st < end:
		mid = (st + end) >> 1
		mid += 1
		#print st, mid, end
		flag = True
		for string in files:
			if string.find(files[0][start:mid]) == -1:
				flag = False
				break
		if flag:
			st = mid
		else:
			end = mid - 1
	print files[0][start:end]
