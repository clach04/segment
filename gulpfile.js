'use strict';

var gulp = require('gulp');
var dataUriStream = require('gulp-data-uri-stream');
var del = require('del');
var template = require('gulp-template');
var data = require('gulp-data');

gulp.task('clean', function(done) {
  del(['src/js/pebble-js-app.js', 'tmp']).then(function() {
    done();
  });
});

gulp.task('build', ['clean'], function() {
  var jsCompile = function(file) {

    // @TODO this is super weird but it works...
    gulp.src('js/pebble-js-app.js')
      .pipe(template({configPage: file.contents.toString()}))
      .pipe(gulp.dest('tmp/'));
  };

  gulp.src('config/index.html')
    .pipe(dataUriStream())
    .pipe(data(jsCompile));
});

gulp.task('default', ['build']);
