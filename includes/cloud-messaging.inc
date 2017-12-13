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

  return drupal_write_record('firebase_cloud_messaging_multicast', $data) ?
    $data : NULL;
}

/**
 * MESSAGES
 */

function fcm_message_search($data) {

  // fields
  // conditions
  // orderBy
  // groupBy
  // limit
  // page

  return $data;

}
