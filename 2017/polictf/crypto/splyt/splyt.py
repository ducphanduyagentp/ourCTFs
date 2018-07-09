#!/usr/bin/env python3
import sys
import json
from Splyt import Splyt


def print_help():
    print("Usage: \n"
          "%s " % (sys.argv[0], ) + "split <file> <n> <threshold> - outputs a json file with the shares\n" +
          "%s " % (sys.argv[0], ) + "join <shares json>\n"
          )

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print_help()
        exit()

    if sys.argv[1] == 'split':
        if len(sys.argv) != 5:
            print_help()
            exit()

        n = int(sys.argv[3])
        threshold = int(sys.argv[4])
        if n < threshold:
            print("n must be greater than the threshold!")
            print_help()
            exit()

        with open(sys.argv[2]) as f:
            flag = f.read()

        shares = Splyt.split(flag, n, threshold)
        print(json.dumps(shares))

    elif sys.argv[1] == 'join':
        with open(sys.argv[2]) as f:
            shares = json.load(f)

        reconstructed = Splyt.join(shares)
        offset = ord(reconstructed[4]) - ord('{')
        for c in reconstructed:
            print(chr(ord(c) - offset), end='')
        print(reconstructed, end='')

    else:
        print_help()
        exit()

