'use strict';

var hashData; try {
  hashData = JSON.parse('$$DATA$$' || {});
} catch (e) {
  hashData = {};
}
var settings = {
  preset: hashData.preset || 'preset-0',
  colorBackground: hashData.colorBackground || '0x000000',
  colorHour: hashData.colorHour || '0xAA0000',
  colorMinLeft: hashData.colorMinLeft || '0xAAAAAA',
  colorMinRight: hashData.colorMinRight || '0xFFFFFF'
};

var presets = [
  {name: 'Red Tole', val: '0x000000,0xAA0000,0xAAAAAA,0xFFFFFF'},
  {name: 'Old Timey', val: '0xFFFFFF,0x000000,0x555555,0xAAAAAA'}
];

function setColors() {
  $('.item-color').each(function() {
    var $this = $(this);
    var name = $this.attr('name');

    $this.val(settings[name]).trigger('change', true);
  });
}

function updateDirectInput() {
  $('#direct-input').val(
    (settings.colorBackground + ',' +
    settings.colorHour + ',' +
    settings.colorMinLeft + ',' +
    settings.colorMinRight).toLowerCase()
  );
}

function updateSettings() {
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
}

$(function() {
  // Initialize Things
  var $presetSelect = $('#preset-custom');
  presets.forEach(function(item, index) {
    $presetSelect.before(
      '<option class="item-select-option" ' +
      'id="preset-' + index + '"' +
      'value="' + item.val + '">' + item.name + '</option>'
    );
  });
  $('#' + settings.preset).prop('selected', true);
  setColors();
  updateDirectInput();

  // event listeners

  $('[name="preset"]').change(updateSettings);

  $('.item-color').change(function(event, keepPreset) {
    var $this = $(this);
    var name = $this.attr('name');

    settings[name] = $this.val();
    if (!keepPreset) {
      $('#preset-select').val('').trigger('change');
    }
    updateDirectInput();
  });

  $('#save-btn').click(function() {
    location.href = 'pebblejs://close#' +
                    encodeURIComponent(JSON.stringify(settings));
  });

  $('#cancel-btn').click(function() {
    location.href = 'pebblejs://close';
  });

  $('#direct-input-btn').click(function() {
    updateSettings.apply($('#direct-input'));
    settings.preset = 'preset-custom';
    $('#preset-select').val('');

  });

  // all complete
  $('.item-container').removeClass('hide');
});
