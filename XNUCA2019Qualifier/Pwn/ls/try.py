#!/usr/bin/python
from random import randint 
import ctypes
from z3 import *
s = ctypes.c_int64(randint(0,0xffffffffffffffff)).value
#s = ctypes.c_int64(5952515969170658357).value
#s = ctypes.c_int64(-7123024779656821287).value
print 's is', s,hex(s)
x=1234

# s+x < s-3*x

def geto(x):
    global cnt
    cnt +=1
    x=ctypes.c_int64(x).value
    oracle = 1 if ctypes.c_int64(s+x).value < ctypes.c_int64(s - ctypes.c_int64(3*x).value).value  else 0
    #print oracle, s,x,ctypes.c_int64(3*x).value, ctypes.c_int64(s+x).value, ctypes.c_int64(s - ctypes.c_int64(3*x).value).value
    return oracle


def random_test():
    while True:
        x=int(raw_input().strip())
        print geto(x)

def mimic_left(x):
    #z = s+x  
    x = ctypes.c_int64(x).value
    if x<0x7fffffffffffffff-s:
        z = s + x
    else:
        z = s + x - (1<<64)
    if x<-0x8000000000000000-s:
        z = s + x + (1<<64)
    try:
        assert z == ctypes.c_int64(s + x).value
    except:
        print x,z, ctypes.c_int64(s + x).value
        raise
    return z

def mimic_right(x):
    # h = 3*x
    # z = s-3*x
    x = ctypes.c_int64(x).value
    if x >  0:
        if x > 0x7fffffffffffffff/3:
            h = 3*x - (1<<64)
        else:
            h = 3*x

    elif x < 0:
        if x < - 0x8000000000000000/3:
            h = 3*x + (1<<64)
        else:
            h = 3*x
    else:
        h=0
    
    if s-h < - 0x8000000000000000:
        z = s-h + (1<<64)
    elif s-h > 0x7fffffffffffffff:
        z = s-h - (1<<64) 
    else:
        z= s-h

    assert z == ctypes.c_int64(s - ctypes.c_int64(3*x).value).value
    return z

#def get_zero_point():
     

def test_mimic_left():
    for i in range(0xffffff):
        mimic_left(i<<40)

def test_mimic_right():
    old_c = True
    for i in range(0xffffff):
        l=mimic_left(i<<40)
        r=mimic_right(i<<40)
        new_c = l<r
        if new_c != old_c:
            print hex(i<<40), new_c
            old_c=new_c

cnt=0
def binary_search(l,r,left=None):
    global cnt
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
    for x in [res1,res2,res3,res4]:
        print geto(x-1),geto(x),geto(x+1)
    sol = z3.Solver()
    ans = BitVec('ans',64)
    sol.add(ans+res1<=ans-3*res1)
    sol.add(ans+res2<=ans-3*res2)
    sol.add(ans+res3<=ans-3*res3)
    sol.add(ans+res4<=ans-3*res4)
    print sol.check()
    m = sol.model()
    r=m[ans].as_signed_long()
    print r
    #import IPython; IPython.embed()
    assert r==s


def test_mimic():
    test_mimic_right()

#test_mimic_left()
def random_test_mimic():
    for i in range(100):
        #s = random.randint(
        s = ctypes.c_int64(randint(0,0xffffffffffffffff)).value
        print 's is',hex(s),s
        test_mimic_right()
        print '-'*60
#test_mimic()
solve()
