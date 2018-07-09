#!/usr/bin/env python

import signal, random
import sys
d = {}

class LinearCongruentialGenerator:

    def __init__(self, a, b, nbits):
        self.a = a
        self.b = b
        self.nbits = nbits
        self.state = random.randint(0, 1 << nbits)

    def nextint(self):
        self.state = ((self.a * self.state) + self.b) % (1 << self.nbits)
        return self.state >> (self.nbits - 32)


if __name__ == "__main__":

    with open("flag", "r") as f:
        flag = f.read()

    multiplier = 0x66e158441b6995
    addend = 0xB
    nbits = 85    # should be enough to prevent bruteforcing
    generator = LinearCongruentialGenerator(multiplier, addend, nbits)

    points = 10

    print "Welcome!"
    print "Do you feel lucky? Try to guess the numbers I'm thinking of."
    print "You have one minute to reach 20 points. Good Luck!"
    sys.stdout.flush()
    #signal.alarm(60)	
    while points > 0 and points < 20:
        #print "You have {} points.".format(points)
        sys.stdout.flush()
        current_num = generator.nextint()
	if current_num in d:
		points = 20
		print "LEN: {}".format(len(d))
		break
	d[current_num] = 1
	
	print current_num
	continue
        guess_num = None
        while guess_num is None:
            try:
                print "Guess the next number:"
                sys.stdout.flush()
                guess_num = int(raw_input())
            except ValueError:
                print "That's not a valid number!"
                sys.stdout.flush()
        if (guess_num == current_num):
            print "That's correct!"
            sys.stdout.flush()
            points = points + 1;
        else:
            print "Wrong, the correct number was {}.".format(current_num)
            sys.stdout.flush()
            points = points -1;

    if points <= 0:
        print "You lost!"
        sys.stdout.flush()
    elif points >= 20:
        print "Congratulations, you won!!"
        print "Here is your flag: {}".format(flag)
        sys.stdout.flush()
    else:
        print "Something went wrong..."
        sys.stdout.flush()
