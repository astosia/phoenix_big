#pragma once
#include <pebble.h>
#define SETTINGS_KEY 1
// A structure containing our settings
typedef struct ClaySettings {
 // GColor Back1Color;
//  GColor Back2Color;
  GColor FrameColor;
   GColor FrameColor1;
   GColor FrameColor2;
   GColor SideColor1;
   GColor SideColor2;
  GColor Text1Color;
  GColor Text2Color;
  GColor Text3Color;
  GColor Text4Color;
  GColor Text5Color;
   GColor Text6Color;
  /////////////////
  GColor HourColor;
  GColor MinColor;
  GColor HourColorN;
  GColor MinColorN;
  ////////////////
 // GColor Back1ColorN;
 // GColor Back2ColorN;
  GColor FrameColorN;
    GColor FrameColor1N;
    GColor FrameColor2N;
     GColor SideColor1N;
   GColor SideColor2N;
  GColor Text1ColorN;
  GColor Text2ColorN; 
    GColor Text3ColorN;
  GColor Text4ColorN;
   GColor Text5ColorN;
     GColor Text6ColorN;
  int WeatherUnit;
  char* WindUnit;
  int UpSlider;
  char* WeatherTemp;
  char* TempFore;
  bool NightTheme;
 // bool Rotate;
 // bool RightLeft;
} __attribute__((__packed__)) ClaySettings;