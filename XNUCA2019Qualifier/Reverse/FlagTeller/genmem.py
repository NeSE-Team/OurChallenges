#!/usr/bin/python2 -i

# variable

flag = 'flag{aef5b6eea4c49d867bf08d95e45dcea4___10v3_53qu3n7ia1_109ic_}\n'

good_key1 = 0x317520b6ad7be159
good_key2 = 0x3eae0f3ca48c27be


# from code, const

pols = [0x30410c084000aa3, 0x283, 0x80000000001005]
ys = [0x12345678deadc32f, 0x113355770044baef, 0xceba1234deaf2019]
start_key = [0xc456a1adda0b49cf]


def b2i(b):
    return sum(j << (8*i) for i, j in enumerate(bytearray(b)))


def i2b(i, n):
    return bytearray([0xff & (i >> (8*j)) for j in range(n)])


def ffmul(x, y, pol):
    x = x & ((1 << 64)-1)
    y = y & ((1 << 64)-1)
    pol = pol & ((1 << 64)-1)
    res = 0
    while x:
        if x & 1:
            res ^= y
        x = x >> 1
        y = y << 1
        if 1 & (y >> 64):
            y = y ^ pol ^ (1 << 64)
    return res


def ffpow(x, e, pol):
    res = 1
    pow = x
    while e:
        if e & 1:
            res = ffmul(res, pow, pol)
        pow = ffmul(pow, pow, pol)
        e >>= 1
    return pow


def ffinv(x, pol):
    return ffpow(x, 2**64-2, pol)


def encrypt_to_il(s, k):
    l = [b2i(s[i*8:(i+1)*8]) for i in range(len(s)//8)]
    ll = []
    for i in l:
        ll.append(k ^ i)
        k = ffmul(k, ys[2], pols[2])
    return ll


def send_char(c):
    ci = c
    l = []
    l.append('uart = 1;\n#2080\nuart = 0;')
    for i in range(8):
        l.append('#1040 uart = {:d};'.format((ci >> i) & 1))
    l.append('#1040 uart = 1;\n#1040')
    return '\n'.join(l)+'\n// -----\n\n'


final_il = encrypt_to_il(flag, ffmul((good_key1+good_key2) & ((1 << 64)-1), ys[2], pols[2])) + \
    encrypt_to_il('Try again plz..\n', start_key[0])
final_il += [ffmul(ffmul(good_key2, ys[1], pols[1]), ys[0], pols[0]),
             ffmul(ffmul(good_key1, ys[0], pols[0]), ys[1], pols[1])]

s = ''.join(map(lambda x: str(i2b(x, 8)), final_il))

with open('memory.bin', 'wb') as f:
    f.write(''.join('{:08b}\n'.format(ord(i)) for i in s))

with open('inputbench.v', 'w') as f:
    f.write(''.join(send_char(42 ^ i)
                    for i in i2b(good_key1, 8)+i2b(good_key2, 8)))
    f.write('\n\n# 2000000\n\n')
    f.write(''.join(send_char(42 ^ i)
                    for i in i2b(good_key2, 8)+i2b(good_key1, 8)))
    f.write('\n\n# 500000\n\n')
