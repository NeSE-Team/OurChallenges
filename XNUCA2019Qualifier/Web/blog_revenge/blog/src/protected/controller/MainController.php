<?php
class MainController extends BaseController {
	// 首页

	function actionOauth(){
		// dump($_SERVER);
		$code = arg('code');
		if($code){
			$res = file_get_contents("http://".$GLOBALS['oauthserver']."/access_token?client_id=3f66bf84f42fec8fd6348593ab74db04&client_secret=51abb9636078defbf888d8457a7c76f85c8f114c&code=".$code."&redirect_uri=http://".$GLOBALS['host']."/main/oauth");
			
			if(!$res){
				$this->error('授权过程发生错误，授权服务器无法访问!');
			}
			$data = json_decode($res,true);
			if(!$data['access_token']){
				$this->error('获取access_token发生错误!');
			}
			$access_token = $data['access_token'];

			$user_info = file_get_contents('http://'.$GLOBALS['oauthserver'].'/user?access_token='.$access_token);
			if(!$user_info){
				$this->error('授权过程发生错误，授权服务器无法访问!');
			}
			$data = json_decode($user_info,true);
			if($data['message'] == 'Bad Token'){
				$this->error('授权过程发生错误，access_token错误');
			}
			if($data['message'] == 'Token expired'){
				$this->error('授权过程发生错误，access_token已经超时,请重新获取!');
			}
			if($data['email']){
				$email = $data['email'];
				$uuid = $data['uuid'];
				$user = New User();

				if(isset($_SESSION['userid']) && isset($_SESSION['username'])){ 
					//如果登录了，就进入账号绑定过程
					$condition = array('id'=>$_SESSION['userid'],'bind'=>false); //查找没有绑定的账号，避免重绑定漏洞
					$res = $user->update($condition,array('email'=>$email,'uuid'=>$uuid,'bind'=>true));
					// if(!$res){ $this->error('账号绑定过程发生未知错误!'); }
					$this->jump('/main/index');
					
				}else{
					//如果没有登录,就判断是否是新账号
					$condition = array('email'=>$email,'uuid'=>$uuid);
					$res = $user->find($condition);
					if($res){

						// 如果是老账号
						$_SESSION['userid'] = $res['id'];
						$_SESSION['username'] = $res['username'];
						$this->jump("/main/index");
					}else{
						// 如果是新账号
						$_SESSION['email'] = $data['email'];
						$_SESSION['uuid'] = $data['uuid'];
						$_SESSION['set'] = true;
						$this->jump('/main/set');
					}
				}
			}else{
				$this->error('授权失败，发生未知错误!');
			}
			
		}	
		
	}

	function actionSet(){
		if($_SESSION['set']&&$_SESSION['email']&&$_SESSION['uuid']){
			if($_POST){
				$username = arg('username');
				$password = md5(arg('password'));

				if(empty($username)||empty($password)){
					// echo "<script>alert('Username or password is error.')</script>";
					$this->error('Username or password is empty.');
				}else{
						$user = New User();
						$condition = array('username'=>$username);
						$res = $user->find($condition);
						if(!empty($res)){
							$this->error('Username is registered!');
							// echo "<script>alert('Username is registered!.')</script>";
						}						
						$res = $user->create(["username"=>$username,
							"password"=>$password,
							"email"=>$_SESSION['email'],
							"uuid" => $_SESSION['uuid'],
							"bind" => true,
						]);
						if(!$res){ 
							$this->error('对不起，发生错误，添加信息失败！');
						}else{
							$_SESSION['userid'] = $res;
							$_SESSION['username'] = $username;
							$this->jump("/main/index");
						}
					}	
				}
			}else{
				$this->jump("/main/login");
		}
	}
	
	function actionIndex(){
		   // $this->display('error.html');
		   $this->checkLogin("/main/index");
		   $user = new User();
		   $condition = array( "id" =>$_SESSION['userid']);
		   $res = $user->find($condition);
		   $this->username = $res['username'];
		   $this->email = $res['email'];
		   $this->uuid = $res['uuid'];
		   if($_SESSION['userid']=== '1'){
				setcookie("flag",$GLOBALS['flag'],time()+30*60,"/","",false,True);
		   }
		   $this->oauthserver = $GLOBALS['oauthserver'];
		   $this->host = $GLOBALS['host'];
		   if(!$res['bind']){
				$this->show = true;
			}
		
	}
	private function checkLogin($next=null){
		if(!isset($_SESSION['userid']) || !isset($_SESSION['username'])){
			if(!$next){
                $this->jump("/main/login"); 
			}else{
				$this->jump("/main/login?next=$next");	
			}
		}
	}
	function actionLoginOut(){
		session_destroy();
		$this->jump("/main/login");
	}

