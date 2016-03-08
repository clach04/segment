'use strict';

module.exports = [
  {
    type: 'heading',
    defaultValue: 'Segment',
    size: 1
  },
  {
    type: 'text',
    defaultValue:
      'By Keegan Lillo - <a href="http://lillo.me" target="_blank">Lillo.me'
  },
  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Personalize'
      },
      {
        type: 'select',
        label: 'Preset',
        appKey: 'preset',
        options: [
          {label: '--Custom--', value: ''},
          {label: 'Black White Grey', value: '000000,ffffff,aaaaaa,aaaaaa'},
          {label: 'Black Grey White', value: '000000,aaaaaa,ffffff,ffffff'},
          {label: 'Grey Black White', value: 'aaaaaa,000000,ffffff,ffffff'},
          {label: 'Grey White Black', value: 'aaaaaa,ffffff,000000,000000'},
          {label: 'White Black Grey', value: 'ffffff,000000,aaaaaa,aaaaaa'},
          {label: 'White Grey Black', value: 'ffffff,aaaaaa,000000,000000'}
        ]
      },
      {
        type: 'color',
        appKey: 'colorBackground',
        defaultValue: '000000'
      },
      {
        type: 'color',
        appKey: 'colorHour',
        defaultValue: 'ffffff'
      },
      {
        type: 'color',
        appKey: 'colorMinLeft',
        defaultValue: 'aaaaaa'
      },
      {
        type: 'color',
        appKey: 'colorMinRight',
        defaultValue: 'aaaaaa'
      }
    ]
  },
  {
    type: 'submit',
    defaultValue: 'Save'
  }
];
