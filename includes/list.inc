<?php
function restify_list($controller, $id=NULL) {
  global $json_data, $theme_key;
  
  if($id) {
    get_json_data($controller, 'index', array_merge($_GET, array('code'=>$id)));
  }
  else {
    get_json_data($controller, 'index', $_GET);
  }

	return theme('list_'.$controller);
}