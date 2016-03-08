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
          {label: 'Blueberry', value: '000055,0055ff,55aaff,aaffff'},
          {label: 'Lost Woods', value: 'aaffaa,005500,00aa00,00ff00'},
          {label: 'Old Timey', value: 'FFFFFF,000000,555555,AAAAAA'},
          {label: 'Red Tole', value: '000000,AA0000,AAAAAA,FFFFFF'},
          {label: 'Sunburst', value: '550000,ff0000,ffaa00,ffff00'},
          {label: 'Wild Flower', value: '000000,ff0000,ff5500,ffaaaa'}
        ]
      },
      {
        type: 'color',
        appKey: 'colorBackground',
        label: 'Background',
        defaultValue: '000055'
      },
      {
        type: 'color',
        appKey: 'colorHour',
        label: 'Hour',
        defaultValue: '0055ff'
      },
      {
        type: 'color',
        appKey: 'colorMinLeft',
        label: 'Left Minute Digit',
        defaultValue: '55aaff'
      },
      {
        type: 'color',
        appKey: 'colorMinRight',
        label: 'Right Minute Digit',
        defaultValue: 'aaffff'
      }
    ]
  },
  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Direct Input'
      },
      {
        type: 'input',
        id: 'directInput',
        defaultValue: '55,55ff,55aaff,aaffff',
        label: 'Copy the contents of this field to share your ' +
               'color combination with a friend.'
      },
      {
        type: 'button',
        id: 'directInputBtn',
        defaultValue: 'Load Direct Input'
      }
    ]
  },
  {
    type: 'submit',
    defaultValue: 'Save'
  }
];