    function actionLogin(){
		
		// header("Content-Security-Policy: script-src 'nonce-".$nonce."'");
		$next = arg("next");
		if(isset($_SESSION['userid']) && isset($_SESSION['username'])){
			if($next){
				$this->jump($next);
			}else{
				$this->jump("/main/index");
			}
		}
		if($_POST){
			$username = arg('username');
			$password = arg('password');
            if (empty($username) || empty($password)) {
				// echo "<script>alert('Username or password is empty.')</script>";
				$this->error('Username or password is empty.');
			}else{
				$user = New User();
				$password = md5($password);
				$condition = array("username"=>$username,"password"=>$password);
				$result = $user->find($condition);
				if(empty($result) || $result['password']!==$password){
					// echo "<script>alert('Username or password is error.')</script>";
					$this->error('Username or password is error.');
				}else{
					$_SESSION['userid'] = $result['id'];
					$_SESSION['username'] = $result['username'];
					$this->jump("/main/index");
				}
			}
		}
		$this->oauthserver = $GLOBALS['oauthserver'];
		// $this->nonce = $this->nonce;
		$this->host = $GLOBALS['host'];
	}

	function actionRegister(){
		if($_POST){
			$username = arg('username');
			$password = arg('password');
			if(empty($username)||empty($password)){
				// echo "<script>alert('Username or password is error.')</script>";
		        $this->error('Username or password is error.');
			}else{
			$password = md5($password);
			$user = New User();
			$condition = array('username'=>$username);
			$res = $user->find($condition);
			if(!empty($res)){
				$this->error('Username is registered!');
				// echo "<script>alert('Username is registered!.')</script>";
			}
			$res = $user->create([
			"username"=>$username,
			"password"=>$password]);
			if(!$res) $this->error('something error. register fiaied!');
			else $this->jump("/main/login");
			}
		}
	}


	function actionNote(){
		$this->checkLogin("/main/note");
		$note = New Note();
		$condition = array('userid'=>$_SESSION['userid']);
		$res = $note->findAll($condition);
		$this->notes = $res;
	}

	function actionAddNote(){
		$this->checkLogin();
		// var_dump($_POST);
		if($_POST){
			$title = arg('title');
			$content = arg('content');
			$note = New Note();
			$res = $note->create([
				"userid"=>$_SESSION['userid'],
				"title"=>$title,
				"content"=>$content]);
			if(!$res) $this->error('something error. add note fiaied!');
			else $this->jump("/main/note");
		}
	}

	function actionShowNote(){
		$this->checkLogin("/main/showNote");
		$this->display("main_showNote.html");
	}
	function actionJsonp(){
		$data = array();
		$id = arg('id');
		$callback = arg('callback')?arg('callback'):"shownote";
		header("Content-Type: application/javascript");
		//判断是否登录
		if(!isset($_SESSION['userid']) || !isset($_SESSION['username'])){
			$data['title'] = "身份认证错误";
			$data["content"] = "请重新登录。" ;
			$callback = "error";
			echo $callback."(".json_encode($data).")";
			return ;
		}

		//判断callback方法是否正确
		if(preg_match('/\[|\]|\\\\|\(|\)|\s|\"|\'|\*|%|<|>|~|\||&|\^|\!|\?|:|;|=|\*|\+/',$callback)){
			 $data['title'] = "回调方法错误";
			 $data['content'] ="此方法不被允许.";
			 $callback = "error";
			 echo $callback."(".json_encode($data).")";
			 return ;
		}
	
		// 判断 id 是否合法
		if(preg_match('/\[|\]|\\\\|\(|\)|\s|\"|\'|\*|%|~|\|\^|\?|:|\*|\+/',$id)){
			$data['title'] = "id参数错误";
			$data['content'] ="id存在非法输入.";
			$callback = "error";
			echo $callback."(".json_encode($data).")";
			return ;
	   }
	   
		$note = New Note();
		if($_SESSION['userid']=== '1'){
			$condition = array('id'=>$id);
		}else{
			$condition = array('id'=>$id,'userid'=>$_SESSION['userid']);
		}
		$res = $note->find($condition);
		if(!$res){
			$data['title'] = "无法查看此note";
			$data['content'] ="id为".$id."的note没找到，或者你无权查看。";
			echo $callback."(".json_encode($data).")";
			return ;
		}else{
			$data['title'] = htmlspecialchars($res['title'],ENT_QUOTES);
			$data['content'] = htmlspecialchars($res['content'],ENT_QUOTES);
			echo $callback."(".json_encode($data).")";
			return ;
		}
	}
    function actionPost(){
		$this->checkLogin("/main/post");
		$bug = New Bug();
		if($_SESSION['userid']!=='1'){
			$condition = array('userid'=>$_SESSION['userid']);
			$res = $bug->findAll($condition);
			$this->bugs = $res;
		}else{
			$condition = array('read'=>false,);
			$res = $bug->findAll($condition);
			$bug->update($condition,array("read"=>true)); # 更新数据库
			$this->bugs = $res;
		}
		$this->display('main_post.html');
	}

	function actionBug(){
		$this->checkLogin();

		if($_POST){	
			$url = arg('url');
			$preurl = "http://".$GLOBALS['host']."/";
			if(stripos($url,$preurl)!==0){
				$this->error('your url is error, you must start your url with '.$preurl);
		}
			$bug = New Bug();
			$res = $bug->create([
				"userid"=>$_SESSION['userid'],
				"bugurl"=>$url]);
			if(!$res) $this->error('something error. add bug url fiaied!');
			else $this->jump("/main/post");
		}
	}

}