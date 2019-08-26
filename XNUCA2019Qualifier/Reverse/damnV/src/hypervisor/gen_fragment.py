import random
import zlib
import base64
import string

s = "Don't you know about the bird? Everybody knows that the bird is the word!"

def base64encode(s):
    srcbase = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
    newbase = "zy0xw1vu2ts3rq4po5nm6lk7ji8hg9fedcba,.ZYXWVUTSRQPONMLKJIHGFEDCBA"

    tran = string.maketrans(srcbase, newbase)
    encode = base64.b64encode(s)
    return map(ord, list(encode.translate(tran)))

def crc32(s):
    return [zlib.crc32(s) & 0xffffffff]


def fibnacci(num):
    a = 1
    b = 0
    for i in range(num):
        tmp = a + b
        b = a
        a = tmp
    return (a & 0xffffffffffffffff)

def calfibnaccidata(s):
    rel = []
    for c in s:
        fib = fibnacci(ord(c)) 
        rel.append(fib)
    return rel


def xorString(s):
    xor = "Bird, Bird, Bird, Bird is the word."
    rel = []
    for i in range(len(s)):
        rel.append(ord(s[i]) ^ ord(xor[i % 35]))

    return rel

def getrandlist():
    rel = []
    num = 0
    while num < len(s):
        randnum = random.randint(2, 6)
        if (num + randnum) > len(s):
            rel.append((len(s) - num))
            break
        else:
            rel.append(randnum)
            num += randnum
    
    return rel
    


def main():
    randlist = getrandlist()
    rel = []
    start = 0
    for i in randlist:
        s_frag = s[start:start+i]
        start += i

        functype = random.randint(1, 4)
        while functype == 1 and i > 3:
            functype = random.randint(1, 4)

        if functype == 1:
            t = 'CRC32'
            checkdata = crc32(s_frag)
        elif functype == 2:
            t = 'FIBNACCI'
            checkdata = calfibnaccidata(s_frag)
        elif functype == 3:
            t = 'BASE64'
            checkdata = base64encode(s_frag)
        elif functype == 4:
            t = 'XORSTRING'
            checkdata = xorString(s_frag)
        
        try:
            checkdata = map(hex, checkdata)
        except:
            print "error: " + str(checkdata)
        rel.append([t, i, checkdata])
    print rel


output = [['FIBNACCI', 4, ['0x6b04f4c2fe42L', '0x2ac6f30501d6999bL', '0x684cfe43b518ef62L', '0x6197ecbL']], ['XORSTRING', 3, ['0x36', '0x49', '0xb']], ['CRC32', 2, ['0x83d3e024']], ['FIBNACCI', 5, ['0x35c7e2L', '0x1ca6eb3ee4626510L', '0x684cfe43b518ef62L', '0x2ac6f30501d6999bL', '0x3cbd2238198c09a0L']], ['BASE64', 2, ['0x33', '0x35', '0x37', '0x3d']], ['BASE64', 6, ['0x6e', '0x54', '0x64', '0x46', '0x67', '0x39', '0x2b', '0x43']], ['XORSTRING', 2, ['0x2a', '0xc']], ['XORSTRING', 5, ['0x62', '0xb', '0x1b', '0x16', '0x48']], ['BASE64', 5, ['0x77', '0x42', '0x2b', '0x36', '0x67', '0x54', '0x72', '0x3d']], ['XORSTRING', 2, ['0x30', '0x10']], ['FIBNACCI', 4, ['0xde2ab8cecafb7902L', '0x2ac6f30501d6999bL', '0x12062f76909038c5L', '0x47b51d498bf6d4f1L']], ['BASE64', 4, ['0x33', '0x35', '0x4a', '0x48', '0x69', '0x45', '0x3d', '0x3d']], ['BASE64', 6, ['0x67', '0x4c', '0x7a', '0x63', '0x67', '0x35', '0x62', '0x62']], ['FIBNACCI', 6, ['0x5fb88f7a983179c2L', '0x35c7e2L', '0x5fb88f7a983179c2L', '0xf5b15148238f5b22L', '0x45e1a61e5624f888L', '0x35c7e2L']], ['FIBNACCI', 4, ['0xde2ab8cecafb7902L', '0x937accfb606984f7L', '0x50eed5966fb5ab95L', '0x12062f76909038c5L']], ['FIBNACCI', 5, ['0x35c7e2L', '0x937accfb606984f7L', '0xec9b9e4287bce2dL', '0x35c7e2L', '0x5fb88f7a983179c2L']], ['BASE64', 4, ['0x6a', '0x35', '0x72', '0x63', '0x67', '0x45', '0x3d', '0x3d']], ['FIBNACCI', 4, ['0x2ac6f30501d6999bL', '0x50eed5966fb5ab95L', '0x12062f76909038c5L', '0x5704e7L']]]


if __name__ == "__main__":
    main()
    # out = []
    # start = 0
    # for item in output:
    #     l = item[1]
    #     out.append(s[start:start+l])
    #     start += l
    # print out