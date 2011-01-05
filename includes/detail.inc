<?php

function restify_detail($controller, $id) {
  global $json_data;
  $_GET['id']=$id;
  get_json_data($controller, 'show', $_GET);
  return theme('detail_'.$controller);
}
