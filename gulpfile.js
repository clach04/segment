'use strict';

var gulp = require('gulp');
var dataUriStream = require('gulp-data-uri-stream');
var del = require('del');
var template = require('gulp-template');
var data = require('gulp-data');
var inline = require('gulp-inline');
var uglify = require('gulp-uglify');

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
      .pipe(gulp.dest('src/js/'));
  };

  gulp.src('config/index.html')
    .pipe(inline({
      base: 'public/',
      //js: uglify()
      // css: minifyCss(),
      // disabledTypes: ['svg', 'img', 'js'], // Only inline css files
      // ignore: ['./css/do-not-inline-me.css']
    }))
    .pipe(dataUriStream())
    .pipe(data(jsCompile));
});

gulp.task('default', ['build']);
