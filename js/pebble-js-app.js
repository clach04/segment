'use strict';

function utf8ToB64(str) {
  return encodeURIComponent(window.btoa(str));
}

function b64ToUtf8(str) {
  return window.atob(decodeURIComponent(str));
}

Pebble.addEventListener('ready', function(e) {
  console.log('JavaScript app ready and running!');
});

Pebble.addEventListener('showConfiguration', function(e) {
  var settings;
  try {
    settings = JSON.parse(localStorage.getItem('settings')) || {};
  } catch (e) {
    console.log(e);
    settings = {};
  }
  // Show config page
  var html = '<%= configPage %>'.replace('data:text/html;base64,', '');
  html = b64ToUtf8(html);
  html = html.replace('$$DATA$$', JSON.stringify(settings));
  Pebble.openURL('data:text/html;base64,' + utf8ToB64(html));
});

Pebble.addEventListener('webviewclosed', function(e) {
  // Decode and parse config data as JSON
  var settings = JSON.parse(decodeURIComponent(e.response));
  if (!settings) return;

  localStorage.setItem('settings', JSON.stringify(settings));

  // Send settings to Pebble watchapp
  Pebble.sendAppMessage({
    colorBackground: parseInt(settings.colorBackground, 16),
    colorHour: parseInt(settings.colorHour, 16),
    colorMinLeft: parseInt(settings.colorMinLeft, 16),
    colorMinRight: parseInt(settings.colorMinRight, 16)
  }, function(e) {
    console.log('Sent config data to Pebble');
  }, function() {
    console.log('Failed to send config data!');
    console.log(JSON.stringify(e));

  });

});
