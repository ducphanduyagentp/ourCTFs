from Crypto.Util.number import *
from hashlib import md5
import string
import itertools
import sys

flag_set = set()
key = string.letters + string.digits + string.punctuation
primes = [3, 3, 5, 5, 7, 107, 487, 607, 28429, 29287, 420577267963, 3680317203978923, 1002528655290265069]
h = 64364485357060434848865708402537097493512746702748009007197338675L

def check(flag):
	assert len(flag) == 14
	pad = bytes_to_long(md5(flag).digest())
	hack = 0
	for char in flag:
        	hack+= pad
	        hack*= ord(char)
	return hack == h

def find_flag(pos, num, flag):
	if pos == 0 and num == 0:
		if check(flag):
			print "FOUND: " + flag
			sys.exit(0)
		return
	if pos <= 0:
		return
	for c in key:
		if num % ord(c) == 0:
			find_flag(pos - 1, (num / ord(c)) - 1, c + flag)

	
def main():
	h = 64364485357060434848865708402537097493512746702748009007197338675L
	L = 14
	all_div = set()
	for l in range(len(primes) + 1):
		for subset in itertools.combinations(primes, l):
			mul = 1L
			for n in subset:
				mul *= n
			all_div.add(mul)
	assert h in all_div
	for num in all_div:
		find_flag(L, h/num, '')

if __name__ == '__main__':
	main()
