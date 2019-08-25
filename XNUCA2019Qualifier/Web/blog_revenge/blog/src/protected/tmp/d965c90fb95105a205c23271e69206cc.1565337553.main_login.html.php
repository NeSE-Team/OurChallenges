<?php if(!class_exists("View", false)) exit("no direct access allowed");?>
<div class="form-content">
    <form class="form-signin" action="#" method="post">
            <h2 class="form-signin-heading">Please login</h2>
            <input  name="username" type="text" class="input-block-level"  placeholder="username">
            <input name="password" type="password" class="input-block-level" placeholder="Password">
            <label class="checkbox">
                <input type="checkbox" value="remember-me"> 记住我
            </label>
            <button class="btn btn-large btn-primary" type="submit">登录</button>
            <button class="btn btn-large btn-primary" type="button" id="auth" > 第三方授权登录 </button>
            <!-- /main/register -->
            <!-- speedphp is so good! -->
    </form>
    <script nonce='<?php echo htmlspecialchars($nonce, ENT_QUOTES, "UTF-8"); ?>'>
       $('#auth').click(function(){
            window.location.href="http://<?php echo htmlspecialchars($oauthserver, ENT_QUOTES, "UTF-8"); ?>/oauth?client_id=3f66bf84f42fec8fd6348593ab74db04&redirect_uri=http%3A%2f%2f<?php echo htmlspecialchars($host, ENT_QUOTES, "UTF-8"); ?>%2fmain%2foauth%2f&scope=user&response_type=code";
        });
    </script>
</div>