'use strict';

var Clay = require('pebble-clay');
var clayConfig = require('./config');
var customFn = require('./custom-function');
var clay = new Clay(clayConfig, customFn);
console.log('Clay Version: ' + clay.version);
