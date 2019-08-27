#coding:utf-8
from pwn import *

class Pos:
    def __init__(self,x,y,z,energy):
        self.x=x
        self.y=y
        self.z=z
        self.energy=energy

def getend(mmap): #获取要到达的目的
    for i in range(5):
        for j in range(8):
            if mmap[i][j]==' ':
                return i,j

movetable={
    'u':(5,5,5,5),
    'd':(3,3,3,3),
    'l':(3,5,3,5),
    'r':(5,3,5,3),
    'l_u':(4,5,4,5),
    'l_d':(3,4,3,4),
    'r_u':(5,4,5,4),
    'r_d':(4,3,4,3),
    'ahead':(3,3,5,5),
    'land':'land'
}

def getname(oz,ox,z,x):
    if oz==z and ox>x:return 'r'
    if oz==z and ox<x:return 'l'
    if oz>z and ox>x:return 'r_u'
    if oz<z and ox>x:return 'r_d'
    if oz>z and ox<x:return 'l_u'
    if oz<z and ox<x:return 'l_d'
    if oz>z and ox==x:return 'u'
    if oz<z and ox==x:return 'd'

def land(p):
    p.sendline('land')
    key=p.recvuntil('\n').strip()
    if key[:4]=='flag':
        print key
        return 1

    cipher=int(p.recvuntil('\n'))
    # print 'enc',cipher
    command='./SPN '+str(key)+' '+str(cipher)
    # print command
    cmd = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    for info in cmd.communicate():
        if len(info):
            info=info[4:]
            # print "dec:",info
            p.sendline(info)

def move(p,r):
    p.recvuntil('>')
    if r=='land':
        if land(p)==1: return 1
        return

    p.sendline('set r1 %d'%r[0])
    p.recvuntil('>')
    p.sendline('set r2 %d'%r[1])
    p.recvuntil('>')
    p.sendline('set r3 %d'%r[2])
    p.recvuntil('>')
    p.sendline('set r4 %d'%r[3])
    p.recvuntil('>')
    # p.sendline('log')
    # print p.recvuntil('>')
    p.sendline('run 1')

def bfs(edge,pos,end):
    if end==(pos.z,pos.x):
        return pos,[]
    queq=[(pos,[])]
    while len(queq):
        sp,route=queq.pop(0)
        for e_point in edge[(sp.z,sp.x)]:
            troute=[i for i in route]
            troute.append(getname(e_point[0],e_point[1],sp.z,sp.x))
            ep=Pos(e_point[1],0,e_point[0],sp.energy-1)
            if e_point==end:
                return ep,troute
            queq.append((ep,troute))
    print 'bfs not found!'
    print edge
    print pos,end
    return 0

def bfs_E(edge,pos,end):

    if end==(pos.z,pos.x):
        return pos,['land'],['u']
    queq=[(pos,[],[])]
    while len(queq):
        sp,route1,route2=queq.pop(0)
        for e_point in edge[(sp.z,sp.x)]:
            if e_point[0]==0:
                continue
            troute1=[i for i in route1]
            troute2=[i for i in route2]
            troute1.append(getname(e_point[0],e_point[1],sp.z,sp.x))
            troute2.insert(0,getname(sp.z,sp.x,e_point[0],e_point[1]))
            ep=Pos(e_point[1],0,e_point[0],20)
            if e_point==end:
                troute1.append('land')
                troute2.insert(0,'u')
                return ep,troute1,troute2
            queq.append((ep,troute1,troute2))
    print 'bfs not found!'
    print edge
    print pos,end
    return 0

