<?php

function _firebase_resource_access($op, $args) {

  // Don't let anonymous users user this at all.
  if (user_is_anonymous()) {
    return services_error(t('Authentication is required to use Firebase.'), 403);
  }

  // @TODO this needs better access checking, i.e. people who can't administer
  // firebase should not be able to work with any tokens they do not own.

  // Now just route the op to Drupal's user access layer.
  return user_access($op);
}
