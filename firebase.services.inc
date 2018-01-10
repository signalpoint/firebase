<?php

function firebase_services_resources() {
  $resources = array(
    'fcm_token' => array(
      'create' => array(
        'help' => 'Creates a Firebase Cloud Messaging token.',
        'file' => array(
          'type' => 'inc',
          'module' => 'firebase',
          'name' => 'resources/resource.cloud-messaging'
        ),
        'callback' => '_fcm_token_create',
        'args' => array(
          array(
            'name' => 'data',
            'type' => 'array',
            'description' => 'The token data.',
            'source' => 'data',
            'optional' => FALSE,
          )
        ),
        'access callback' => '_firebase_resource_access',
        'access callback file' => array(
          'type' => 'inc',
          'module' => 'firebase',
          'name' => 'firebase.resource',
        ),
        'access arguments' => array('create fcm token'),
        'access arguments append' => TRUE,
      ),
      'retrieve' => array(
        'help' => 'Retrieves a Firebase Cloud Messaging token.',
        'file' => array(
          'type' => 'inc',
          'module' => 'firebase',
          'name' => 'resources/resource.cloud-messaging'
        ),
        'callback' => '_fcm_token_retrieve',
        'args' => array(
          array(
            'name' => 'id',
            'type' => 'int',
            'description' => 'The token ID.',
            'source' => array('path' => '0'),
            'optional' => FALSE,
          ),
        ),
        'access callback' => '_firebase_resource_access',
        'access callback file' => array(
          'type' => 'inc',
          'module' => 'firebase',
          'name' => 'firebase.resource',
        ),
        'access arguments' => array('retrieve fcm token'),
        'access arguments append' => TRUE,
      ),
      'update' => array(
        'help' => 'Updates a Firebase Cloud Messaging token.',
        'file' => array(
          'type' => 'inc',
          'module' => 'firebase',
          'name' => 'resources/resource.cloud-messaging'
        ),
        'callback' => '_fcm_token_update',
        'args' => array(
          array(
            'name' => 'id',
            'type' => 'int',
            'description' => 'The token ID.',
            'source' => array('path' => '0'),
            'optional' => FALSE,
          ),
          array(
            'name' => 'data',
            'type' => 'array',
            'description' => 'The token data.',
            'source' => 'data',
            'optional' => FALSE,
          )
        ),
        'access callback' => '_firebase_resource_access',
        'access callback file' => array(
          'type' => 'inc',
          'module' => 'firebase',
          'name' => 'firebase.resource',
        ),
        'access arguments' => array('update fcm token'),
        'access arguments append' => TRUE,
      ),
      'delete' => array(
        'help' => 'Deletes a Firebase Cloud Messaging token.',
        'file' => array(
          'type' => 'inc',
          'module' => 'firebase',
          'name' => 'resources/resource.cloud-messaging'
        ),
        'callback' => '_fcm_token_delete',
        'args' => array(
          array(
            'name' => 'id',
            'type' => 'int',
            'description' => 'The token ID.',
            'source' => array('path' => '0'),
            'optional' => FALSE,
          ),
        ),
        'access callback' => '_firebase_resource_access',
        'access callback file' => array(
          'type' => 'inc',
          'module' => 'firebase',
          'name' => 'firebase.resource',
        ),
        'access arguments' => array('delete fcm token'),
        'access arguments append' => TRUE,
      )
    ),
    'fcm_message' => array(
      'retrieve' => array(
        'help' => 'Retrieves a Firebase Cloud Messaging message.',
        'file' => array(
          'type' => 'inc',
          'module' => 'firebase',
          'name' => 'resources/resource.cloud-messaging'
        ),
        'callback' => '_fcm_message_retrieve',
        'args' => array(
          array(
            'name' => 'id',
            'type' => 'int',
            'description' => 'The message ID.',
            'source' => array('path' => '0'),
            'optional' => FALSE,
          ),
        ),
        'access callback' => '_firebase_resource_access',
        'access callback file' => array(
          'type' => 'inc',
          'module' => 'firebase',
          'name' => 'firebase.resource',
        ),
        'access arguments' => array('retrieve fcm message'),
        'access arguments append' => TRUE,
      ),
      'update' => array(
        'help' => 'Updates a Firebase Cloud Messaging message.',
        'file' => array(
          'type' => 'inc',
          'module' => 'firebase',
          'name' => 'resources/resource.cloud-messaging'
        ),
        'callback' => '_fcm_message_update',
        'args' => array(
          array(
            'name' => 'id',
            'type' => 'int',
            'description' => 'The message ID.',
            'source' => array('path' => '0'),
            'optional' => FALSE,
          ),
          array(
            'name' => 'data',
            'type' => 'array',
            'description' => 'The message data.',
            'source' => 'data',
            'optional' => FALSE,
          )
        ),
        'access callback' => '_firebase_resource_access',
        'access callback file' => array(
          'type' => 'inc',
          'module' => 'firebase',
          'name' => 'firebase.resource',
        ),
        'access arguments' => array('update fcm message'),
        'access arguments append' => TRUE,
      ),
      'delete' => array(
        'help' => 'Deletes a Firebase Cloud Messaging message.',
        'file' => array(
          'type' => 'inc',
          'module' => 'firebase',
          'name' => 'resources/resource.cloud-messaging'
        ),
        'callback' => '_fcm_message_delete',
        'args' => array(
          array(
            'name' => 'id',
            'type' => 'int',
            'description' => 'The message ID.',
            'source' => array('path' => '0'),
            'optional' => FALSE,
          ),
        ),
        'access callback' => '_firebase_resource_access',
        'access callback file' => array(
          'type' => 'inc',
          'module' => 'firebase',
          'name' => 'firebase.resource',
        ),
        'access arguments' => array('delete fcm message'),
        'access arguments append' => TRUE,
      ),
      'actions' => array(
        'search' => array(
          'help' => t('Searches through messages.'),
          'file' => array(
            'type' => 'inc',
            'module' => 'firebase',
            'name' => 'resources/resource.cloud-messaging'
          ),
          'callback' => '_fcm_message_search',
          'args' => array(
            array(
              'name' => 'data',
              'type' => 'struct',
              'description' => t('The message search data.'),
              'source' => 'data',
              'optional' => FALSE
            )
          ),
          'access callback' => '_firebase_resource_access',
          'access callback file' => array(
            'type' => 'inc',
            'module' => 'firebase',
            'name' => 'firebase.resource'
          ),
          'access arguments' => array('message search'),
          'access arguments append' => TRUE
        ),
        'bulk' => array(
          'help' => t('Bulk operations for messages.'),
          'file' => array(
            'type' => 'inc',
            'module' => 'firebase',
            'name' => 'resources/resource.cloud-messaging'
          ),
          'callback' => '_fcm_message_bulk',
          'args' => array(
            array(
              'name' => 'data',
              'type' => 'struct',
              'description' => t('The bulk data operations.'),
              'source' => 'data',
              'optional' => FALSE
            )
          ),
          'access callback' => '_firebase_resource_access',
          'access callback file' => array(
            'type' => 'inc',
            'module' => 'firebase',
            'name' => 'firebase.resource'
          ),
          'access arguments' => array('message bulk'),
          'access arguments append' => TRUE
        )
      )
    )
  );
  return $resources;
}
