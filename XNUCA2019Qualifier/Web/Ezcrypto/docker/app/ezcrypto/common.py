import sys
import time
import marshal
import types
import signal
import hmac 
import hashlib
import secret
from Crypto.Util import number
from Crypto.Hash import MD5
import binascii
import re


def create_key(lowlimit, uplimit):
    if lowlimit < 0.25 or uplimit < 0.25:
        return (0, 0)
    if lowlimit > 0.4 or uplimit > 0.4:
        return (0, 0)
    Nsize = 1280
    pqsize = Nsize//2
    N = 0 
    while(N.bit_length()!=Nsize):
        while True:
            p = number.getStrongPrime(pqsize)
            q = number.getStrongPrime(pqsize)
            if abs(p-q).bit_length() > (Nsize*0.496):
                break
        N = p*q
    phi = (p-1)*(q-1)
    while True:
        d = number.getRandomRange(pow(2,int(Nsize*lowlimit)),pow(2,int(Nsize*uplimit)+1))
        if number.GCD(d, phi)!=1:
            continue
        e = number.inverse(d, phi)
        if number.GCD(e, phi)!=1:
            continue
        break
    return (N, e)


def md5(input):
    h = MD5.new()
    h.update(input.encode(encoding="utf-8"))
    return h.hexdigest()


def fastExpMod(b, e, N):
    result = 1
    while e != 0:
        if (e & 1) == 1:
            result = (result * b) % N
        e >>= 1
        b = (b*b) % N
    return result


def crypt(message, N, e):
    b = int(binascii.b2a_hex(message.encode(encoding="utf-8")), 16)
    return fastExpMod(b, e, N)


def wafs(input):
    pattern = r'Nroot|Eroot|Nuser|Euser|insert|update|;|join|order|sleep|\\|\/|\*|\.|-'
    return re.search(pattern, input, re.I)


def wafd(input):
    pattern = r'Nroot|Eroot|Nuser|Euser|insert|update|join|order|sleep|random|#|\$|%|\*|\+|-|\.|\/|:|;|<|>|\?|@|\[|\\|]|\^|_|`|{|\||}|~'
    return re.search(pattern, input, re.I)