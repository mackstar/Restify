<?php
/**
 * main forms controller
 * @param confirm
 * 
 */
function restify_create($controller, $action=false, $id=null) {
  
  global $json_data;
  restify_check_delivery();
  $get_vars = $_GET;

  if ($action == "login") {
    drupal_goto ('create/login/new', array('destination'=>'create/'.$controller.'/'.$id));
    return;
  }
  // Takara Specific
  elseif ($_SERVER['REQUEST_METHOD']=='GET') {
    if ($action=='shinki') {
      $get_vars['shinki']=1; 
    }
    if (!empty($id)) {
      $get_vars['form_id']=$id;
    }
    get_json_data($controller, 'add', $get_vars);
  }
  else {
    $post_vars = $_POST;
    if (!empty($id)) {
       $post_vars['form_id']=$id; 
    }
    $post_vars = array_merge($post_vars, $get_vars);
    
    if (!isset($post_vars['confirm'])) {
      get_json_data($controller, 'create', array_merge($post_vars, array('confirm'=>1)));
    }
    else {
      
      unset($post_vars['confirm']);
      get_json_data($controller, 'create', $post_vars);
      
      if (!isset($_POST['form_id'])&&count(drupal_get_messages('error', FALSE))<1) {
        drupal_goto(isset($_GET['destination'])? $_GET['destination']:'thanks');
      }
    }  
    $form = drupal_get_form('restify_create_form');
    if (!is_form_errors()) {
      $form = drupal_get_form('restify_create_confirm_form');
    }
  }
  
  return isset($form)? $form : drupal_get_form('restify_create_form');
}

// If delivery is the same as address this creates the delivery fields automatically
function restify_check_delivery() {
  if ($_POST['delivery_is_address']==t('Yes')) {
    foreach($_POST as $key=>$val) {
      if (substr($key, 0, 8)=='address-') {
        $_POST[str_replace('address-', 'delivery-', $key)]=$val;
      }
    }
  }
}

 
function restify_create_form(&$form_state) {
  return restify_build_form_elements();
}

function restify_create_confirm_form(&$form_state) {
  return restify_build_form_elements();
}

function restify_create_form_submit($form, &$form_state) {
  global $json_data;
  $request_url = explode('?', substr(request_uri(), 1));
  
  if (isset($json_data->info->urikake_fail)) {
    header('Location: /thanks/checkout/'.$json_data->info->order);
    exit;
  }
          
  if (isset($_POST['confirm'])&&!is_form_errors()) {
    if (isset($json_data->info->order) && arg(1)!='kessai') {
      $url = '/create/kessai/'.$json_data->info->order;
    }
    elseif (isset($json_data->info->order)) {
      $url = '/thanks/checkout/'.$json_data->info->order; 
    }
    elseif (isset($json_data->info->mitsumori)) {
      $url = '/thanks/mitsumoris/'.$json_data->info->mitsumori;
    }
    elseif ($request_url[0] == $_GET['destination']) {
      $url = '/'.$_GET['destination'].'?destination='.$_GET['destination'];
    }
    else{
      $url = isset($_GET['destination'])? '/'.$_GET['destination']:'/thanks';
      $url.= isset($_GET['query'])? '?'.urldecode($_GET['query']):'';
    }
    header('Location: '.$url);
    exit;
  }
}



function restify_create_confirm_form_submit($form, &$form_state) {
  restify_create_form_submit($form, $form_state);
}

