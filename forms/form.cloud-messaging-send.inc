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
    '#required' => TRUE,
//    '#default_value' => '12875',
    '#default_value' => '12875,4391'
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

//function firebase_cloud_messaging_send_form_validate($form, &$form_state) {
//  $year_of_birth = $form_state['values']['year_of_birth'];
//  if ($year_of_birth && ($year_of_birth < 1900 || $year_of_birth > 2000)) {
//    form_set_error('year_of_birth', t('Enter a year between 1900 and 2000.'));
//  }
//}

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

  // Put together a collection of internal token ids and actual Firebase tokens.
  $tokenIds = [];
  $tokens = [];
  foreach ($userTokens as $tokenId => $tokenData) {
    $tokenIds[] = $tokenData->id;
    $tokens[] = $tokenData->token;
  }
//  dpm($tokenIds);
//  dpm($tokens);


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

  // Set up the fields to POST.
  $fields = array(
//    'to' => $tokenData->token, // @todo apparently change 'to' to 'registration_ids' for multiple recipients
    'registration_ids' => $tokens, // @todo apparently change 'to' to 'registration_ids' for multiple recipients
    'notification' => $msg
  );

  // @TODO turn this into a function(s)!

  // Set up the headers.
  $headers = array(
    'Authorization: key=' . variable_get('firebase_cloud_messaging_server_key'),
    'Content-Type: application/json'
  );

  // Send everything to Firebase.
  $ch = curl_init();
  curl_setopt($ch, CURLOPT_URL, 'https://fcm.googleapis.com/fcm/send');
  curl_setopt($ch, CURLOPT_POST, true);
  curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
  curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
  curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($fields));
  $response = json_decode(curl_exec($ch));
  curl_close($ch);

  #Echo Result Of FireBase Server
//  dpm($response);

  if ($response->success) {
    drupal_set_message(t('Successfully sent @count @plural.', array(
      '@count' => $response->success,
      '@plural' => format_plural($response->success, t('message'), t('messages'))
    )));
  }

  if ($response->failure) {
    drupal_set_message(t('Failed to send @count @plural.', array(
      '@count' => $response->success,
      '@plural' => format_plural($response->success, t('message'), t('messages'))
    )), 'warning');
  }

  if (isset($response->results) && is_array($response->results) && sizeof($response->results)) {

    // @TODO watch out for errors here
    // @TODO watch out for NotRegistered responses, those tokens can be deleted

    // Save the multicast and the notification data if there was at least one
    // successful delivery.
    $multicast = NULL;
    if ($response->success) {
      $multicast = new stdClass();
      $multicast->multicast_id = $response->multicast_id;
      $multicast->notification = $msg;
      fcm_multicast_save($multicast);
    }

    // Prepare to track the user ids of  all the successful message recipients,
    // we'll need these ids to relate their messages to the DB later.
    $recipientUserIds = array();

    // Now process each message result...
    foreach ($response->results as $delta => $result) {

      // Grab our internal token id.
      $tokenId = $tokenIds[$delta];

      // Check for errors.
      if (isset($result->error)) {

        // Delete tokens that are no longer registered.
        if ($result->error == 'NotRegistered') {
          fcm_token_delete($tokenId);
        }

        continue; // Skip this token, there was an error.
      }

      // There were no errors with this message...

      if (isset($result->message_id)) {

        // Example message id: 0:1234567890%abcdefg

        $tokenData = $userTokens[$tokenId];
        $recipientUid = $tokenData->uid;

        // Mark this user id as a recipient, if it hasn't been already.
        if (!in_array($recipientUid, $recipientUserIds)) {
          $recipientUserIds[] = $recipientUid;
        }

      }

    }

    // We've gone over each message and have what we need to proceed...

    // Insert a message record for each recipient user id. The record will
    // reference the multicast id where the notification data is actually saved,
    // here we just care about tracking the status of the message: new, unread,
    // read, archived, etc.
    if ($multicast && isset($multicast->mcid) && !empty($recipientUserIds)) {
      $now = time();

      $query = db_insert('firebase_cloud_messaging_messages')->fields(
        array('mcid', 'uid', 'status', 'changed')
      );
      foreach ($recipientUserIds as $uid) {
        $query->values(array(
          'mcid' => $multicast->mcid,
          'uid' => $uid,
          'status' => FIREBASE_MESSAGE_STATUS_NEW,
          'changed' => $now
        ));
      }
      $query->execute();
    }

  }

}
