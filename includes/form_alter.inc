<?php
/**
 * Alters the form into a confirmation form
 *
 * Adds a hidden tag hidden => 1
 * Adds a back button history.go(-1)
 *
 * @todo have the button go back to original form with details entered
 *  we need  a way to save state for this.
 *
 * @param $form
 * @param $form_state
 * @return Nowt
 */
function restify_form_restify_create_confirm_form_alter(&$form, &$form_state) {
  restify_convert_to_hidden_fields($form);
  $form['confirm'] = array('#type'=>'hidden', '#value'=>1);
  $form['back'] = array(
    '#type'=>'button', '#button_type' => 'button', '#value' => t('Return'), '#weight' => 1000, 
    '#attributes' => array('onclick' => 'window.location=\''.request_uri().'\'; return false;')
    );
}


/**
 * Alters the standard form that we have created
 *
 * @param $form
 * @param $form_state
 * @return Nowt
 */
function restify_form_restify_create_form_alter(&$form, &$form_state) {
  if(!isset($form['confirm'])) {
    $form['submit']['#value'] = t('Go to Confirmation Screen');
  }

  // If kessai - check for urikake do auto kessai
  if (arg(1) == 'kessai') {
    global $json_data;
    $doKessai = "do_kessai?";
    
    // If payment is not needed to be made then skip kessai
    // This is done for shipping message order
    
    if (
      !$json_data->order->$doKessai || 
      ($json_data->info->member->status == 'kaiin' && 
      $json_data->order->payment == t('urikake'))
    ) {
      header('Location: /thanks/checkout/'.arg(2));
      exit;
    }

        // If not logged in go to login page
    if(carty_not_logged_in() && $json_data->order->payment == t('urikake')){
      drupal_set_message(t('You need to be logged in for credit account payments'), 'error'); 
      header('Location: /create/login?destination='.substr(request_uri(), 1));
      exit;
    }
  }
  
  // If products
  if(substr(request_uri(), 0, 10)=='/products/' && isset($form['details']['bbs-subject'])){
    global $json_data;
    $form['details']['confirm']=array('#type' => 'hidden','#value' => 1);
    $form['details']['bbs-product_code']=array(
      '#type' => 'hidden','#value' => $json_data->product->code
      );
    $form['#action']= "/create/bbs?destination=".substr(request_uri(),1);
    $form['submit']['#value']=t('Submit');
  }
}

/**
 * Recursive function to pass fields on to be created as hidden fields
 *
 * @param &$object
 *    The form object although it is actually an array - I should have called it this
 * @param $name - optional
 *    The name of the field to be passed.
 *    This gets passed onto get parsed by default as a #type item and hidden tag
 *    It is handled by the restify_parse_show_form_values function.
 * @return Nowt
 */
function restify_convert_to_hidden_fields(&$object, $name=null) {
  $convert_to_strings = array(
    'textfield', 
    'textarea', 
    'select', 
    'date', 
    'radio', 
    'checkbox', 
    'radios', 
    'tel', 
    'cell', 
    'yubin', 
    'card', 
    'card_expiry', 
    'password',
  );
  if(in_array($object['#type'], $convert_to_strings)) {
    if(is_array($_POST[$name]) || strlen($_POST[$name])>0) {
      restify_parse_show_form_values($object, $name);
    }
    else {
      $object = NULL;
    }
  }
  elseif(has_children($object)) {
    foreach(element_children($object) as $child) {
      restify_convert_to_hidden_fields($object[$child], $child);
    }
  }
}


/**
 * Parses by default as a #type item and hidden tag
 *
 * @param &$object
 *    The form object although it is actually an array - I should have called it this
 * @param $name
 *    The name of the field to be parsed.
 * @return Nowt
 */
function restify_parse_show_form_values(&$object, $name) {

  $prefix = '<div class="kakunin">';
  if($object['#type']=='date') {
    $object['#value'] = $_POST[$name]['year'].'年'.$_POST[$name]['month'].'月'.$_POST[$name]['day'].'日';
    foreach($_POST[$name] as $key=>$val) {
       $prefix .= '<input type="hidden" name="'.$name.'['.$key.']" value="'.$val.'" />';
    }
  }
  elseif($object['#type']=='tel'||$object['#type']=='cell'||$object['#type']=='yubin'||$object['#type']=='card'||$object['#type']=='card_expiry') {
    $data_flag = false;
    foreach($_POST[$name] as $val) {
      if(!empty($val)) {
        $data_flag = true;
      }
    }
    if($data_flag) {
      $object['#value'] = implode('-', $_POST[$name]);
    }
    else {
      $object['#value'] = '';
    }
    foreach ($_POST[$name] as $key=>$val) {
       $prefix .= '<input type="hidden" name="'.$name.'['.$key.']" value="'.$val.'" />';
    }
    if ($object['#type'] == 'card') {
      $object['#value'] = preg_replace('/[0-9]{4}-/', 'XXXX-', $object['#value']);
    }
  }
  elseif ($object['#type'] == 'password')  {
    $prefix.='<input type="hidden" name="'.$name.'" value="'.$_POST[$name].'" />';
    $object['#value'] = preg_replace('/[^\*]/', '*', $_POST[$name]);
  }
  else {
    $prefix.='<input type="hidden" name="'.$name.'" value="'.$_POST[$name].'" />';
    if ($object['#type']=='select' && is_numeric($_POST[$name])) {
      $object['#value'] = $object['#options'][$_POST[$name]];
    }
    else {
      $object['#value'] = $_POST[$name];
    }
  }
  
  $object['#suffix']  = '</div>';
  $object['#prefix'] = $prefix;
  $object['#type']='item';
  unset($object['#description']);
} 

/**
 * Checks if the form item has children
 *
 * @param $object
 *    The form item object although it is actually an array - I should have called it this
 * @return true or false
 */
function has_children($object) {
  if(count(element_children($object)>0)) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/**
 * Checks if the form has errors
 *
 * @return true or false
 */
function is_form_errors() {
  if (count(form_get_errors())>0 || count(drupal_get_messages('error', FALSE))) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}