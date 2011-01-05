<?php

function restify_destroy($controller, $id) {
  global $json_data;
  
  $_GET['id']=$id;
  get_json_data($controller, 'destroy', $_GET);
  header('Location: /'.(isset($_GET['destination'])? $_GET['destination']:'thanks'));
  exit;
  
}