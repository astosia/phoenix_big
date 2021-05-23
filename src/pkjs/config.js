// Clay Config: see https://github.com/pebble/clay
module.exports = [
  {
    "type": "heading",
    "defaultValue": "Settings"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Theme settings"
      },
      {
          "type": "toggle",
          "messageKey": "HealthOff",
          "label": "Switch off Steps",
          "defaultValue": true,
          "capabilities":["HEALTH"]
        },
        {
          "type": "toggle",
          "messageKey": "AddZero12h",
          "label": "Add leading zero to 12h time",
          "defaultValue": false
        },
        {
          "type": "toggle",
          "messageKey": "RemoveZero24h",
          "label": "Remove leading zero from 24h time",
          "defaultValue": false
        },
  /*    {
        "type":"toggle",
        "messageKey":"Rotate",
        "label":"Rotate by 90°?",
        "defaultValue":false,
      },
       {
           "type": "toggle",
           "messageKey": "RightLeft",
          "label": "Rotation direction",
          "description": "On to rotate Right/clockwise, off to rotate Left/anti-clockwise.",
             "defaultValue": true,
           },
      {
        "type": "color",
        "messageKey": "Back1Color",
        "defaultValue": "0x000000",
        "label": "Background"
      },
       {
        "type": "color",
        "messageKey": "FrameColor",
        "defaultValue": "0x000000",
        "label": "Time Background Colour"
      },*/
    /*   {
        "type": "color",
        "messageKey": "FrameColor2",
        "defaultValue": "0x000000",
        "label": "Minute Background Colour"
      },*/
      {
        "type": "color",
        "messageKey": "FrameColor1",
        "defaultValue": "0x000000",
        "label": "Left Sidebar Colour",
        "capabilities":["ROUND"]
      },
      {
        "type": "color",
        "messageKey": "SideColor1",
        "defaultValue": "0x000000",
        "label": "Minute Background Colour",
        "allowGray":true
      },
       {
        "type": "color",
        "messageKey": "SideColor2",
        "defaultValue": "0x000000",
        "label": "Hour Background Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "HourColor",
        "defaultValue": "0x00FFFF",
        "label": "Hour Text Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "MinColor",
        "defaultValue": "0x00FFFF",
        "label": "Minute Text Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "Text1Color",
        "defaultValue": "0xFFFFFF",
        "label": "Steps Colour",
        "allowGray":true,
        "capabilities":["HEALTH"]
      },
      {
        "type": "color",
        "messageKey": "Text3Color",
        "defaultValue": "0xFFFFFF",
        "label": "Day & Battery Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "Text6Color",
        "defaultValue": "0xFFFFFF",
        "label": "Date Colour",
        "allowGray":true
      },
       {
        "type": "color",
        "messageKey": "Text2Color",
        "defaultValue": "0xFFFFFF",
        "label": "Sunset Colour",
        "allowGray":true
      },
      {
        "type":"color",
        "messageKey":"Text4Color",
        "defaultValue":"0xFFFFFF",
        "label":"Moon Colour",
        "allowGray":true
      },
      {"type": "section",
       "items": [
         {
           "type": "heading",
           "defaultValue": "Night Theme",
           "size":4
         } ,
         {
           "type": "toggle",
           "messageKey": "NightTheme",
           "label": "Activate Night Theme",
           "defaultValue": false,
         },
         /*{
           "type": "color",
           "messageKey": "Back1ColorN",
           "defaultValue": "0xFFFFFF",
           "label": "Background"
         },
                  {
        "type": "color",
        "messageKey": "FrameColorN",
        "defaultValue": "0xFFFFFF",
        "label": "Time Background Colour",
          "allowGray":true
      },*/
      /* {
        "type": "color",
        "messageKey": "FrameColor2N",
        "defaultValue": "0xFFFFFF",
        "label": "Minute Background Colour"
      },*/
      {
        "type": "color",
        "messageKey": "FrameColor1N",
        "defaultValue": "0xFFFFFF",
        "label": "Left Sidebar Colour",
        "capabilities":["ROUND"]
      },
             {
        "type": "color",
        "messageKey": "SideColor1N",
        "defaultValue": "0xFFFFFF",
        "label": "Right Background Colour",
        "allowGray":true
      },
       {
        "type": "color",
        "messageKey": "SideColor2N",
        "defaultValue": "0xFFFFFF",
        "label": "Left Background Colour",
        "allowGray":true
      },
         {
        "type": "color",
        "messageKey": "HourColorN",
        "defaultValue": "0x000000",
        "label": "Hour Text Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "MinColorN",
        "defaultValue": "0x000000",
        "label": "Minute Text Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "Text1ColorN",
        "defaultValue": "0x000000",
        "label": "Steps Colour",
        "allowGray":true,
        "capabilities":["HEALTH"]
      },
         {
           "type": "color",
           "messageKey": "Text3ColorN",
           "defaultValue": "0x000000",
           "label": "Day & Battery Colour",
           "allowGray":true
         },
         {
        "type": "color",
        "messageKey": "Text6ColorN",
        "defaultValue": "0x000000",
        "label": "Date Colour",
        "allowGray":true
      },
          {
        "type": "color",
        "messageKey": "Text2ColorN",
        "defaultValue": "0x000000",
        "label": "Sunset Colour",
        "allowGray":true
      },
      {
        "type":"color",
        "messageKey":"Text4ColorN",
        "defaultValue":"0xFFFFFF",
        "label":"Moon Colour",
        "allowGray":true
      },
      {
         "type": "input",
         "messageKey": "Lat",
         "defaultValue": "",
         "label": "Manual Location - Latitude",
         "attributes": {
         "placeholder": "eg: 51.4962"
         }
       },
       {
          "type": "input",
          "messageKey": "Long",
          "defaultValue": "",
          "label": "Manual Location - Longitude",
          "description": "Leave both blank to use GPS location for sunrise & sunset times. You can use <a href =https://www.google.com/maps>Google Maps</a> or <a href =https://www.openstreetmap.org/>OpenStreetMap</a> to find latitude & longitude.",
          "attributes": {
            "placeholder": "eg: -0.0989"
          }
        },
       ]
         }
       ]
      },
      {
          "type": "submit",
          "defaultValue":"SAVE"
          },
          {
          "type": "heading",
          "defaultValue": "version v1.0",
          "size":6
          },
          {
          "type": "heading",
          "defaultValue": "Made in UK",
          "size":6
          }
        ];
