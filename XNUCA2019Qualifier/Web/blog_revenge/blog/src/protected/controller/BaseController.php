<?php
class BaseController extends Controller{
	public $layout = "layout.html";
	protected $nonce;  
	private function randomStr($len=16){
		$baseStr = "abcdefghijklmnopqrstuvwxyz0123456789";
		$randStr = "";
		for($i=0;$i<$len;$i++){
			$randStr.= $baseStr[mt_rand(0,strlen($baseStr)-1)];
		}
		return $randStr;
	}

	function init(){
		$this->nonce = base64_encode($this->randomStr(32));
		ini_set("session.cookie_httponly",true);		
		// ini_set('session.save_handler', 'user');  
		// $handler = new MySessionHandler();  
		// session_set_save_handler($handler, true);
		session_start();
		header("Content-type: text/html; charset=utf-8");
		header("Content-Security-Policy: default-src 'self'; base-uri 'self';frame-ancestors 'none'; connect-src 'self'; img-src 'self'; style-src 'self';font-src 'self'; form-action 'self'; script-src 'self'  'nonce-".$this->nonce."';");
	    header("Referrer-Policy: no-referrer");
	}
	
	function error($message) {
        $this->message = $message;
        $this->display('error.html');
        exit;
	}
	
    function tips($msg, $url){
    	$url = "location.href=\"{$url}\";";
		echo "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><script>function sptips(){alert(\"{$msg}\");{$url}}</script></head><body onload=\"sptips()\"></body></html>";
		exit;
    }
    function jump($url, $delay = 0){
		$url = htmlspecialchars($url,ENT_QUOTES);
        echo "<html><head><meta http-equiv='refresh' content='{$delay};url={$url}'></head><body></body></html>";
        exit;
    }
	
	public static function err404($module, $controller, $action, $msg){
		header("HTTP/1.0 404 Not Found");
		exit;
	}
} 