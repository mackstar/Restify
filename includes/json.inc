<?php

function restify_load_json() {
  global $json_data;
  if (!isset($json_data)) {
    get_json_data('main_categories', 'index');
  }
}

function get_json_data($controller, $action, $post_array= array(), $confirm=FALSE) {
  global $json_data, $conf;
  //Get json data from rest new
  if ($json_data==NULL) {
    $rest = new Rest("0", $conf);
    $result = $rest->$action($controller, $post_array);
    if ($result=='') {
      if (empty($_SESSION['messages']['error'])) {
        drupal_set_message(t('Couldn\'t connect to the data source'), 'error');
      }
    }
    else {
      $json_data = parse_json_data($result);
    }
  } 
}

/**
 * Parses and Sorts data into a more preferable object setting errors and messages
 * 
 * @param $json_object
 * Stores dummy object
 * @return 
 * The json object in its final format
 */
function parse_json_data($result) {
  $result = json_decode($result);
  // Enumerates object and object vars
  $json_object = new stdClass;
  for($i=0; $i < count($result); $i++) {
    
    // List of items is an array NOT an object so we cant iterate it as an object!
    // Convert array list to object list
    if (!is_array($result[$i]) && !is_object($result[$i])) {
      return false;
    }
    elseif (!is_array($result[$i])) {
      $object_vars = get_object_vars($result[$i]);
      if (count($object_vars)>0) {
        foreach($object_vars as $key=>$val) {
          $json_object->$key = $val;
        }
      }
    }
    // Iterate the array into an array of ojects 
    else {
      foreach($result[$i] as $object) {
        $object_vars = get_object_vars($object);
        if (count($object_vars) > 0) {
          foreach($object_vars as $key=>$val) {
            $json_object->{$key}[] = $val;
          }
        }
      }
    }  
  }

  // Sets Errors
  if (count($json_object->errors) > 0) {
    foreach($json_object->errors as $error) {
      if (is_array($error)&&$error[0] == 'base') {
        drupal_set_message(t($error[1]), 'error');
      }
      elseif (is_array($error)) {
        form_set_error($error[0], $error[1]);
      }
      else {
        drupal_set_message(t($error), 'error');
      }
    }
  }
  
  // Sets Messages
  if (count($json_object->notices)>0) {
    foreach($json_object->notices as $notice) {
      drupal_set_message(t($notice));
    }  
  }
  
  // Set title
  if (isset($json_object->info->title)) {
    drupal_set_title($json_object->info->title);
  }
  
  //set description
  if (isset($json_object->info->description)) {
    drupal_set_html_head('<meta name="description" content="'.$json_object->info->description.'" />');
  }
  
  //set keywords
  if (isset($json_object->info->keywords)) {
    if (is_array($json_object->info->keywords)) {
      drupal_set_html_head('<meta name="keywords" content="'.implode(', ', $json_object->info->keywords).'" />');
    }
    else {
      drupal_set_html_head('<meta name="keywords" content="'.$json_object->info->keywords.'" />');
    }
  }
  
  //set description
  if (isset($json_object->info->breadcrumbs)) {
    restify_breadcrumb($json_object->info->breadcrumbs);
  }
  
  return $json_object;
}

