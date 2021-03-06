# 2017.06.18 10:07:15 EDT
#Embedded file name: hasher.py
import sys
from binascii import unhexlify, hexlify
from hashlib import md5

def string_to_int(string):
    out = 0
    for c in string:
        out <<= 8
        out |= ord(c)

    return out


def int_to_string(integer):
    out = ''
    while integer > 0:
        out = chr(integer & 255) + out
        integer >>= 8

    return out


class ZXHash:
    key1 = None
    key2 = None

    def __init__(self, key1, key2):
        self.key1 = key1
        self.key2 = key2

    def hash(self, inp):
        string = self.key1 + inp
        string = string + (64 - len(string) % 64) * '0'
        value = int(string, 16)
        s = 0
        while value > 0:
            s = s ^ value & pow(2, 256) - 1
            value = value >> 256

        b4 = s & pow(2, 64) - 1
        s = s >> 64
        b3 = s & pow(2, 64) - 1
        s = s >> 64
        b2 = s & pow(2, 64) - 1
        s = s >> 64
        b1 = s & pow(2, 64) - 1
        hsh = md5(int_to_string(b4)).digest()[:8]
        m = string_to_int(hsh)
        b3 = b3 % m
        e = pow(self.key2, 128 + b3, m)
        return hex((b1 ^ b2 ^ e) % m)[2:-1]
+++ okay decompyling hasher.pyc 
# decompiled 1 files: 1 okay, 0 failed, 0 verify failed
# 2017.06.18 10:07:15 EDT
