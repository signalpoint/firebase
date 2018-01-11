<?php

function firebase_cloud_messaging_send_form($form, $form_state) {
  if (!variable_get('firebase_cloud_messaging_server_key')) {
    drupal_set_message(t('You must set the server key to send a message.'), 'warning');
    return $form;
  }
  $form['message'] = array(
    '#type' => 'fieldset',
    '#title' => t('Message'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );

  $form['message']['uid'] = array(
    '#type' => 'textfield',
    '#title' => t('User ID(s)'),
    '#required' => TRUE
  );

  $form['message']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#required' => TRUE,
    '#default_value' => 'Hello world'
  );

  $form['message']['body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#required' => TRUE,
    '#default_value' => 'How are you?'
  );

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
//  dpm($userTokens);
  if (!$userTokens) {
    drupal_set_message(t('No tokens found for user @uid.', array(
      '@uid' => print_r($uid, TRUE)
    )), 'error');
    return;
  }

  // Load the token from its id.
//  $tokenData = fcm_token_load($form_state['values']['id']);
//  if (!$tokenData) {
//    drupal_set_message(t('Failed to send message'), 'error');
//    return;
//  }

  // Build the notification.
  $msg = array(
    'title' => $form_state['values']['title'],
    'body' => $form_state['values']['body']
  );

  $response = fcm_message_send($msg, $userTokens);

}
