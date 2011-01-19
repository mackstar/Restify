<?php

// to do tie this together with create - almost same??

function restify_rest_update($controller, $id=1) {
  GLOBAL $json_data;
  restify_check_delivery();
  
  if ($action == 'login') {
    drupal_goto ('create/login/new', array('destination'=>'edit/'.$controller.'/'.$id));
    return;
  }
  elseif ($_SERVER['REQUEST_METHOD']=='GET') {
    get_json_data($controller, 'edit', $action == 'shinki'? array('shinki'=>1, 'id'=>$id): array('id'=>$id));
  }
  else {
    if (!isset($_POST['confirm'])) {
      get_json_data($controller, 'update', array_merge($_POST, array('confirm'=>1, 'id'=>$id)));
    }
    else {
      $post_vars = array_merge($_POST, array('id'=>$id));
      unset($post_vars['confirm']);
      get_json_data($controller, 'update', $post_vars);
      
      if(!isset($_POST['form_id'])&&count(drupal_get_messages('error', FALSE))<1) {
        drupal_goto(isset($_GET['destination'])? $_GET['destination']:'thanks');
      }
    }  
    $form = drupal_get_form('restify_create_form');
    if(!is_form_errors()){
      $form = drupal_get_form('restify_create_confirm_form');
    }
  }
  return isset($form)? $form : drupal_get_form('restify_create_form');
}