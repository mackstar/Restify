<?php

/*
 * This is going to get huge so bringing it into its own file
 */
function restify_menu() {
	
  // forms need the extra argument for the id
	$items['create/%'] = array(
		'page callback' => 'restify_create',
		'page arguments' => array(1),
		'access callback' => TRUE,
		'type' => MENU_NORMAL_ITEM,
	);
	
	$items['create/%/%'] = array(
		'page callback' => 'restify_create',
		'page arguments' => array(1, false,  2),
		'access callback' => true,
		'type' => MENU_NORMAL_ITEM
	);
	
	// forms need the extra argument for the id
	$items['update/%'] = array(
		'page callback' => 'restify_update',
		'page arguments' => array(1),
		'access callback' => TRUE,
	);
	$items['update/%/%'] = array(
		'page callback' => 'restify_rest_update',
		'page arguments' => array(1, 2),
		'access callback' => TRUE,
	);
	$items['destroy/%/%']=array(
		'page callback' => 'restify_destroy',
		'page arguments' => array(1, 2),
		'access callback' => true
	);
	
	$items['list/%'] = array(
		'page callback' => 'restify_list',
		'page arguments' => array(1),
		'access callback' => TRUE,
		'type' => MENU_CALLBACK,
		'weight'=>1
	);
	
	$items['list/%/%'] = $items['list/%'];
	$items['list/%/%']['page arguments']=array(1,2,true);
	
	$items['detail/%/%'] = array(
		'page callback' => 'restify_detail',
		'page arguments' => array(1, 2),
		'access callback' => true
	);

	return $items;
}