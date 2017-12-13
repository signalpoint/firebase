<?php

function firebase_config_form($form, &$form_state) {
  $form['firebase_cloud_messaging'] = array(
    '#type' => 'fieldset',
    '#title' => t('Firebase Cloud Messaging'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,

  );
  $form['firebase_cloud_messaging']['firebase_cloud_messaging_server_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Server key'),
    '#default_value' => variable_get('firebase_cloud_messaging_server_key', NULL),
    '#maxlength' => 255
  );
  $form['firebase_cloud_messaging']['firebase_cloud_messaging_sender_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Sender ID'),
    '#default_value' => variable_get('firebase_cloud_messaging_sender_id', NULL)
  );
  return system_settings_form($form);
}