def passmd5(p):
    import hashlib
    md5=p.recvuntil('\n').strip()
    print 'md5:',md5
    cmd = subprocess.Popen("./gettime "+md5, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    for info in cmd.communicate():
        if len(info):
            # info=info[]
            print info
            p.sendline(info)

def fireD(p,mode):
    k=1
    t=1
    while k:
        mmap1=[]
        p.recvuntil('>')
        p.sendline('radr')
        readmap(p,mmap1)
        mmap2=[]
        readmap(p,mmap2)
        for line in mmap2:
            for c in line:
                if c=='D':
                    t=0
                    if mode: #探测模式
                        return 1
                    p.recvuntil('>')
                    p.sendline('fire')
                    passmd5(p)
                    k=0
        k=not k
    return t,mmap2

def getentry(p):
    mmap1=[]
    p.recvuntil('>')
    p.sendline('radr')
    readmap(p,mmap1)
    mmap2=[]
    readmap(p,mmap2)
    for i in range(5):
        for j in range(8):
            if (mmap2[i][j]==' ' or mmap2[i][j]=='D' ) and mmap1[i][j]==' ':
                return i,j

def fire(p,pos):
    #此时已经在入口
    while 1:
        t,mmap2=fireD(p,0) #歼灭模式
        move(p,movetable['ahead'])
        pos.y+=1
        edge=getedge(mmap2)
        if t:break #未发现敌人
        end=getentry(p)
        
        pos,route=bfs(edge,pos,end)
        for key in route:
            move(p,movetable[key])
    if 'F' in mmap2[0]:
        return charge(p,pos,edge,(1,mmap2[0].index('F')))
  
def charge(p,pos,edge,end):#充电
    t,route1,route2=bfs_E(edge,pos,end)
    for key in route1:
        if move(p,movetable[key])==1:return 1
    for key in route2:
        move(p,movetable[key])
    move(p,movetable['ahead'])
    pos.y+=1
    if fireD(p,1)==1: return fire(p,pos)
    move(p,movetable['ahead'])
    # p.recvuntil(">")
    # p.sendline("log")
    # p.recvuntil('\n')
    pos.y+=1
    return pos
   
def getedge(mmap1):
    edge={}
    for i in range(5):
        for j in range(8):
            if  mmap1[i][j]==' ' or mmap1[i][j]=='E' or mmap1[i][j]=='B':
                edge[(i,j)]= []
                for ki in range(i-1,i+2):
                    if ki<0 or ki>4:
                        continue
                    for kj in range(j-1,j+2):
                        if kj<0 or kj>7 or (ki==i and kj==j):
                            continue
                        if (mmap1[ki][kj]==' ' or mmap1[ki][kj]=='E' or mmap1[i][j]=='B') and (ki,kj) not in edge[(i,j)]:
                            edge[(i,j)].append((ki,kj))
    return edge

def readmap(p,mmap):
    c=p.recv(1)
    if c[0]=='_':
        return
    c+=p.recv(8)
    mmap.append(c[:-1])
    for i in range(4):
        mmap.insert(0,p.recvuntil('\n')[:-1])
    p.recvuntil('--------\n')
    # print '----------------'
    # for line in mmap:
    #     print line
    # print '----------------'
    for i in range(5):mmap[i]=mmap[i].replace('U',' ')

def getmap(p,pos):
    global movetable

    mmap1=[]
    mmap2=[]
    mmap3=[]
    mmap4=[]
    
    p.recvuntil('>')
    p.sendline('radr')
    readmap(p,mmap1)
    readmap(p,mmap2)

    edge=getedge(mmap1)
    if 'E' in mmap1[0]:
        return charge(p,pos,edge,(1,mmap1[0].index('E')))
    if 'B' in mmap1[0]:
        return charge(p,pos,edge,(1,mmap1[0].index('B')))
    if 'F' in mmap1[0]:
        return charge(p,pos,edge,(1,mmap1[0].index('F')))

    end=getend(mmap2)
    # print edge
    start=(pos.z,pos.x)
    pos,route=bfs(edge,pos,end)
    # print 'route:',route
    for key in route:
        move(p,movetable[key])

    move(p,movetable['ahead'])
    pos.y+=1
    
    move(p,movetable['ahead'])
    # p.recvuntil('>')
    # p.sendline('log')
    # p.recvuntil('\n')
    pos.y+=1
    pos.energy-=2
    return pos

def main():
    #p=remote('127.0.0.1',1235)
    p=remote('e552a168f687.gamectf.com',9999)
    # p.sendline('map%d.txt'%(i))
    pos=Pos(1,0,0,20) 
    for i in range(200):
        pos=getmap(p,pos)
        if pos==1:return
main()
