<?php

/**
 * TOKENS
 */

function fcm_token_load($id) {
  $query = db_select('firebase_cloud_messaging_tokens', 'fcm')
    ->fields('fcm')
    ->condition('id', $id);
  return $query->execute()->fetchObject();
}

function fcm_token_save(&$data) {
  $new = !isset($data->id);

  // Set any missing defaults.
  if (!isset($data->uid)) { $data->uid = $GLOBALS['user']->uid; }
  if (!isset($data->created)) { $data->created = time(); }
  if (!isset($data->language)) { $data->language = LANGUAGE_NONE; }

  // Don't let the same token/platform be used twice, instead just load and
  // return the token data if they tried to save the same combo twice.
  if ($new) {
    $query = db_select('firebase_cloud_messaging_tokens', 't')
      ->fields('t')
      ->condition('t.token', $data->token)
      ->condition('t.platform', $data->platform)
      ->range(0, 1)
      ->execute();
    $result = $query->fetchObject();
    if ($result->id) {
      return $result;
    }
  }

  return drupal_write_record('firebase_cloud_messaging_tokens', $data) ?
    $data : NULL;
}

function fcm_token_delete($id) {
  return db_delete('firebase_cloud_messaging_tokens')
    ->condition('id', $id)
    ->execute();
}

/**
 * @param $uid {Number|Array}
 * @return array|null
 */
function fcm_user_tokens_load($uid) {
  $query = db_select('firebase_cloud_messaging_tokens', 't')
    ->fields('t', array('id', 'uid', 'token'));
  $query->condition('t.uid', $uid, is_array($uid) ? 'IN' : '=');

  $results = $query->execute();
  $rows = array();
  while ($row = $results->fetchObject()) {
    $rows[$row->id] = $row;
  }
  return !empty($rows) ? $rows : NULL;
}

/**
 * MULTICAST
 */
function fcm_multicast_save(&$data) {
  $new = !isset($data->multicast_id);

  // Set any missing defaults.
  if (!isset($data->created)) { $data->created = time(); }

  // Serialize the notification it.
  if (isset($data->notification) && !is_string($data->notification)) {
    $data->notification = serialize($data->notification);
  }

  // Serialize the data payload if any.
  if (isset($data->data) && !is_string($data->data)) {
    $data->data = serialize($data->data);
  }

  // Don't let the same multicast be saved twice, instead just load and return
  // the multicast data if they tried to save the same one twice..
  // @TODO why don't we just make the multicast_id column in the DB a primary
  // key, then it would just throw an integrity constraint violation.
  if ($new) {
    $query = db_select('firebase_cloud_messaging_multicast', 'm')
      ->fields('m')
      ->condition('m.multicast_id', $data->multicast_id)
      ->range(0, 1)
      ->execute();
    $result = $query->fetchObject();
    if ($result->mcid) {
      return $result;
    }
  }

  // @TODO this isn't ready for use with updating a record, need to add the
  // third parameter.
  return drupal_write_record('firebase_cloud_messaging_multicast', $data) ? $data : NULL;
}

/**
 * MESSAGES
 */

