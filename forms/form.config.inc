<?php

function firebase_config_form($form, &$form_state) {

  // fieldset
  $form['firebase_cloud_messaging'] = [
    '#type' => 'fieldset',
    '#title' => t('Firebase Cloud Messaging'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  ];

  // send message link
  $form['firebase_cloud_messaging']['link'] = [
    '#markup' => l(t('Send a Message'), 'admin/config/services/firebase/cloud-messaging/send'),
  ];

  // sender id
  $form['firebase_cloud_messaging']['firebase_cloud_messaging_sender_id'] = [
    '#type' => 'textfield',
    '#title' => t('Sender ID'),
    '#default_value' => variable_get('firebase_cloud_messaging_sender_id', '')
  ];

  // project id
  $form['firebase_cloud_messaging']['firebase_cloud_messaging_project_id'] = [
    '#type' => 'textfield',
    '#title' => t('Project ID'),
    '#default_value' => variable_get('firebase_cloud_messaging_project_id', ''),
  ];

  // private key path
  $form['firebase_cloud_messaging']['firebase_cloud_messaging_private_key_path'] = [
    '#type' => 'textfield',
    '#title' => t('Private Key Path'),
    '#default_value' => variable_get('firebase_cloud_messaging_private_key_path', ''),
  ];

  return system_settings_form($form);
}
