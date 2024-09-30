<?php

function firebase_cloud_messaging_send_form($form, $form_state) {

  // settings link
  $form['#prefix'] = l('Settings', 'admin/config/services/firebase', [
    'query' => drupal_get_destination(),
  ]);

  if (!firebase_cloud_messaging_ok()) {
    return $form;
  }

//  // BEGIN: GOOGLE DRIVE TEST
//
//  $scopes = [
//    'https://www.googleapis.com/auth/drive.readonly',
//  ];
//
//  $bearerToken = firebase_cloud_messaging_get_bearer_token();
//
//  cwDsm("Bearer Token: " . $bearerToken);
//
//  // create middleware
//  $middleware = ApplicationDefaultCredentials::getMiddleware($scopes);
//  $stack = HandlerStack::create();
//  $stack->push($middleware);
//
//  // create the HTTP client
//  $client = new Client([
//    'handler' => $stack,
//    'base_uri' => 'https://www.googleapis.com',
//    'auth' => 'google_auth'  // authorize all requests
//  ]);
//
//  // make the request
//  $response = $client->get('drive/v2/files');
//  cwDsm($response->getBody());
//
//  // END: GOOGLE DRIVE TEST

  // fieldset
  $form['message'] = array(
    '#type' => 'fieldset',
    '#title' => t('Message'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );

  // user id
  $form['message']['uid'] = array(
    '#type' => 'textfield',
    '#title' => t('User ID(s)'),
    '#required' => TRUE,
  );

  // title
  $form['message']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#required' => TRUE,
    '#default_value' => 'Hello world'
  );

  // body
  $form['message']['body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#required' => TRUE,
    '#default_value' => 'How are you?'
  );

  // submit
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Send',
  );

  return $form;

}

function firebase_cloud_messaging_send_form_submit($form, &$form_state) {

  // Load tokens for this user(s), if any.
  $uid = $form_state['values']['uid'];
  if (!is_numeric($uid)) {
    $uid = explode(',', $uid);
  }
  $userTokens = fcm_user_tokens_load($uid);
  if (!$userTokens) {
    drupal_set_message(t('No tokens found for user @uid.', array(
      '@uid' => print_r($uid, TRUE)
    )), 'error');
    return;
  }

  // Build the notification.
  $msg = array(
    'title' => $form_state['values']['title'],
    'body' => $form_state['values']['body']
  );

  $result = fcm_message_send($msg, $userTokens);

  // @TODO copy drupal_set_messages() from README to here.

}
