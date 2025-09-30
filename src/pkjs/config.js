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
        "defaultValue": "0xFFFFFF",
        "label": "Hour Text Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "MinColor",
        "defaultValue": "0xFFFFFF",
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
        "label": "Day Colour",
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
        "label": "Sunset/Sunrise Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "Text8Color",
        "defaultValue": "0xFFFFFF",
        "label": "Battery Colour",
        "allowGray":true
      },
      {
        "type":"color",
        "messageKey":"Text4Color",
        "defaultValue":"0xFFFFFF",
        "label":"Moon Colour",
        "allowGray":true
      },
      {
    "type": "color",
    "messageKey": "Text5Color",
    "defaultValue": "0xFFFFFF",
    "label": "Weather Icon Colour",
    "allowGray":true
  },
  {
"type": "color",
"messageKey": "Text7Color",
"defaultValue": "0xFFFFFF",
"label": "Temperature Colour",
"allowGray":true
},
    ]
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
           "label": "Day Colour",
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
        "label": "Sunset/Sunrise Colour",
        "allowGray":true
      },
      {
        "type": "color",
        "messageKey": "Text8ColorN",
        "defaultValue": "0x000000",
        "label": "Battery Colour",
        "allowGray":true
      },
      {
        "type":"color",
        "messageKey":"Text4ColorN",
        "defaultValue":"0x000000",
        "label":"Moon Colour",
        "allowGray":true
      },
          {
        "type": "color",
        "messageKey": "Text5ColorN",
        "defaultValue": "0x000000",
        "label": "Weather Icon Colour",
        "allowGray":true
      },
      {
    "type": "color",
    "messageKey": "Text7ColorN",
    "defaultValue": "0x000000",
    "label": "Temperature Colour",
    "allowGray":true
  },
     ]
   },
   {
      "type": "section",
      "items": [
            {
              "type": "heading",
              "defaultValue": "Weather settings",
              "description": "Shake or tap to change view between Sunset/Sunrise, Current Weather and Forecast Weather",
            },
            {
              "type": "toggle",
              "messageKey": "SunsetOn",
              "label": "Turn Sunset/Sunrise & Moonphase On",
              "defaultValue": true,
            },
            {
              "type": "toggle",
              "messageKey": "WeatherOn",
              "label": "Turn Current Weather On",
              "defaultValue": false,
            },
            {
              "type": "toggle",
              "messageKey": "ForecastWeatherOn",
              "label": "Turn Forecast Weather On",
              "defaultValue": false,
            },
            {
              "type": "toggle",
              "messageKey": "WeatherUnit",
              "label": "Temperature in Fahrenheit",
              "defaultValue": false,
            },
            /*  {
              "type": "select",
              "messageKey": "WindUnit",
              "label": "Wind speed",
              "defaultValue": "knots",
                 "options": [
                {
                  "label": "knots",
                  "value": "kts"
                },
                {
                  "label": "miles per hour",
                  "value": "mph"
                },
                   {
                  "label": "metres per second",
                  "value": "ms"
                },
                {
                  "label": "kilometres per hour",
                  "value": "kph"
                }
              ]
            },*/
            {
              "type": "select",
              "messageKey": "WeatherProv",
              "defaultValue": "ds",
              "label": "Weather Provider",
              "options": [
                {
                  "label": "Open-Meteo",
                  "value": "ds"
                },
                {
                  "label": "OpenWeatherMap",
                  "value": "owm"
                }
              ]
            },
            {
               "type": "input",
               "messageKey": "Lat",
               "defaultValue": "",
               "label": "Manual Location - Latitude",
               "attributes": {
               "placeholder": "eg: 51.4962 (leave blank to use GPS)"
               }
             },
             {
                "type": "input",
                "messageKey": "Long",
                "defaultValue": "",
                "label": "Manual Location - Longitude",
                "description": "Leave both blank to use GPS location for sunrise & sunset times and weather. You can use <a href =https://www.google.com/maps>Google Maps</a> or <a href =https://www.openstreetmap.org/>OpenStreetMap</a> to find latitude & longitude.",
                "attributes": {
                  "placeholder": "eg: -0.0989 (leave blank to use GPS)"
                }
              },
              {
                 "type": "input",
                 "messageKey": "APIKEY_User",
                 "defaultValue": "",
                 "label": "API Key",
                 "description": "Weather data uses Open-Meteo by default which does not require an API key.  If you prefer OpenWeatherMap, you can <a href =https://home.openweathermap.org/users/sign_up/>register for a free personal API key here</a>.",
                 "attributes": {
                   "placeholder": "Paste OpenWeatherMap API Key, leave blank for Open-Meteo"
                 }
               },
            {
              "type": "slider",
              "messageKey": "UpSlider",
              "defaultValue": 30,
              "label": "Update frequency (minutes)",
              "description": "More frequent requests will drain your phone battery more quickly",
              "min": 15,
              "max": 120,
              "step": 15},
          ]
     },
    {
    "type": "submit",
    "defaultValue":"SAVE"
    },
    {
    "type": "heading",
    "defaultValue": "version v3.6",
    "size":6
    },
    {
    "type": "heading",
    "defaultValue": "Made in UK",
    "size":6
    }
  ];
