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
                <a href="<?php echo url(array('c'=>"main", 'a'=>"LoginOut", ));?>" class="navbar-link">Loginout</a>
              </p>
              <ul class="nav">
                    <li ><a href="<?php echo url(array('c'=>"main", 'a'=>"index", ));?>">Home</a></li>
                    <li class="active"><a href="<?php echo url(array('c'=>"main", 'a'=>"post", ));?>">post_bug</a></li>
                    <li><a href="<?php echo url(array('c'=>"main", 'a'=>"note", ));?>">note</a></li>
              </ul>
            </div><!--/.nav-collapse -->
          </div>
        </div>
</div>

<div class="upload">
          <form action="<?php echo url(array('c'=>"main", 'a'=>"bug", ));?>"  method="post">
              <div class="form-group">
                  <label for="url">提交本网站有bug的url地址,管理员会及时查看并会给oauth2.0服务授权。</label>
                  <input id="url" type="text" class="form-control" name="url" placeholder="">
                <button type="submit" class="btn btn-default">submit</button>
              </div>
          </form>
            <p> 已经提交的bug:</p>
            <ul>
              <?php if(!empty($bugs)){ $_foreach_bug_counter = 0; $_foreach_bug_total = count($bugs);?><?php foreach( $bugs as $bug ) : ?><?php $_foreach_bug_index = $_foreach_bug_counter;$_foreach_bug_iteration = $_foreach_bug_counter + 1;$_foreach_bug_first = ($_foreach_bug_counter == 0);$_foreach_bug_last = ($_foreach_bug_counter == $_foreach_bug_total - 1);$_foreach_bug_counter++;?>
              <li><a href="<?php echo htmlspecialchars($bug['bugurl'], ENT_QUOTES, "UTF-8"); ?>" target="_blank"> <?php echo htmlspecialchars($bug['bugurl'], ENT_QUOTES, "UTF-8"); ?> </a> </li>
              <?php endforeach; }?>
            </ul>
</div>
