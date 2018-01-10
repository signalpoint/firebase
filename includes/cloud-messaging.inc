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

  // Serialize it.
  if (isset($data->notification) && !is_string($data->notification)) {
    $data->notification = serialize($data->notification);
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
  return drupal_write_record('firebase_cloud_messaging_multicast', $data) ?
    $data : NULL;
}

/**
 * MESSAGES
 */

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
      if (!firebase_message_status_valid($data['mid'])) {
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
