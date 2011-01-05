<?php

/**
  * Functions __construct, request, curlexec, process_response
  */
class TakaraCurl{ 
	
	/**
    * Initialize variables
    */
	protected $post =  NULL;
	protected $url = NULL;
	protected $report = NULL;
	protected $cookie = array();
	protected $result = NULL;
	protected $headers = NULL;
	protected $cookies = array();
	
	/**
    * Excecute curl and parse needed variables
    */
	public function curlexec()
	{
		$c = curl_init($this->url);
		if(isset($this->post)){
			curl_setopt($c, CURLOPT_POST, 1);
			curl_setopt($c, CURLOPT_POSTFIELDS, $this->post);
			unset($this->post);
		}
		
		if(count($_COOKIE)>0){
			$send_cookies = "";
			foreach($_COOKIE as $key=>$val){
				$send_cookies .= $key.'='.$val.'; ';
			}
			curl_setopt($c, CURLOPT_VERBOSE, 1);
			curl_setopt($c, CURLOPT_COOKIE, $send_cookies);
		}
		
		if($_SERVER['SERVER_PORT']=='443'||$_SERVER['HTTPS'])
			$referrer = 'https://';
		else
			$referrer = 'http://';
		$referrer .= $_SERVER['HTTP_HOST'].$_SERVER['REQUEST_URI'].$_SERVER['QUERY_STRING'];	
		curl_setopt($c, CURLOPT_REFERER, $referrer);
		curl_setopt($c, CURLOPT_RETURNTRANSFER, 1);
		curl_setopt ($c, CURLOPT_SSL_VERIFYPEER, 0);
		curl_setopt ($c, CURLOPT_SSL_VERIFYHOST, 0);
		curl_setopt($c, CURLOPT_HEADER, 1);
		curl_setopt($c, CURLINFO_HEADER_OUT, 1);	
		
		/**
      * do the excecute and go to processing - break this up to get rid of the long method
      */
		$this->result = curl_exec($c);
		$this->process_result($c);
	}
	
	private function process_result($c){
		$this->report.= "<br />\\\\\\\\\\\\\\\\SENT HEADERS\\\\\\\\\\\\\\\\<br />";
		$this->report.=nl2br(curl_getinfo($c,CURLINFO_HTTP_CODE));
		$this->report.="<br />\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\<br />";
		
		curl_close($c);

		$lines = explode("\n", $this->result);
			foreach($lines as $line){
				if(strlen($line)<2){
				  if(strstr($response_header, 'Continue')){
					  unset($response_header);
				  }
				  else{
				    $stop=TRUE;
				  }
				}	
				if(!$stop){
				  if(!isset($response_header)) {
				    $response_header = $line;
				  }
					$headers.=$line."\n";
					if(strstr($line, "Set-Cookie")){
						$cookie_elems=str_replace("Set-Cookie: ", "", $line);
						$cookie_elems=explode("=", $cookie_elems);
						$cookie_val=explode(";", $cookie_elems[1]);
						$this->cookie[$cookie_elems[0]]=$cookie_val[0];
					}	
				}

			}
			
			$this->response_header = $response_header;
			$this->report.= "<br /><br />HEADER RTN: ". nl2br($headers);
			$this->headers=$headers;
			$this->result=str_replace($this->headers, "", $this->result);
			if(strstr($this->result, 'Exception caught')){
			 echo $this->result; 
			}
			$this->report.= "<hr />";
	}
	
	public function __set($key, $val){
		$this->$key=$val;
	}
	
	public function __get($key){
		return $this->$key;
	}

}