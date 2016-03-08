'use strict';

module.exports = function(minified) {
  /** @type {ClayConfig} */
  var Clay = this;

  function updateDirectInput() {
    var directInputItem = Clay.getItemById('directInput');
    var colors = Clay.getItemsByType('color').map(function(colorItem) {
      return colorItem.get().toString(16);
    });

    directInputItem.set(colors.join(','));
  }

  function setColorsFromPreset() {
    var preset = this.get().split(',');

    if (preset.length < 4) { return; }

    removeColorListeners();

    var colorItems = Clay.getItemsByType('color');
    preset.forEach(function(color, index) {
      colorItems[index].set(color);
    });
    updateDirectInput();

    addColorListeners();
  }

  function handleColorChange() {
    var presetItem = Clay.getItemByAppKey('preset');
    presetItem.set('');
    updateDirectInput();
  }

  function loadFromDirectInput() {
    var colorItems = Clay.getItemsByType('color');
    Clay.getItemById('directInput').get().split(',').forEach(function(color, index) {
      console.log('color', color);
      colorItems[index].set(color);
    });
  }

  function addColorListeners() {
    Clay.getItemsByType('color').forEach(function(colorItem) {
      colorItem.on('change', handleColorChange);
    });
  }

  function removeColorListeners() {
    Clay.getItemsByType('color').forEach(function(colorItem) {
      colorItem.off(handleColorChange);
    });
  }

  Clay.on(Clay.EVENTS.AFTER_BUILD, function() {
    var presetItem = Clay.getItemByAppKey('preset');
    presetItem.on('change', setColorsFromPreset);
    addColorListeners();

    var directInputBtn = Clay.getItemById('directInputBtn');
    if (directInputBtn) {
      directInputBtn.$manipulatorTarget.on('click', loadFromDirectInput);
    }

    if (Clay.meta.activeWatchInfo.platform === 'aplite') {
      Clay.getItemsByType('color').forEach(function(colorItem) {
        colorItem.hide();
      });
    }
  });
};
