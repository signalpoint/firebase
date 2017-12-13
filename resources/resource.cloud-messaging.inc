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

function _fcm_message_search($data) {
  return fcm_message_search($data);
}