<?php

/**
 * TOKENS
 */

function _fcm_token_create($data) {
  $data = (object) $data;
  return fcm_token_save($data);
}

function _fcm_token_retrieve($id) {
  return fcm_token_load($id);
}

function _fcm_token_update($id, $data) {
  $data = (object) $data;
  if (!isset($data->id)) { $data->id = $id; }
  return fcm_token_save($data);
}

function _fcm_token_delete($id) {
  return fcm_token_delete($id);
}

/**
 * MESSAGES
 */

// PROXIES

function _fcm_message_update($mid, $data) {
  $data = (object) $data;
  if (!isset($data->mid)) { $data->mid = $mid; }
  return fcm_message_save($data);
}

function _fcm_message_search($data) {
  return fcm_message_search($data);
}

function _fcm_message_bulk($data) {
  return fcm_message_bulk($data);
}

// HELPERS

function fcm_message_load($mid) {
  $query = db_select('firebase_cloud_messaging_messages', 'messages')
    ->fields('messages')
    ->condition('messages.mid', $mid)
    ->execute();
  return $query->fetchObject();
}