function restify_build_form_elements() {
  
  global $json_data;
  $form_keys = array(
    'type',
    'label',
    'key',
    'announce',
    'width',
    'height',
    'default_content',
    'hitsuyou',
    'collapsible',
    'collapsed',
    'selected',
    'required_show'
  ); 

  // Set page title from form
  $form['#redirect'] = FALSE;
  $weight = 0;

  //iteration form elements to create form
  if ($json_data->form->form_elements!=NULL) {
    foreach ($json_data->form->form_elements as $form_element) {

      $weight++;
      //iteration of form keys to set local variables (type label etc)
      foreach ($form_keys as $form_key) {
        if (isset($form_element->$form_key) && strlen($form_element->$form_key)>0) {
          ${$form_key}=$form_element->$form_key;
        }
        else {
          ${$form_key} = "";
        }
      }

      if (strlen($key)<1) {
        $key = "mitsumori-comment-{$label}";
      }

      $option = explode("\n",$default_content);
      $options = "";//オプション初期化

      if (!isset($current_title)) {
        $current_title='form';
      }

      if ($required_show==1) {
        $label.=' <span class="form-required" title="'. t('This field is required.') .'">*</span> ';
      }

      if ($type == 'title') {
        $form[$key] = array(
          '#description' => t($announce),
          '#type' => 'fieldset',
          '#weight' => $weight,
          '#attributes' => array('id' => 'title-'.$key),
          '#title' => strlen($label)>0? $label : $default_content,
          '#collapsible' => $collapsed? TRUE : FALSE,
          '#collapsed' => $collapsed? TRUE : FALSE,
        );
        if (isset($current_embed)) {
          unset($current_embed);
        }
        $current_title = $key;
      }

      if ($type == 'embed_title') {
        $form[$current_title][$key] = array(
          '#description' => t($announce),
          '#type' => 'fieldset',
          '#weight' => $weight,
          '#title' => strlen($label)>0? $label : $default_content,
          '#collapsible' => $collapsed? TRUE : FALSE,
          '#collapsed' => $collapsed? TRUE : FALSE,
          '#attributes' => array('class' => 'inner'),
        );
        $current_embed=$key;
      }

      if ($type == 'escape_embed_title') {
      	unset($current_embed);
      }

      if ($type == 'text') {
        $item = array(
          '#type' => 'textfield',
          '#title' => t($label),
          '#description' => t($announce),
          '#required' => ($hitsuyou==1)? TRUE : FALSE,
          '#size' => ($width>0)? $width : 50,
          '#default_value' => $default_content,
          '#weight' => $weight,
        );
        if (!isset($current_embed)) {
          $form[$current_title][$key] = $item;
        }
        else {
          $form[$current_title][$current_embed][$key] = $item;
        } 
      }

      if ($type == 'hidden') {
        $item = array(
          '#type' => 'hidden',
          '#value' => $default_content,
        );
        $form[$key] = $item;
      }

      if ($type == 'show') {
        $item = array(
          '#type' => 'item',
          '#title' => t($label),
          '#value' => $default_content,
          '#weight' => $weight,
          '#prefix' => '<div class="kakunin">',
          '#suffix' => '</div>',
        );
        if (!isset($current_embed)) {
          $form[$current_title][$key] = $item;
        }
        else {
          $form[$current_title][$current_embed][$key]  = $item;
        }
      }

      if ($type == 'textarea') {
        $form[$current_title][$key] = array(
          '#type' => $type,
          '#title' => t($label),
          '#cols' => isset($width)? $width : null,
          '#rows' => $height,
          '#required' => ($hitsuyou==1)? TRUE : FALSE,
          '#default_value' => $default_content,
          '#description' => t($announce),
          '#weight' => $weight,
        );
      }

      if ($type == 'password') {
        $item = array(
          '#size' => ($width>0)? $width : 50,
          '#type' => 'password',
          '#required' => ($hitsuyou==1)? TRUE : FALSE,
          '#title' => t($label),
          '#weight' => $weight,
        );
        if (!isset($current_embed)) {
          $form[$current_title][$key] = $item;
        }
        else {
          $form[$current_title][$current_embed][$key]  = $item;
        }
      }

      if ($type == 'select') {
        $optgrp = false;
        // loop options
           for ($j=0;$j<count($option);$j++) {
          //assign optgp var if -- present
          if (strstr($option[$j], '--')) {
            $optgrp = true;
          }
          //if a separator is present we add the first as a key, the next as a value
          if (strstr($option[$j], ':')) {
            $items = explode(':', $option[$j]);
            $options[$items[0]] = $items[1];
            $no_merge_flag=true;
          // if there is no separator for keys and values we make the key and value the same  
          }else{
            $options[str_replace("\r",'',$option[$j])] = str_replace("\r",'',$option[$j]);
          }
        }
        $option = null;
        $new_options = array();
        // if optgrp is set
        if ($optgrp) {
          //loop over the options
          foreach ($options as $option) {
            //if current option is a section
            if (strstr($option, '--')) {
              $option = str_replace('--', '', $option);
              $new_options[$option] = array();
              $parent = $option;
            }
            else {
              $new_options[$parent][$option] = $option;
            }
          }
          $options = $new_options;
        }
        $key = strlen($key)> 0 ? $key : $label;
        $item = array(
          '#type' => $type,
          '#title' => t($label),
          '#required' => ($hitsuyou==1)? TRUE : FALSE,
          '#value'=> isset($_POST[$key])? $_POST[$key] : (isset($selected)? $selected : ''),
          '#options' => isset($no_merge_flag)? $options : array_merge(array(''=>"選択してください"), $options),
          '#description' => t($announce),
          '#weight' => $weight,
        );
        if (!isset($current_embed)) {
          $form[$current_title][$key] = $item;
        }
        else {
          $form[$current_title][$current_embed][$key]  = $item;
        }
      }

      if ($type == 'submit') {
        $form['submit'] = array(
          '#type' => $type,
          '#value' => strlen($key)>0? t($key) : '送信',
          '#weight' => $weight,
        );
      }

      if ($type == 'checkbox') {
        $form[$current_title]["checkbox_{$label}"] = array(
          '#type' => 'fieldset',
          '#title' => t((strlen($announce)>0)? $label.' - '.$announce : $label),
          '#collapsible' => FALSE,
          '#weight' => $weight,
          '#collapsed' => FALSE,
        );
        for ($j=0; $j < count($option); $j++) {
          $form[$current_title]["checkbox_{$label}"][$key.":".$option[$j]] = array(
            '#type' => $type,
            '#title' => t($option[$j]),
            '#attributes' => array('class' => $key),
          );
        }
      }

      if ($type == 'boolean') {
        $form[$current_title]["checkbox_{$label}"] = array(
          '#type' => 'fieldset',
          '#title' => t((strlen($announce)>0)? $label.' - '.$announce : $label),
          '#collapsible' => FALSE,
          '#weight' => $weight,
          '#collapsed' => FALSE,
        );
        $form[$current_title]["checkbox_{$label}"][$key] = array(
          '#type' => 'checkbox',
          '#title' => t($option[0]),
        );
      }

      if ($type == 'radio') {
        for ($j=0;$j<count($option);$j++) {
          $options[$option[$j]] = $option[$j];
        }
        $form[$current_title]['radio'.$key] = array(
          '#type' => 'fieldset',
          '#required' => ($hitsuyou==1)? TRUE : FALSE,
          '#title' => t($label.$announce),
          '#collapsible' => FALSE,
          '#collapsed' => FALSE,
          '#weight' => $weight,
        );
        $form[$current_title]['radio'.$key][$key] = array(
          '#type' => 'radios',
          '#default_value'=> !empty($selected)? $selected : '',
          '#options' => $options,
        );
      }

      if ($type == 'date') {
        $form[$current_title][$key] = array(
          '#type' => 'date',
          '#title' => t($label),
          '#process' => array('japanese_date_format'),
          '#required' => ($hitsuyou==1)? TRUE : FALSE,
          '#description' => t($announce),
          '#weight' => $weight,
          '#start_year' => date('Y'),
        );
      }

      if (
        $type == 'yubin'||$type == 'cell'||$type == 'card'||
        $type == 'tel'||$type == 'card_expiry'
      ) {
        $item = array(
          '#title' => t($label),
          '#type' => $type,
          '#required' => ($hitsuyou==1)? TRUE : FALSE,
          '#default_content' => isset($default_content)? $default_content : '',
          '#description' => t($announce),
          '#weight' => $weight,
        );
        if (!isset($current_embed)) {
          $form[$current_title][$key] = $item;
        }
        else {
          $form[$current_title][$current_embed][$key] = $item;
        } 
      }
    }

    if (!isset($form['submit'])) {
      $form['submit'] = array(
        '#type' => "submit",
        '#value' => '送信',
        '#weight' => $weight+1,
      );
    }

    $form['#action'] = $json_data->form->action;
  }
  return $form;
}

