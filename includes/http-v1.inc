<?php

// @see https://firebase.google.com/docs/cloud-messaging/migrate-v1

require 'vendor/autoload.php';

use Google\Auth\CredentialsLoader;

function _firebase_cloud_messaging_get_bearer_token($reset = FALSE) {

  $bearerToken = NULL;

  // Grab the cache key.
  $cid = firebase_cloud_messaging_bearer_token_cache_key();

  // Try to load the response from cache (if we're not resetting).
  $cached = !$reset ? cache_get($cid) : NULL;

  // If the response was cached and not expired, return it.
  if ($cached && REQUEST_TIME < $cached->expire) {

    $bearerToken = $cached->data;

  }
  else {

    $scopes = firebase_cloud_messaging_scopes();

    // Load the credentials and create an OAuth2 token
    $credentials = CredentialsLoader::makeCredentials(
      $scopes, json_decode(file_get_contents(getenv('GOOGLE_APPLICATION_CREDENTIALS')), true)
    );

    // Fetch the access token
    $token = $credentials->fetchAuthToken();

    // Get the bearer token (access token)
    $bearerToken = $token['access_token'];

    // Cache it.
    cache_set($cid, $bearerToken, 'cache', strtotime('+1 hour'));

  }

  return $bearerToken;

}