function fcm_message_send($notification, $userTokens, $data = array()) {

  // If the server key hasn't been set, throw an error and return.
  $serverKey = variable_get('firebase_cloud_messaging_server_key', '');
  if ($serverKey == '') {
    drupal_set_message(t('Firebase "Server key" has not been set.'), 'warning');
    return NULL;
  }

  // If no tokens were included, return.
  if (empty($userTokens)) {
    watchdog('fcm', 'No user tokens provided for fcm_message_send().');
    return NULL;
  }

  // Put together a collection of internal token ids and actual Firebase tokens.
  $tokenIds = [];
  $tokens = [];
  foreach ($userTokens as $tokenId => $tokenData) {
    $tokenIds[] = $tokenData->id;
    $tokens[] = $tokenData->token;
  }
//  dpm($tokenIds);
//  dpm('tokens');
//  dpm($tokens);

  // Any extra data?
  $hasData = !empty($data);

  // The click_action property seems to cause issues for Android, so we remove
  // it. We may have been using it wrong, so we're open to ideas here. But from
  // what we can tell, click_action is only used for web apps so the browser
  // knows where to go if the user clicks the notification when it pops up.
  // If you need a click_action, instead use platform specific requests:
  // @see https://firebase.google.com/docs/cloud-messaging/concept-options#customizing_a_message_across_platforms
  if (isset($notification['click_action'])) {
    watchdog('firebase', 'Removing click_action from $notification', NULL, WATCHDOG_WARNING);
    unset($notification['click_action']);
  }

  // Set up the fields to POST. This includes the notification, and if it's not
  // empty, the data.
  $fields = array('notification' => $notification);
  if ($hasData) { $fields['data'] = $data; }

  // DEBUG - prior to tokens being attached (for readability)
//  watchdog('fcm', 'fcm_message_send()<pre>' . print_r($fields, TRUE) . '</pre>');

  // If there is only a single token use it alongside "to" as the field,
  // otherwise use "registration_ids" along with an array of tokens. Without
  // this we seem to get hit with the InvalidRegistration error from Firebase
  // for a single iOS token: @see https://stackoverflow.com/a/49765050/763010
  if (sizeof($tokens) == 1) { $fields['to'] = $tokens[0]; }
  else { $fields['registration_ids'] = $tokens; }

//  dpm('fields');
//  dpm($fields);

  // Set up the headers.
  $headers = array(
    'Authorization: key=' . $serverKey,
    'Content-Type: application/json'
  );

  // Send everything to Firebase via CURL.
  $ch = curl_init();
  curl_setopt($ch, CURLOPT_URL, 'https://fcm.googleapis.com/fcm/send');
  curl_setopt($ch, CURLOPT_POST, true);
  curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
  curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
  curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($fields));
  $response = json_decode(curl_exec($ch));
  curl_close($ch);
//  dpm('response');
//  dpm($response);
//  drupal_set_message('<pre>' . print_r($response, TRUE) . '</pre>');

  // If we have a response with success and results, assemble our multicast from
  // the FCM response.
  $multicast = NULL;
  if (isset($response->results) && is_array($response->results) && sizeof($response->results)) {

    // @TODO watch out for errors here

    // Save the multicast and the notification data if there was at least one
    // successful delivery.
    if ($response->success) {
      $multicast = new stdClass();
      $multicast->multicast_id = $response->multicast_id;
      $multicast->notification = $notification;
      if ($hasData) { $multicast->data = $data; }
      fcm_multicast_save($multicast);
    }

    // Prepare to track the user ids of all the successful message recipients,
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

  // Return the response and multicast bundles as a result.
  return [
    'response' => $response,
    'multicast' => $multicast
  ];

}

function fcm_message_save($data) {
  $new = !isset($data->mid);
  if ($new) {
    return drupal_write_record('firebase_cloud_messaging_messages', $data) ?
      $data : NULL;
  }
  else {
    return drupal_write_record('firebase_cloud_messaging_messages', $data, 'mid') ?
      $data : NULL;
  }
}

function fcm_message_search($data) {

  global $user;
  $uid = $user->uid;

  // Check for any 'limit' on the query range.
  $max = 100;
  $limit = isset($data['limit']) ? $data['limit'] : $max;
  if ($limit > $max) { $limit = $max; }

  $query = db_select('firebase_cloud_messaging_messages', 'messages');
  $query->join('firebase_cloud_messaging_multicast', 'multicast', 'messages.mcid = multicast.mcid');
  $query->fields('messages', array('mid', 'status'));
  $query->fields('multicast', array('mcid', 'notification', 'created'));
  $query->condition('messages.uid', $uid);
  $query->orderBy('multicast.created', 'DESC');
  $query->range(0, $limit);

  $results = $query->execute();
  $rows = array();
  while ($row = $results->fetchObject()) {
    $row->notification = unserialize($row->notification);
    $row->mid = (int) $row->mid;
    $row->mcid = (int) $row->mcid;
    $row->created = (int) $row->created;
    $row->status = (int) $row->status;
    $rows[] = $row;
  }
  return !empty($rows) ? $rows : NULL;

}

function fcm_message_bulk($data) {

  switch ($data['op']) {

    case 'updateStatus':

      // Validate the message type.
      if (!firebase_message_status_valid($data['status'])) {
        watchdog('firebase', t('Invalid message status @status.', array('@status' => $data['status'])));
        return FALSE;
      }

      // Update the message(s) type.
      db_update('firebase_cloud_messaging_messages')
        ->fields(array('status' => $data['status']))
        ->condition('mid', $data['mid'], 'IN')
        ->execute();

      return TRUE;

      break;

  }

  return FALSE;

}
