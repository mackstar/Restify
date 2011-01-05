<?php

/**
  * needs curl class to work
  */
require_once(dirname(__FILE__).'/class_curl.php');

/**
  * Functions __construct, request, curlexec, process_response
  */

class Rest extends TakaraCurl{
	
	/**
	 * Initialize variables
	 */
	private $domain = NULL;
	private $website_id = NULL;
	private $config = array();
	
	/**
	 * Initialize class website_id and configuration needed
	 */
	function __construct($website_id, $config) {
	
		$this->config = $config;
		$this->website_id = $this->config['website_id'];
		$this->url = $this->config['protocol'].'://'.$this->config['domain'];
		if($this->config['port']!=NULL){
			$this->url.=':'.$this->config['port'];
		}
		$this->url.='/';
	}
	
	/**
	 * Request session allows GET, POST, PUT and DESTROY actions
	 */
	private function request($url, $params=NULL) {
		
		$this->url = $url;
		/**
  	 * Set parameters for get string or post string
  	 */
  	$params['website_id']=$this->website_id;
		$this->url .= "?";
		if(is_array($params)&&count($params)>0){
  		foreach($params as $key=>$val){
  		  // Want to turn params with "-" in to arrays
  		  if(strstr($key, '-')) {
  		    $elements = explode('-', $key);
  		    for($i=0; $i<count($elements);$i++) {
  		      if($i>0) {
  		        $elements[$i]='['.$elements[$i].']';
  		      }
  		    }
  		    $key = implode('', $elements);
  		  }
  		  if(is_array($val)) {
  		    $val = implode("-", $val);
  		  }
  		  
  			if($params["method"]=='post'||$params["_method"]=='delete'||$params["_method"]=='put'){
  				$this->post .= "{$key}=".urlencode($val)."&";
  			}
  			else{
  				$this->url.="{$key}=".urlencode($val).'&';
  			}
  		}
		}
		$this->curlexec();
	}	
	
	/**
	 * Process response from curl headers and result
	*/	
	private function response(){
		$this->save_cookie();
		return $this->result;
	}
	
	/**
	 * Save cookie that has been collected by curl - Yesそのまま！
	 */
	private function save_cookie(){
		if(count($this->cookie)>0){
			foreach($this->cookie as $key=>$val){
				setcookie($key, $val, 0, '/');
				$_COOKIE[$key]=$val;
			}
		}
	}

	/**
	 * Show!
	 */
	public function show($controller, $params=NULL){
	  $id=$params['id'];
	  unset($params['id']);
		$this->request($this->url.$controller.'/'.$id, $params);
		return $this->response();
	}
	
	public function index($controller, $params=NULL){
		$this->request($this->url.$controller, $params);
		return $this->response();
	}
	
	public function destroy($controller, $params=NULL){
		$params['_method']='delete';
		$this->request($this->url.$controller.'/'.$params['id'], $params);
		return $this->response();
	}
	
	public function edit($controller, $params=NULL){
	  $id = $params['id'];
	  unset($params['id']);
		$this->request($this->url.$controller.'/'.$id.'/edit', $params);
		return $this->response();
	}
	
	public function update($controller, $params){
		$params['_method']='put';
		$id = $params['id'];
		unset($params['id']);
		$this->request($this->url.$controller.'/'.$id, $params);
		return $this->response();
	}
	
	public function add($controller, $params=NULL){
		$this->request($this->url.$controller.'/new', $params);
		return $this->response();
	}
	
	public function create($controller, $params){
		$params['method']='post';
		$this->request($this->url.$controller, $params);
		return $this->response();
	}

}