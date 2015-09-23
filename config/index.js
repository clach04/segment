'use strict';

var hashData = JSON.parse('$$DATA$$' || null);
var settings = {
  preset: hashData.preset || 'preset-0',
  colorBackground: hashData.colorBackground || '0x000000',
  colorHour: hashData.colorHour || '0xAA0000',
  colorMinLeft: hashData.colorMinLeft || '0xAAAAAA',
  colorMinRight: hashData.colorMinRight || '0xFFFFFF'
};

function setColors() {
  $('.item-color').each(function() {
    var $this = $(this);
    var name = $this.attr('name');

    $this.val(settings[name]).trigger('change', true);
  });
}

$(function() {
  // Initialize Things
  $('#' + settings.preset).prop('selected', true);
  setColors();

  // event listeners

  $('[name="preset"]').change(function() {
    var $this = $(this);

    var colorsArr = $this.val().split(',');

    if (colorsArr.length > 1) {
      settings.colorBackground = colorsArr[0];
      settings.colorHour = colorsArr[1];
      settings.colorMinLeft = colorsArr[2];
      settings.colorMinRight = colorsArr[3];
      settings.preset =
        $this.find('option').not(function() { return !this.selected; }).attr('id');

      setColors();
    } else {
      settings.preset = 'preset-custom';
    }
  });

  $('.item-color').change(function(event, keepPreset) {
    var $this = $(this);
    var name = $this.attr('name');

    settings[name] = $this.val();
    if (!keepPreset) {
      $('#preset-select').val('').trigger('change');
    }
  });

  $('#save-btn').click(function() {
    // window.localStorage.setItem('settings', JSON.stringify(settings));
    location.href = 'pebblejs://close#' + encodeURIComponent(JSON.stringify(settings));
  });

  $('#cancel-btn').click(function() {
    // window.localStorage.setItem('settings', JSON.stringify(settings));
    location.href = 'pebblejs://close';
  });

  // all complete
  $('.item-container').removeClass('hide');
});
// Close config page and return data
// location.href = 'pebblejs://close#success';
