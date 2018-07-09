<?php

function _firebase_resource_access($op, $args) {

  // Don't let anonymous users user this at all.
  if (user_is_anonymous()) {
    return services_error(t('Authentication is required to use Firebase.'), 403);
  }

  // Firebase admin always have access.
  if (user_access('administer firebase')) { return TRUE; }

  global $user;

  switch ($op) {

    case 'update fcm message':

      $mid = $args[0];
      $data = $args[1];

      // The user must own the message.
      $message = fcm_message_load($mid);
      return $message && $message->uid == $user->uid;

      break;

    case 'message search':

      // This can return TRUE for now since the messages query condition down
      // the line is hard coded to the current user.
      return TRUE;

      break;

    case 'message bulk':

      $data = $args[0];

      switch ($data['op']) {

        case 'updateStatus':

          // Make sure the user owns each of the messages.
          $query = db_select('firebase_cloud_messaging_messages', 'messages')
            ->fields('messages', array('mid', 'uid'))
            ->condition('messages.mid', $data['mid'], 'IN')
            ->execute();
          while ($row = $query->fetchObject()) {
            if ($row->uid != $user->uid) {
              return services_error(t('You do not own message @mid.', array('@mid' => $row->mid)), 403);
            }
          }
          return TRUE;

          break;

      }

      break;

    case 'delete fcm token':

      // First make sure they have permission from Drupal, then make sure they
      // are the owner of the token.
      if (user_access($op)) {
        $id = $args[0];
        $tokenData = fcm_token_load($id);
        if ($tokenData->uid == $user->uid) {
          return TRUE;
        }
      }
      return FALSE;

      break;

  }

  // If we didn't figure it out, just access Drupal's default access check.
  return user_access($op);

}
