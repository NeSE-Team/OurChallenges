<?php if(!class_exists("View", false)) exit("no direct access allowed");?><div class="navbar navbar-inverse navbar-fixed-top">
	<div class="navbar-inner">
	  <div class="container-fluid">
		<button type="button" class="btn btn-navbar" data-toggle="collapse" data-target=".nav-collapse">
		  <span class="icon-bar"></span>
		  <span class="icon-bar"></span>
		  <span class="icon-bar"></span>
		</button>
		<a class="brand" href="#">blog</a>
		<div class="nav-collapse collapse">
		  <p class="navbar-text pull-right">
			<?php if ($show) : ?>
				<a href="http://<?php echo htmlspecialchars($oauthserver, ENT_QUOTES, "UTF-8"); ?>/oauth?client_id=3f66bf84f42fec8fd6348593ab74db04&redirect_uri=http%3A%2f%2f<?php echo htmlspecialchars($host, ENT_QUOTES, "UTF-8"); ?>%2fmain%2foauth%2f&scope=user&response_type=code" class="navbar-link">绑定第三方账号</a>
			<?php endif; ?>
			
			<a href="<?php echo url(array('c'=>"main", 'a'=>"LoginOut", ));?>" class="navbar-link">Loginout</a>
		  </p>
		  <ul class="nav">
				<li class="active"><a href="<?php echo url(array('c'=>"main", 'a'=>"index", ));?>">Home</a></li>
				<li><a href="<?php echo url(array('c'=>"main", 'a'=>"post", ));?>">post_bug</a></li>
				<li><a href="<?php echo url(array('c'=>"main", 'a'=>"note", ));?>">note</a></li>
		  </ul>
		</div><!--/.nav-collapse -->
	  </div>
	</div>
</div>
<div class="content">
<div class="jumbotron">
  <h3>welcome: <?php echo htmlspecialchars($username, ENT_QUOTES, "UTF-8"); ?>!</h3>
  <p>如果你绑定了第三方账号，下面会显示妳的邮件地址和uuid,如果没有绑定，你可以选择绑定!</p>
  <p>email: <?php echo htmlspecialchars($email, ENT_QUOTES, "UTF-8"); ?></p>
  <p>uuid: <?php echo htmlspecialchars($uuid, ENT_QUOTES, "UTF-8"); ?></p>
</div>
</div>
