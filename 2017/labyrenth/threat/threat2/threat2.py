import subprocess

try:
	subprocess.call('ls -rS labyrenth > filenames.txt', shell=True)
except:
	print 'Error getting filenames'
template = """rule yara_challenge\n
{\n
        strings:\n
                $yara_challenge = { \n
"""
template1 = """ }\n
        condition:\n
                 all of them\n
}\n
"""

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
	inp.close()
d = dict()

for i in range(len(files)):
	if len(files[i]) not in d:
		rule = []
		d[len(files[i])] = 1
		for l in range(len(files[i])):
			flag = True
			for j in range(i, len(files)):
				if len(files[j]) != len(files[i]):
					break
				if files[j][l] != files[i][l]:
					flag = False
					break
			if flag:
				rule.append(files[i][l])
			else:
				rule.append('?')
		ruleToWrite = ''.join(rule)
		print ruleToWrite.count('?')
		with open('rule' + str(len(files[i])) + '.txt', 'w') as out:		
			pass
			out.write(template + ' '.join([a + b for a,b in zip(rule[::2], rule[1::2])]) + template1)
		out.close()

