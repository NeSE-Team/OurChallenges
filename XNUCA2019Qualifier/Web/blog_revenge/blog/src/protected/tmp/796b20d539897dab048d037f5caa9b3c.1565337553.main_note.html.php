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
                <li><a href="<?php echo url(array('c'=>"main", 'a'=>"post", ));?>">post_bug</a></li>
                <li class="active"><a href="<?php echo url(array('c'=>"main", 'a'=>"note", ));?>">note</a></li>
          </ul>
        </div><!--/.nav-collapse -->
      </div>
    </div>
</div>

<div class="upload">
      <form action="<?php echo url(array('c'=>"main", 'a'=>"addNote", ));?>"  method="post">
        <div class="form-group">
            <label for="title">note标题</label>
            <input name="title" type="text" class="form-control" id="title" placeholder="标题">
        </div>
        <div class="form-group">
            <label for="content">note内容</label>
            <input name="content" type="text" class="form-control" id="content" placeholder="内容">
        </div>
        <button type="submit" class="btn btn-default">submit</button>
      </form>

        <p> 你已经写得note如下:</p>
        <ul>
          <?php if(!empty($notes)){ $_foreach_note_counter = 0; $_foreach_note_total = count($notes);?><?php foreach( $notes as $note ) : ?><?php $_foreach_note_index = $_foreach_note_counter;$_foreach_note_iteration = $_foreach_note_counter + 1;$_foreach_note_first = ($_foreach_note_counter == 0);$_foreach_note_last = ($_foreach_note_counter == $_foreach_note_total - 1);$_foreach_note_counter++;?>
          <li><a href="/main/showNote?id=<?php echo htmlspecialchars($note['id'], ENT_QUOTES, "UTF-8"); ?>" target="_blank"> <?php echo htmlspecialchars($note['title'], ENT_QUOTES, "UTF-8"); ?> </a> </li>
          <?php endforeach; }?>
        </ul>

</div>