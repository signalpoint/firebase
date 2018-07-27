# firebase

The Firebase module for Drupal 7.

# Send a message

When you'd like to send a message, add code like this to your custom Drupal module:

```
$notification = [];
$tokens = [];
$data = [];

// Send the message.
$result = fcm_message_send($notification, $tokens, $data);

// If there was a problem, stop.
if (!$result) {
  drupal_set_message(t('Unable to send message'), 'error');
  return;
}

// Grab the response and the multicast from the send message result.
$response = $result['response'];
$multicast = $result['multicast'];

// Mention any successful messages.
if ($response->success) {
  drupal_set_message(t('Successfully sent @count @plural.', array(
    '@count' => $response->success,
    '@plural' => format_plural($response->success, t('message'), t('messages'))
  )));
}

// Mention any failed messages.
if ($response->failure) {
  drupal_set_message(t('Failed to send @count @plural.', array(
    '@count' => $response->success,
    '@plural' => format_plural($response->success, t('message'), t('messages'))
  )), 'warning');
}
```
