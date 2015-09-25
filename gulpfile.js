'use strict';

var gulp = require('gulp');
var dataUriStream = require('gulp-data-uri-stream');
var del = require('del');
var template = require('gulp-template');
var data = require('gulp-data');
var inline = require('gulp-inline');
var concat = require('gulp-concat');
var minifyInline = require('gulp-minify-inline');
var minifyHTML = require('gulp-minify-html');

gulp.task('clean', function(done) {
  del(['src/js/pebble-js-app.js', 'tmp']).then(function() {
    done();
  });
});

gulp.task('build', ['clean'], function() {
  var jsCompile = function(file) {

    // @TODO this is super weird but it works...
    gulp.src(['bower_components/base64/base64.min.js', 'js/pebble-js-app.js'])
      .pipe(concat('pebble-js-app.js'))
      .pipe(template({configPage: file.contents.toString()}))
      .pipe(gulp.dest('src/js/'));
  };

  gulp.src('config/index.html')
    .pipe(template({versionLabel: require('./appinfo.json').versionLabel}))
    .pipe(inline())
    .pipe(minifyInline({
      js: {},
      jsSelector: 'script[uglify]'
    }))
    .pipe(minifyHTML())
    .pipe(gulp.dest('tmp/'))
    .pipe(dataUriStream())
    .pipe(data(jsCompile));
});

gulp.task('default', ['build']);
