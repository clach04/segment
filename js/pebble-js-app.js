'use strict';

Pebble.addEventListener('ready', function(e) {
  console.log('JavaScript app ready and running!');
});

Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('<%= configPage %>');
});

