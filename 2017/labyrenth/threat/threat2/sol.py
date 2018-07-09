#!/usr/bin/env python

from pwn import *

filenames = [line.strip() for line in open('filenames.txt')]


rule1 = '''rule yara_challenge
{
	strings:
		$yara_challenge = { '''

rule3 = ''' }
	condition:
		 all of them
}'''

Max = 308
Start = "\x53\x56\x8b"
content = []

for i in range(len(filenames)):
	f = open('labyrenth/' + filenames[i], 'r').read()
	content.append(f)


rule2 = []

while (Max > 0):
	l = len(rule2)
	try:
		byte1 = "%02x" % ord(content[0][ content[0].index(Start) + l ])
		for filename in range(len(content)):
			byte2 = "%02x" % ord(content[filename][ content[filename].index(Start) + l ])
		
			if byte1 == byte2:
				byte1 = byte2
			elif byte1[1:] == byte2[1:]:
				byte1 = '?' + byte1[1:]
			else:
				byte1 = '??'
				break
	except:
		break

	rule2.append(byte1)
	Max -= byte1.count('?')

complete_rule = rule1 + ' '.join(rule2) + rule3
print complete_rule

r = remote('52.42.81.161', 8082)
r.send(complete_rule)
r.interactive()
print r.recv()

