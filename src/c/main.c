#include <pebble.h>
#include "main.h"
#include "weekday.h"
#include "effect_layer.h"
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/fpath.h>
#include <pebble-fctx/ffont.h>

#define ROUND_VERTICAL_PADDING 15
#define ROTATION_SETTING_DEFAULT 0
#define ROTATION_SETTING_LEFT    0
#define ROTATION_SETTING_RIGHT   1

static uint32_t s_last_tap_time = 0;
#define TAP_COOLDOWN_MS 1000  // 1 second cooldown

//Static and initial vars
static GFont 
  FontDaySunsetSteps, FontDateNumber, FontBattery, FontIcon, FontIcon2, FontWeatherIcons, FontFore; 


FFont* time_font;

static Window * s_window;

static Layer * s_canvas_background;
static Layer * s_canvas;
static Layer * s_canvas_sunset_icon;
static Layer * s_canvas_bt_icon;
static Layer * s_canvas_qt_icon;
static TextLayer *s_step_layer;
Layer * time_area_layer;


static int s_hours, s_minutes, s_weekday, s_day, s_loop;

static char* moon_phase[] ={
  "\U0000F095",//'wi-moon-new':0,
  "\U0000F096",//'wi-moon-waxing-crescent-1',1,
  "\U0000F097",//'wi-moon-waxing-crescent-2',2,
  "\U0000F098",//'wi-moon-waxing-crescent-3',3,
  "\U0000F099",//'wi-moon-waxing-crescent-4',4,
  "\U0000F09A",//'wi-moon-waxing-crescent-5',5,
  "\U0000F09B",//'wi-moon-waxing-crescent-6',6,
  "\U0000F09C",//'wi-moon-first-quarter',7,
  "\U0000F09D",//'wi-moon-waxing-gibbous-1',8,
  "\U0000F09E",//'wi-moon-waxing-gibbous-2',9,
  "\U0000F09F",//'wi-moon-waxing-gibbous-3',10,
  "\U0000F0A0",//'wi-moon-waxing-gibbous-4',11,
  "\U0000F0A1",//'wi-moon-waxing-gibbous-5',12,
  "\U0000F0A2",//'wi-moon-waxing-gibbous-6',13,
  "\U0000F0A3",//'wi-moon-full',14,
  "\U0000F0A4",//'wi-moon-waning-gibbous-1',15,
  "\U0000F0A5",//'wi-moon-waning-gibbous-2',16,
  "\U0000F0A6",//'wi-moon-waning-gibbous-3',17,
  "\U0000F0A7",//'wi-moon-waning-gibbous-4',18,
  "\U0000F0A8",//'wi-moon-waning-gibbous-5',19,
  "\U0000F0A9",//'wi-moon-waning-gibbous-6',20,
  "\U0000F0AA",//'wi-moon-third-quarter',21,
  "\U0000F0AB",//'wi-moon-waning-crescent-1',22,
  "\U0000F0AC",//'wi-moon-waning-crescent-2',23,
  "\U0000F0AD",//'wi-moon-waning-crescent-3',24,
  "\U0000F0AE",//'wi-moon-waning-crescent-4',25,
  "\U0000F0AF",//'wi-moon-waning-crescent-5',26,
  "\U0000F0B0",//'wi-moon-waning-crescent-6',27,
  "\U0000F095",//'wi-moon-new',28,
};

static char* weather_conditions[] = {
  "\U0000F07B", // 'unknown': 0,
  "\U0000f00e", //thunderstorm with light rain: 1
  "\U0000f02c", //thunderstorm with light rain: 2
  "\U0000f010", //thunderstorm with rain: 3
  "\U0000f02d", //thunderstorm with rain: 4
  "\U0000f01e", //thunderstorm with heavy rain: 5
  "\U0000f01e", //thunderstorm with heavy rain: 6
  "\U0000f005", //light thunderstorm: 7
  "\U0000f025", //light thunderstorm: 8
  "\U0000f01e", //thunderstorm: 9
  "\U0000f01e", //thunderstorm: 10
  "\U0000f01e", //heavy thunderstorm: 11
  "\U0000f01e", //heavy thunderstorm: 12
  "\U0000f01e", //ragged thunderstorm: 13
  "\U0000f01e", //ragged thunderstorm: 14
  "\U0000f00e", //thunderstorm with light drizzle: 15
  "\U0000f02c", //thunderstorm with light drizzle: 16
  "\U0000f00e", //thunderstorm with drizzle: 17
  "\U0000f02c", //thunderstorm with drizzle: 18
  "\U0000f01d", //thunderstorm with heavy drizzle: 19
  "\U0000f01d", //thunderstorm with heavy drizzle: 20
  "\U0000f00b", //light intensity drizzle: 21
  "\U0000f02b", //light intensity drizzle: 22
  "\U0000f01c", //drizzle: 23
  "\U0000f01c", //drizzle: 24
  "\U0000f01a", //heavy intensity drizzle: 25
  "\U0000f01a", //heavy intensity drizzle: 26
  "\U0000f00b", //light intensity drizzle rain: 27
  "\U0000f02b", //light intensity drizzle rain: 28
  "\U0000f00b", //drizzle rain: 29
  "\U0000f029", //drizzle rain: 30
  "\U0000f019", //heavy intensity drizzle rain: 31
  "\U0000f019", //heavy intensity drizzle rain: 32
  "\U0000f01a", //shower rain and drizzle: 33
  "\U0000f01a", //shower rain and drizzle: 34
  "\U0000f01a", //heavy shower rain and drizzle: 35
  "\U0000f01a", //heavy shower rain and drizzle: 36
  "\U0000f00b", //shower drizzle: 37
  "\U0000f02b", //shower drizzle: 38
  "\U0000f01a", //light rain: 39
  "\U0000f01a", //light rain: 40
  "\U0000f019", //moderate rain: 41
  "\U0000f019", //moderate rain: 42
  "\U0000f019", //heavy intensity rain: 43
  "\U0000f019 ", //heavy intensity rain: 44
  "\U0000f019", //very heavy rain: 45
  "\U0000f019", //very heavy rain: 46
  "\U0000f018", //extreme rain: 47
  "\U0000f018", //extreme rain: 48
  "\U0000f017", //freezing rain: 49
  "\U0000f017", //freezing rain: 50
  "\U0000f01a", //light intensity shower rain: 51
  "\U0000f01a", //light intensity shower rain: 52
  "\U0000f01a", //shower rain: 53
  "\U0000f01a", //shower rain: 54
  "\U0000f01a", //heavy intensity shower rain: 55
  "\U0000f01a", //heavy intensity shower rain: 56
  "\U0000f018", //ragged shower rain: 57
  "\U0000f018", //ragged shower rain: 58
  "\U0000f00a", //light snow: 59
  "\U0000f02a", //light snow: 60
  "\U0000f01b", //Snow: 61
  "\U0000f01b", //Snow: 62
  "\U0000f076", //Heavy snow: 63
  "\U0000f076", //Heavy snow: 64
  "\U0000f017", //Sleet: 65
  "\U0000f017", //Sleet: 66
  "\U0000f0b2", //Light shower sleet: 67
  "\U0000f0b4", //Light shower sleet: 68
  "\U0000f0b5", //Shower sleet: 69
  "\U0000f0b5", //Shower sleet: 70
  "\U0000f006", //Light rain and snow: 71
  "\U0000f026", //Light rain and snow: 72
  "\U0000f017", //Rain and snow: 73
  "\U0000f017", //Rain and snow: 74
  "\U0000f00a", //Light shower snow: 75
  "\U0000f02a", //Light shower snow: 76
  "\U0000f00a", //Shower snow: 77
  "\U0000f02a", //Shower snow: 78
  "\U0000f076", //Heavy shower snow: 79
  "\U0000f076", //Heavy shower snow: 80
  "\U0000f003", //mist: 81
  "\U0000f04a", //mist: 82
  "\U0000f062", //Smoke: 83
  "\U0000f062", //Smoke: 84
  "\U0000f0b6", //Haze: 85
  "\U0000f023", //Haze: 86
  "\U0000f082", //sand/ dust whirls: 87
  "\U0000f082", //sand/ dust whirls: 88
  "\U0000f014", //fog: 89
  "\U0000f014", //fog: 90
  "\U0000f082", //sand: 91
  "\U0000f082", //sand: 92
  "\U0000f082", //dust: 93
  "\U0000f082", //dust: 94
  "\U0000f0c8", //volcanic ash: 95
  "\U0000f0c8", //volcanic ash: 96
  "\U0000f011", //squalls: 97
  "\U0000f011", //squalls: 98
  "\U0000f056", //tornado: 99
  "\U0000f056", //tornado: 100
  "\U0000f00d", //clear sky: 101
  "\U0000f02e", //clear sky: 102
  "\U0000f00c", //few clouds: 11-25%: 103
  "\U0000f081", //few clouds: 11-25%: 104
  "\U0000f002", //scattered clouds: 25-50%: 105
  "\U0000f086", //scattered clouds: 25-50%: 106
  "\U0000f041", //broken clouds: 51-84%: 107
  "\U0000f041", //broken clouds: 51-84%: 108
  "\U0000f013", //overcast clouds: 85-100%: 109
  "\U0000f013", //overcast clouds: 85-100%: 110
  "\U0000f056", //tornado: 111
  "\U0000f01d", //storm-showers: 112
  "\U0000f073", //hurricane: 113
  "\U0000f076", //snowflake-cold: 114
  "\U0000f072", //hot: 115
  "\U0000f050", //windy: 116
  "\U0000f015", //hail: 117
  "\U0000f050", //strong-wind: 118
};
//////Init Configuration///
//Init Clay
ClaySettings settings;
// Initialize the default settings
static void prv_default_settings(){
  settings.FrameColor1 = GColorBlack;
  settings.SideColor1 = GColorBlack;
  settings.SideColor2 = GColorWhite;
  settings.Text1Color = GColorWhite;
  settings.Text2Color = GColorWhite;
  settings.Text3Color = GColorWhite;
  settings.Text4Color = GColorWhite;
  settings.Text5Color = GColorWhite;
  settings.Text6Color = GColorWhite;
  settings.Text7Color = GColorWhite;
  settings.Text8Color = GColorWhite;
  settings.HourColor = GColorBlack;
  settings.MinColor = GColorWhite;
  settings.HourColorN = GColorBlack;
  settings.MinColorN = GColorBlack;
  settings.FrameColor1N = GColorWhite;
  settings.SideColor1N = GColorWhite;
  settings.SideColor2N = GColorWhite;
  settings.Text1ColorN = GColorBlack;
  settings.Text2ColorN = GColorBlack;
  settings.Text3ColorN = GColorBlack;
  settings.Text4ColorN = GColorBlack;
  settings.Text5ColorN = GColorBlack;
  settings.Text6ColorN = GColorBlack;
  settings.Text7ColorN = GColorBlack;
  settings.Text8ColorN = GColorBlack;
  settings.UpSlider = 30;
  settings.NightTheme = false;
  settings.HealthOff = true;
  settings.AddZero12h = false;
  settings.RemoveZero24h = false;
  settings.WeatherOn = false;
  settings.ForecastWeatherOn = false;
  settings.SunsetOn = true;
}
int HourSunrise=700;
int HourSunset=2200;
int moonphase=0;
bool BTOn=true;
bool GPSOn=true;
bool IsNightNow=false;
int s_countdown = 0;
int showWeather = 0;

//////End Configuration///
///////////////////////////

static GColor ColorSelect(GColor ColorDay, GColor ColorNight){
  if (settings.NightTheme && IsNightNow && GPSOn ){
    return ColorNight;
  }
  else{
    return ColorDay;
  }
}
// Callback for js request
void request_watchjs(){
  //Starting loop at 0
  s_loop = 0;
  // Begin dictionary
  DictionaryIterator * iter;
  app_message_outbox_begin( & iter);
  // Add a key-value pair
  dict_write_uint8(iter, 0, 0);
  // Send the message!
  app_message_outbox_send();
}

// static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
//   // A tap event occured
//   int options_count = settings.WeatherOn + settings.ForecastWeatherOn + settings.SunsetOn;

//   if(options_count < 2 ){
//       return;
//   }

//   else if(settings.WeatherOn && settings.ForecastWeatherOn && settings.SunsetOn){
//       //switch between all three options
//         if (showWeather == 2 || showWeather > 2){
//           //Reset
//           showWeather = 0;
//         } else{
//           showWeather = showWeather + 1;
//         }
//         layer_mark_dirty (s_canvas);
//         }

//   else if(settings.WeatherOn && settings.SunsetOn){
//       //switch between sunset (0) and current weather (1)
//         if (showWeather == 1 || showWeather > 1){
//           //Reset
//           showWeather = 0;
//         } else{
//           showWeather = showWeather + 1;
//         }
//         layer_mark_dirty (s_canvas);
//         }

//   else if(settings.ForecastWeatherOn && settings.SunsetOn){
//       //switch between sunset (0) and forecast weather (2)
//         if (showWeather == 2 || showWeather > 2){
//           //Reset
//           showWeather = 0;
//         } else{
//           showWeather = showWeather + 2;
//         }
//         layer_mark_dirty (s_canvas);
//         }

//   else if(settings.WeatherOn && settings.ForecastWeatherOn){
//       //switch between current weather (1) and forecast weather (2)
//         if (showWeather == 2 || showWeather > 2){
//           //Reset
//           showWeather = 1;
//         } else{
//           showWeather = showWeather + 1;
//         }
//         layer_mark_dirty (s_canvas);
//         }
//         //  showWeather = !showWeather;

//   APP_LOG(APP_LOG_LEVEL_DEBUG, "showweather is %d", showWeather);

// }

//////////changed function to avoid false/rebound taps on Pebble Time 2 alpha unit
static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  
  time_t seconds;
  uint16_t milliseconds;
  time_ms(&seconds, &milliseconds);
  
  uint32_t now = (uint32_t)seconds * TAP_COOLDOWN_MS + milliseconds;

  // Check cooldown
  if (s_last_tap_time != 0 && (now - s_last_tap_time < TAP_COOLDOWN_MS)) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Tap ignored");
    return;
  }
  
  s_last_tap_time = now;

  int options_count = settings.WeatherOn + settings.ForecastWeatherOn + settings.SunsetOn;
  if(options_count < 2) return;

  if(settings.WeatherOn && settings.ForecastWeatherOn && settings.SunsetOn){
      showWeather = (showWeather >= 2) ? 0 : showWeather + 1;
  }
  else if(settings.WeatherOn && settings.SunsetOn){
      showWeather = (showWeather >= 1) ? 0 : showWeather + 1;
  }
  else if(settings.ForecastWeatherOn && settings.SunsetOn){
      showWeather = (showWeather >= 2) ? 0 : showWeather + 2;
  }
  else if(settings.WeatherOn && settings.ForecastWeatherOn){
      showWeather = (showWeather >= 2) ? 1 : showWeather + 1;
  }

  layer_mark_dirty(s_canvas);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Valid Tap! showWeather: %d", (int)showWeather);
}

///BT Connection
static void bluetooth_callback(bool connected){
  BTOn = connected;

}

static void bluetooth_vibe_icon (bool connected) {

  layer_set_hidden(s_canvas_bt_icon, connected);
//  layer_set_hidden(s_canvas_sunset_icon, !connected);

  if(!connected && !quiet_time_is_active()) {
    // Issue a vibrating alert
    vibes_double_pulse();
  }
}

static void quiet_time_icon () {
  if(!quiet_time_is_active()) {
  layer_set_hidden(s_canvas_qt_icon,true);
  }


}

static void onreconnection(bool before, bool now){
  if (!before && now){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "BT reconnected, requesting weather at %d", s_minutes);
    request_watchjs();
  }
}


//Add in HEALTH to the watchface
static char s_current_steps_buffer[16];
static int s_step_count = 0; //, s_step_goal = 0; //, s_step_average = 0;

// Is step data available?
bool step_data_is_available() {
  return HealthServiceAccessibilityMaskAvailable &
    health_service_metric_accessible(HealthMetricStepCount,
      time_start_of_today(), time(NULL));
}

// Todays current step count
static void get_step_count() {
    s_step_count = (int)health_service_sum_today(HealthMetricStepCount);
}

static void display_step_count() {
  int thousands = s_step_count / 1000;
  int hundreds = (s_step_count % 1000)/100;
  int hundreds2 = (s_step_count % 1000);
//  static char s_emoji[5];

text_layer_set_text_color(s_step_layer, ColorSelect(settings.Text1Color, settings.Text1ColorN));
 
  if(thousands > 0) {
    PBL_IF_ROUND_ELSE(
    
    snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),
     "%d.%d%s", thousands, hundreds, "k"),
      snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),
      "%d.%d%s", thousands, hundreds, "k"/*s_emoji*/));
  } else {
    snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),
      "%d"/* %s"*/, hundreds2/*, s_emoji*/);
  }
  text_layer_set_text(s_step_layer, s_current_steps_buffer);
 
}



static void health_handler(HealthEventType event, void *context) {
  
   if(event != HealthEventSleepUpdate) {
     get_step_count();
     display_step_count();
   }
}



void layer_update_proc_background (Layer * back_layer, GContext * ctx){
  // Create Rects
  GRect bounds = layer_get_bounds(back_layer);
  GRect MediumBand =
    PBL_IF_ROUND_ELSE(
     GRect(0, 0, 38, bounds.size.h),
     GRect(0, 0, 0, 0));

 GRect SideBand1 =
    PBL_IF_ROUND_ELSE(
     GRect(100, 0, 80, bounds.size.h ),
      GRect(bounds.size.w/2, 0, bounds.size.w/2, bounds.size.h));

 GRect SideBand2 =
    PBL_IF_ROUND_ELSE(
     GRect(38, 0, 100-38, bounds.size.h ),
      GRect(0, 0, bounds.size.w/2, bounds.size.h));

      graphics_context_set_fill_color(ctx, ColorSelect(settings.FrameColor1, settings.FrameColor1N));
      graphics_fill_rect(ctx, MediumBand,0,GCornersAll);
      graphics_context_set_fill_color(ctx, ColorSelect(settings.SideColor1, settings.SideColor1N));
      graphics_fill_rect(ctx, SideBand1,0,GCornersAll);
      graphics_context_set_fill_color(ctx, ColorSelect(settings.SideColor2, settings.SideColor2N));
      graphics_fill_rect(ctx, SideBand2,0,GCornersAll);
}

void update_time_area_layer(Layer *l, GContext* ctx) {
  // 1. Get bounds
  
#if defined(PBL_ROUND)
    //Round: offset slightly to look better in the circle
  GRect bounds = layer_get_unobstructed_bounds(l);
  GRect hour_bounds = GRect(0, 0, bounds.size.w / 3 * 2, bounds.size.h);
#else
  GRect bounds = layer_get_unobstructed_bounds(l);
  GRect boundsobs = layer_get_bounds(l);
#endif

  //GRect minute_bounds = GRect(bounds.size.w/2,0,bounds.size.w/2, bounds.size.h);

  // 2. Initialize FCTX (Only once for both hour and minute)
  FContext fctx;
  fctx_init_context(&fctx, ctx);
  fctx_set_color_bias(&fctx, 0);
  #if defined(PBL_COLOR)
    fctx_enable_aa(true);
  #endif

  // 3. Get Time Data
  time_t temp = time(NULL);
  struct tm *time_now = localtime(&temp);
  char hourdraw[8];
  char mindraw[8];
  
  // Format Hours
  if(clock_is_24h_style() && settings.RemoveZero24h){
      strftime(hourdraw, sizeof(hourdraw),"%k",time_now);
  } else if (clock_is_24h_style() && !settings.RemoveZero24h) {
      strftime(hourdraw, sizeof(hourdraw),"%H",time_now);
  } else if (settings.AddZero12h) {
    strftime(hourdraw, sizeof(hourdraw),"%I",time_now);
  } else {
    strftime(hourdraw, sizeof(hourdraw),"%l",time_now);
  }
  
  // Format Minutes
  strftime(mindraw, sizeof(mindraw), "%M", time_now);

  // ==========================================
  // DRAW HOURS
  // ==========================================
  fctx_begin_fill(&fctx);
  fctx_set_fill_color(&fctx, ColorSelect(settings.HourColor, settings.HourColorN));
  
  #ifdef PBL_ROUND
    int font_size_h = 168;
  #else
    int font_size_h = boundsobs.size.h;
  #endif
  
  fctx_set_text_em_height(&fctx, time_font, font_size_h);
  
  FPoint hour_pos;
  #ifdef PBL_PLATFORM_EMERY
    hour_pos.x = INT_TO_FIXED((bounds.size.w / 4) + 1);
    hour_pos.y = INT_TO_FIXED(1);
  #else
    hour_pos.x = INT_TO_FIXED(PBL_IF_ROUND_ELSE(hour_bounds.size.w / 2 + 8, bounds.size.w / 4 + 1));
    hour_pos.y = INT_TO_FIXED(PBL_IF_ROUND_ELSE(hour_bounds.size.h / 2 + 1, 1));
  #endif

  int h_width = fctx_string_width(&fctx, hourdraw, time_font);
  #if defined(PBL_PLATFORM_EMERY)
    fctx_set_scale(&fctx, FPoint(h_width * 0.7, font_size_h), 
                           FPoint(200 * 16, -font_size_h * 3.6 * bounds.size.h / boundsobs.size.h));
  #elif defined (PBL_PLATFORM_CHALK)
    //int h_ref_width = 144;

    //fctx_set_scale(&fctx, FPoint(h_width, font_size_h), 
    //                      FPoint(h_ref_width * 16, -font_size_h * 2.25));

    //fctx_set_scale(&fctx,FPoint(h_width,font_size_h),FPoint(144*16,-font_size_h*2.25));

    int hour_len = strlen(hourdraw);

    if (hour_len == 1) {
      // DYNAMIC SCALE FOR SINGLE DIGIT: 
      // We increase the width relative to height (e.g., 1.5x wider)
      // and adjust the reference width (100*16) to keep it centered properly
      fctx_set_scale(&fctx, FPoint(h_width * 0.75, font_size_h), FPoint(144 * 16, -font_size_h * 2.25));
    } else {
      // STANDARD SCALE FOR DOUBLE DIGITS (10, 11, 12 or 00-24)
      fctx_set_scale(&fctx, FPoint(h_width, font_size_h), FPoint(144 * 16, -font_size_h * 2.25));
    }
 
  #else
    int h_ref_width = 144;
    fctx_set_scale(&fctx, FPoint (h_width, font_size_h), 
                          FPoint(h_ref_width * 16, -font_size_h * 2.65 * bounds.size.h / boundsobs.size.h));
  #endif

  fctx_set_offset(&fctx, hour_pos);
  fctx_draw_string(&fctx, hourdraw, time_font, GTextAlignmentCenter, PBL_IF_ROUND_ELSE(FTextAnchorMiddle, FTextAnchorTop));
  fctx_end_fill(&fctx);

  // ==========================================
  // DRAW MINUTES
  // ==========================================
  fctx_begin_fill(&fctx);
  fctx_set_fill_color(&fctx, ColorSelect(settings.MinColor, settings.MinColorN));

  #ifdef PBL_ROUND
    int font_size_m = bounds.size.h * 0.55;
  #elif defined(PBL_PLATFORM_EMERY)
    int font_size_m = 148;
  #else
    int font_size_m = 109;
  #endif


  
  // Reset scale to default before repositioning or apply specific minute scaling
  fctx_set_scale(&fctx, FPoint(INT_TO_FIXED(1), INT_TO_FIXED(1)), FPoint(INT_TO_FIXED(1), INT_TO_FIXED(1)));

  fctx_set_text_em_height(&fctx, time_font, font_size_m);

  FPoint min_pos;
  #ifdef PBL_PLATFORM_EMERY
    min_pos.x = INT_TO_FIXED(150);
    min_pos.y = INT_TO_FIXED(boundsobs.size.h / 4 + 10);
  #else
    min_pos.x = INT_TO_FIXED(PBL_IF_ROUND_ELSE(135, 108));
    min_pos.y = INT_TO_FIXED(PBL_IF_ROUND_ELSE(bounds.size.h / 2, boundsobs.size.h / 4 + 6));
  #endif

  fctx_set_offset(&fctx, min_pos);
  fctx_draw_string(&fctx, mindraw, time_font, GTextAlignmentCenter, FTextAnchorMiddle);
  fctx_end_fill(&fctx);

  // 4. Deinit (Frees memory only once)
  fctx_deinit_context(&fctx);
}

static void layer_update_proc(Layer * layer, GContext * ctx){

#if defined(PBL_PLATFORM_EMERY)
   GRect DateRect  = GRect(100, 128, 50, 22);
   GRect DateRect2 = GRect(100, 148, 48, 54);
   GRect BatteryRect = GRect(100, 205, 50, 27);

#else

 GRect DateRect =
  
   (PBL_IF_ROUND_ELSE(
      GRect(22-22+6, 26+40+2, 32, 40),
      GRect(0+108-19-18, 0+94, 38, 16)));

  GRect DateRect2 =
    (PBL_IF_ROUND_ELSE(
      GRect(22-22+6, 16+22+40+2, 32, 40),
      GRect(0+108-19-18, 16+90, 38, 40)));

  GRect BatteryRect =
   (PBL_IF_ROUND_ELSE(
      GRect(100,136+12,50,20),
      GRect(0+108-19-18,150, 38, 20)));

#endif

  //Date
  // Local language
  const char * sys_locale = i18n_get_system_locale();
  char datedraw[10];
  fetchwday(s_weekday, sys_locale, datedraw);
  char datenow[10];
  snprintf(datenow, sizeof(datenow), "%s", datedraw);


  int daydraw;
  daydraw = s_day;
  char daynow[8];
  snprintf(daynow, sizeof(daynow), "%02d", daydraw);

  //Battery
  int battery_level = battery_state_service_peek().charge_percent;
  char battperc[20];
  snprintf(battperc, sizeof(battperc), "%d", battery_level);
  strcat(battperc, "%");

  graphics_context_set_text_color(ctx, ColorSelect(settings.Text3Color, settings.Text3ColorN));
  graphics_draw_text(ctx, datenow, FontDaySunsetSteps, DateRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
  graphics_context_set_text_color(ctx, ColorSelect(settings.Text8Color, settings.Text8ColorN));
  graphics_draw_text(ctx, battperc, FontBattery, BatteryRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);

  graphics_context_set_text_color(ctx, ColorSelect(settings.Text6Color, settings.Text6ColorN));
  graphics_draw_text(ctx, daynow, PBL_IF_ROUND_ELSE(FontDateNumber,FontDateNumber), DateRect2, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
}


static void layer_update_proc_sunset(Layer * layer2, GContext * ctx2){

#if defined(PBL_PLATFORM_EMERY)
    GRect SunsetIconRect = GRect(150, 180, 52, 28);
    GRect MoonRect = GRect(150, 148, 52, 54);
    GRect SunsetRect = GRect(136, 199, 78, 28);
    GRect TempRect   = GRect(136, 199, 78, 28);
    GRect ForeRect   = GRect(136, 199, 78, 28);
    GRect IconNowRect = GRect(150, 161, 52, 54);

#else

   GRect SunsetIconRect =
    (PBL_IF_ROUND_ELSE(
      GRect(102,136,75,20),
      GRect(0+108-19+18,16+96+22-1,38,20)));

  GRect MoonRect =
         (PBL_IF_ROUND_ELSE(
           GRect(28-20,139-100+8,28,20),
           GRect(0+108-19+18,16+93, 38, 40)));

   GRect SunsetRect =
     (PBL_IF_ROUND_ELSE(
      GRect(100,130,75,20),
       GRect(0+108+18-28, 146, 54,20)));

   GRect TempRect =  //temperature number
        (PBL_IF_ROUND_ELSE(
               GRect(8,106,32,40),
           GRect(0+108+18-28, 146, 54,20)));

           GRect ForeRect =  //temperature number
                (PBL_IF_ROUND_ELSE(
                   GRect(8,106+2,32,40),
                   GRect(0+108+18-28, 146, 54,20)));

    GRect IconNowRect = //weather condition icon
         (PBL_IF_ROUND_ELSE(
           GRect(28-20,139-100+6,28,20),
           GRect(0+108-19+18,16+93+10, 38, 40)));
#endif

  char MoonToDraw[20];
snprintf(MoonToDraw, sizeof(MoonToDraw), "%s",settings.moonstring);

 char SunsetIconToDraw[20];
  char SunsetIconToShow[20];
  int nowthehouris = s_hours * 100 + s_minutes;
   if (HourSunrise <= nowthehouris && nowthehouris <= HourSunset){
      snprintf(SunsetIconToShow, sizeof(SunsetIconToShow),  "%s", "\U0000F052");
  }
  else{
      snprintf(SunsetIconToShow, sizeof(SunsetIconToShow),  "%s",  "\U0000F051");
  }


if (strcmp(settings.sunsetstring, "") == 0){
    snprintf(SunsetIconToDraw, sizeof(SunsetIconToDraw),  "%s", "\U0000F04C");
  }
  else {
    snprintf(SunsetIconToDraw, sizeof(SunsetIconToDraw), "%s",SunsetIconToShow/*"\U0000F052"*/);
  }

char SunsetToDraw[20];

if (clock_is_24h_style()){
if (HourSunrise <= nowthehouris && nowthehouris <= HourSunset){
snprintf(SunsetToDraw, sizeof(SunsetToDraw), "%s",settings.sunsetstring);
}
else{
snprintf(SunsetToDraw, sizeof(SunsetToDraw), "%s",settings.sunrisestring);
}
} else {
if (HourSunrise <= nowthehouris && nowthehouris <= HourSunset){
snprintf(SunsetToDraw, sizeof(SunsetToDraw), "%s",settings.sunsetstring12);
}
else{
snprintf(SunsetToDraw, sizeof(SunsetToDraw), "%s",settings.sunrisestring12);
}
}
char CondToDraw[20];
char TempToDraw[20];
char ForeToDraw[20];
char HiLowToDraw[20];

snprintf(CondToDraw, sizeof(CondToDraw), "%s",settings.iconnowstring);
snprintf(TempToDraw, sizeof(TempToDraw), "%s",settings.tempstring);
snprintf(ForeToDraw, sizeof(ForeToDraw), "%s",settings.iconforestring);
snprintf(HiLowToDraw, sizeof(HiLowToDraw), "%s",settings.temphistring);

if (!settings.WeatherOn && !settings.ForecastWeatherOn && !settings.SunsetOn) {
  //draw nothing as no options selected

} else {

if (showWeather==0) //show moonphase and sunset/sunrise
{
 graphics_context_set_text_color(ctx2, ColorSelect(settings.Text2Color, settings.Text2ColorN));
 graphics_draw_text(ctx2, SunsetIconToDraw, FontWeatherIcons, SunsetIconRect, GTextOverflowModeFill,PBL_IF_ROUND_ELSE(GTextAlignmentLeft,GTextAlignmentCenter), NULL);
 graphics_context_set_text_color(ctx2,ColorSelect(settings.Text4Color,settings.Text4ColorN));
 graphics_draw_text(ctx2, MoonToDraw, FontIcon, MoonRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
 graphics_context_set_text_color(ctx2, ColorSelect(settings.Text2Color, settings.Text2ColorN));
 graphics_draw_text(ctx2, SunsetToDraw, FontDaySunsetSteps, SunsetRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
}
else if (showWeather==1) //show current weather
{
  graphics_context_set_text_color(ctx2,ColorSelect(settings.Text7Color,settings.Text7ColorN));
  graphics_draw_text(ctx2, TempToDraw, FontDaySunsetSteps, TempRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
  graphics_context_set_text_color(ctx2,ColorSelect(settings.Text5Color,settings.Text5ColorN));
  graphics_draw_text(ctx2, CondToDraw, FontIcon, IconNowRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
  #ifdef PBL_ROUND
  graphics_context_set_text_color(ctx2, ColorSelect(settings.Text2Color, settings.Text2ColorN));
  graphics_draw_text(ctx2, SunsetIconToDraw, FontWeatherIcons, SunsetIconRect, GTextOverflowModeFill,PBL_IF_ROUND_ELSE(GTextAlignmentLeft,GTextAlignmentCenter), NULL);
  graphics_context_set_text_color(ctx2, ColorSelect(settings.Text2Color, settings.Text2ColorN));
  graphics_draw_text(ctx2, SunsetToDraw, FontDaySunsetSteps, SunsetRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
  #endif
 }
 else if (showWeather==2) //show forecast weather
 {
   graphics_context_set_text_color(ctx2,ColorSelect(settings.Text7Color,settings.Text7ColorN));
   graphics_draw_text(ctx2, HiLowToDraw, FontFore, ForeRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
   graphics_context_set_text_color(ctx2,ColorSelect(settings.Text5Color,settings.Text5ColorN));
   graphics_draw_text(ctx2, ForeToDraw, FontIcon, IconNowRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
   #ifdef PBL_ROUND
   graphics_context_set_text_color(ctx2, ColorSelect(settings.Text2Color, settings.Text2ColorN));
   graphics_draw_text(ctx2, SunsetIconToDraw, FontWeatherIcons, SunsetIconRect, GTextOverflowModeFill,PBL_IF_ROUND_ELSE(GTextAlignmentLeft,GTextAlignmentCenter), NULL);
   graphics_context_set_text_color(ctx2, ColorSelect(settings.Text2Color, settings.Text2ColorN));
   graphics_draw_text(ctx2, SunsetToDraw, FontDaySunsetSteps, SunsetRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
   #endif
  }
}
}

static void layer_update_proc_bt(Layer * layer3, GContext * ctx3){
   // Create Rects

#if defined(PBL_PLATFORM_EMERY)
    GRect BTIconRect = GRect(150, 136, 25, 27);
#else
  GRect BTIconRect =
    (PBL_IF_ROUND_ELSE(
      GRect(100,16,72,20),
      GRect(126-18,100-2,18,20)));

#endif

 onreconnection(BTOn, connection_service_peek_pebble_app_connection());
 bluetooth_callback(connection_service_peek_pebble_app_connection());

 graphics_context_set_text_color(ctx3, ColorSelect(settings.Text3Color, settings.Text3ColorN));
 graphics_draw_text(ctx3, "z", FontIcon2, BTIconRect, GTextOverflowModeFill,GTextAlignmentCenter, NULL);

}

static void layer_update_proc_qt(Layer * layer4, GContext * ctx4){
   // Create Rects

#if defined(PBL_PLATFORM_EMERY)
    GRect QTIconRect = GRect(175, 136, 25, 27);
#else
  GRect QTIconRect =
    (PBL_IF_ROUND_ELSE(
      GRect(8,126,32,20),
      GRect(126,100-2,18,20)));

#endif


 quiet_time_icon();

 graphics_context_set_text_color(ctx4, ColorSelect(settings.Text3Color, settings.Text3ColorN));
 graphics_draw_text(ctx4, "\U0000E061", FontIcon2, QTIconRect, GTextOverflowModeFill,GTextAlignmentCenter, NULL);

}


/////////////////////////////////////////
////Init: Handle Settings and Weather////
/////////////////////////////////////////
// Read settings from persistent storage
static void prv_load_settings(){
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, & settings, sizeof(settings));
}
// Save the settings to persistent storage
static void prv_save_settings(){
  persist_write_data(SETTINGS_KEY, & settings, sizeof(settings));
}
// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator * iter, void * context){
  s_loop = s_loop + 1;
  bool weather_options_changed = false;
  if (s_loop == 1){
  
  }
  Tuple * fr1_color_t = dict_find(iter, MESSAGE_KEY_FrameColor1);
  if (fr1_color_t){
    settings.FrameColor1 = GColorFromHEX(fr1_color_t-> value -> int32);
  }
  Tuple * nfr1_color_t = dict_find(iter, MESSAGE_KEY_FrameColor1N);
  if (nfr1_color_t){
    settings.FrameColor1N = GColorFromHEX(nfr1_color_t-> value -> int32);
  }
  ///////////
   Tuple * sd1_color_t = dict_find(iter, MESSAGE_KEY_SideColor1);
  if (sd1_color_t){
    settings.SideColor1 = GColorFromHEX(sd1_color_t-> value -> int32);
  }
  Tuple * nsd1_color_t = dict_find(iter, MESSAGE_KEY_SideColor1N);
  if (nsd1_color_t){
    settings.SideColor1N = GColorFromHEX(nsd1_color_t-> value -> int32);
  }
   Tuple * sd2_color_t = dict_find(iter, MESSAGE_KEY_SideColor2);
  if (sd2_color_t){
    settings.SideColor2 = GColorFromHEX(sd2_color_t-> value -> int32);
  }
  Tuple * nsd2_color_t = dict_find(iter, MESSAGE_KEY_SideColor2N);
  if (nsd2_color_t){
    settings.SideColor2N = GColorFromHEX(nsd2_color_t-> value -> int32);
  }
  ////////////
  Tuple * tx1_color_t = dict_find(iter, MESSAGE_KEY_Text1Color);
  if (tx1_color_t){
    settings.Text1Color = GColorFromHEX(tx1_color_t-> value -> int32);
  }
  Tuple * ntx1_color_t = dict_find(iter, MESSAGE_KEY_Text1ColorN);
  if (ntx1_color_t){
    settings.Text1ColorN = GColorFromHEX(ntx1_color_t-> value -> int32);
  }
  ///////////////////////////////
  Tuple * hr_color_t = dict_find(iter, MESSAGE_KEY_HourColor);
  if (hr_color_t){
    settings.HourColor = GColorFromHEX(hr_color_t-> value -> int32);
  }
  Tuple * nthr_color_t = dict_find(iter, MESSAGE_KEY_HourColorN);
  if (nthr_color_t){
    settings.HourColorN = GColorFromHEX(nthr_color_t-> value -> int32);
  }
  Tuple * min_color_t = dict_find(iter, MESSAGE_KEY_MinColor);
  if (min_color_t){
    settings.MinColor = GColorFromHEX(min_color_t-> value -> int32);
  }
  Tuple * ntmin_color_t = dict_find(iter, MESSAGE_KEY_MinColorN);
  if (ntmin_color_t){
    settings.MinColorN = GColorFromHEX(ntmin_color_t-> value -> int32);
  }
  ///////////////////////////////
  Tuple * tx2_color_t = dict_find(iter, MESSAGE_KEY_Text2Color);
  if (tx2_color_t){
    settings.Text2Color = GColorFromHEX(tx2_color_t-> value -> int32);
  }
  Tuple * ntx2_color_t = dict_find(iter, MESSAGE_KEY_Text2ColorN);
  if (ntx2_color_t){
    settings.Text2ColorN = GColorFromHEX(ntx2_color_t-> value -> int32);
  }
   Tuple * tx3_color_t = dict_find(iter, MESSAGE_KEY_Text3Color);
  if (tx3_color_t){
    settings.Text3Color = GColorFromHEX(tx3_color_t-> value -> int32);
  }
  Tuple * ntx3_color_t = dict_find(iter, MESSAGE_KEY_Text3ColorN);
  if (ntx3_color_t){
    settings.Text3ColorN = GColorFromHEX(ntx3_color_t-> value -> int32);
  }
  Tuple * tx4_color_t = dict_find(iter,MESSAGE_KEY_Text4Color);
  if (tx4_color_t){
    settings.Text4Color = GColorFromHEX(tx4_color_t-> value -> int32);
    }
  Tuple * ntx4_color_t = dict_find(iter, MESSAGE_KEY_Text4ColorN);
  if(ntx4_color_t){
    settings.Text4ColorN = GColorFromHEX(ntx4_color_t-> value -> int32);
  }
  Tuple * tx5_color_t = dict_find(iter,MESSAGE_KEY_Text5Color);
  if (tx5_color_t){
    settings.Text5Color = GColorFromHEX(tx5_color_t-> value -> int32);
    }
  Tuple * ntx5_color_t = dict_find(iter, MESSAGE_KEY_Text5ColorN);
  if(ntx5_color_t){
    settings.Text5ColorN = GColorFromHEX(ntx5_color_t-> value -> int32);
  }
   Tuple * tx6_color_t = dict_find(iter,MESSAGE_KEY_Text6Color);
  if (tx6_color_t){
    settings.Text6Color = GColorFromHEX(tx6_color_t-> value -> int32);
    }
  Tuple * ntx6_color_t = dict_find(iter, MESSAGE_KEY_Text6ColorN);
  if(ntx6_color_t){
    settings.Text6ColorN = GColorFromHEX(ntx6_color_t-> value -> int32);
  }

  Tuple * tx7_color_t = dict_find(iter,MESSAGE_KEY_Text7Color);
 if (tx7_color_t){
   settings.Text7Color = GColorFromHEX(tx7_color_t-> value -> int32);
   }
 Tuple * ntx7_color_t = dict_find(iter, MESSAGE_KEY_Text7ColorN);
 if(ntx7_color_t){
   settings.Text7ColorN = GColorFromHEX(ntx7_color_t-> value -> int32);
 }

 Tuple * tx8_color_t = dict_find(iter,MESSAGE_KEY_Text8Color);
if (tx8_color_t){
  settings.Text8Color = GColorFromHEX(tx8_color_t-> value -> int32);
  }
Tuple * ntx8_color_t = dict_find(iter, MESSAGE_KEY_Text8ColorN);
if(ntx8_color_t){
  settings.Text8ColorN = GColorFromHEX(ntx8_color_t-> value -> int32);
}
   //Control of data from http
  // Weather Cond
   //Hour Sunrise and Sunset
  Tuple * sunrise_t = dict_find(iter, MESSAGE_KEY_HourSunrise);
  if (sunrise_t){
    HourSunrise = (int) sunrise_t -> value -> int32;
  }
  Tuple * sunset_t = dict_find(iter, MESSAGE_KEY_HourSunset);
  if (sunset_t){
    HourSunset = (int) sunset_t -> value -> int32;
  }
  Tuple * sunset_dt = dict_find(iter, MESSAGE_KEY_WEATHER_SUNSET_KEY);
  if (sunset_dt){
     snprintf(settings.sunsetstring, sizeof(settings.sunsetstring), "%s", sunset_dt -> value -> cstring);
  }
   Tuple * sunrise_dt = dict_find(iter, MESSAGE_KEY_WEATHER_SUNRISE_KEY);
  if (sunrise_dt){
     snprintf(settings.sunrisestring, sizeof(settings.sunrisestring), "%s", sunrise_dt -> value -> cstring);
  }
  Tuple * sunset12_dt = dict_find(iter, MESSAGE_KEY_WEATHER_SUNSET_KEY_12H);
  if (sunset12_dt){
     snprintf(settings.sunsetstring12, sizeof(settings.sunsetstring12), "%s", sunset12_dt -> value -> cstring);
  }
   Tuple * sunrise12_dt = dict_find(iter, MESSAGE_KEY_WEATHER_SUNRISE_KEY_12H);
  if (sunrise12_dt){
     snprintf(settings.sunrisestring12, sizeof(settings.sunrisestring12), "%s", sunrise12_dt -> value -> cstring);
  }
  Tuple * moon_tuple = dict_find(iter, MESSAGE_KEY_MoonPhase);
  if (moon_tuple){
    snprintf(settings.moonstring,sizeof(settings.moonstring),"%s",moon_phase[(int)moon_tuple->value->int32]);
  }
Tuple * wtemp_t = dict_find(iter, MESSAGE_KEY_WeatherTemp);
if (wtemp_t){
snprintf(settings.tempstring, sizeof(settings.tempstring), "%s", wtemp_t -> value -> cstring);
}

Tuple * wforetemp_t = dict_find(iter, MESSAGE_KEY_TempFore);
if (wforetemp_t){
  snprintf(settings.temphistring, sizeof(settings.temphistring), "%s", wforetemp_t -> value -> cstring);
}

Tuple * iconnow_tuple = dict_find(iter, MESSAGE_KEY_IconNow);
  //////////Add in icons and forecast hi/lo temp////////////
  if (iconnow_tuple){
    snprintf(settings.iconnowstring,sizeof(settings.iconnowstring),"%s",weather_conditions[(int)iconnow_tuple->value->int32]);
}

Tuple * iconfore_tuple = dict_find(iter, MESSAGE_KEY_IconFore);
if (iconfore_tuple){
  snprintf(settings.iconforestring,sizeof(settings.iconforestring),"%s",weather_conditions[(int)iconfore_tuple->value->int32]);
}

Tuple * frequpdate = dict_find(iter, MESSAGE_KEY_UpSlider);
if (frequpdate){
  settings.UpSlider = (int) frequpdate -> value -> int32;
  //Restart the counter
  s_countdown = settings.UpSlider;
}

Tuple * disntheme_t = dict_find(iter, MESSAGE_KEY_NightTheme);
  if (disntheme_t){
    if (disntheme_t -> value -> int32 == 0){
      settings.NightTheme = false;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "NTHeme off");
    } else settings.NightTheme = true;
  }

  Tuple * health_t = dict_find(iter, MESSAGE_KEY_HealthOff);
  if (health_t){
    if (health_t -> value -> int32 == 0){
      settings.HealthOff = false;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Health on");
      get_step_count();
      display_step_count();
    } else {
      settings.HealthOff = true;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Health off");
      snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),
       "%s", "");
    }
  }

  Tuple * weatheron_t = dict_find(iter, MESSAGE_KEY_WeatherOn);
  if (weatheron_t){
    if (weatheron_t -> value -> int32 == 0){
      settings.WeatherOn = false;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Current Weather off");

    } else {
      settings.WeatherOn = true;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Current Weather on");

    }
    weather_options_changed = true;
  }

  Tuple * forecast_weatheron_t = dict_find(iter, MESSAGE_KEY_ForecastWeatherOn);
  if (forecast_weatheron_t){
    if (forecast_weatheron_t -> value -> int32 == 0){
      settings.ForecastWeatherOn = false;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Forecast Weather off");
      
    } else {
      settings.ForecastWeatherOn = true;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Forecast Weather on");
   
    }
    weather_options_changed = true;
  }

  Tuple * sunseton_t = dict_find(iter, MESSAGE_KEY_SunsetOn);
  if (sunseton_t){
    if (sunseton_t -> value -> int32 == 0){
      settings.SunsetOn = false;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Sunset/Sunrise off");
    
    } else {
      settings.SunsetOn = true;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Sunset/Sunrise on");
   
    }
    weather_options_changed = true;
  }

  int options_count = settings.WeatherOn + settings.ForecastWeatherOn + settings.SunsetOn;

  if (options_count == 1) {
      if (settings.WeatherOn) {
          showWeather = 1;
      } else if (settings.ForecastWeatherOn) {
          showWeather = 2;
      } else if (settings.SunsetOn) {
          showWeather = 0;
      }
      weather_options_changed = true;
  }

  if (options_count == 2){
    //redraw on save to show a weather option instead of sunset
    if (settings.WeatherOn) {
        showWeather = 1;
    } else if (settings.ForecastWeatherOn) {
        showWeather = 2;
    }
    weather_options_changed = true;
  }



  Tuple * addzero12_t = dict_find(iter, MESSAGE_KEY_AddZero12h);
  if (addzero12_t){
    if (addzero12_t -> value -> int32 == 0){
      settings.AddZero12h = false;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Add Zero 12h off");
    } else {
    settings.AddZero12h = true;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Add Zero 12h on");
      }
    }

  Tuple * remzero24_t = dict_find(iter, MESSAGE_KEY_RemoveZero24h);
  if (remzero24_t){
    if (remzero24_t -> value -> int32 == 0){
      settings.RemoveZero24h = false;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Remove Zero 24h off");
    } else {
    settings.RemoveZero24h = true;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Remove Zero 24h off");
      }
    }
  

  //Update colors
  layer_mark_dirty(s_canvas);
  if(weather_options_changed){
      layer_mark_dirty (s_canvas_sunset_icon);
    }
  layer_mark_dirty(s_canvas_bt_icon);
  layer_mark_dirty(s_canvas_qt_icon);
  
  layer_mark_dirty(time_area_layer);
  // Save the new settings to persistent storage

  prv_save_settings();
}



//Load main layer
static void window_load(Window * window){
  Layer * window_layer = window_get_root_layer(window);
  GRect bounds4 = layer_get_bounds(window_layer);

  s_canvas_background = layer_create(bounds4);
    layer_set_update_proc(s_canvas_background, layer_update_proc_background);
    layer_add_child(window_layer, s_canvas_background);

 time_area_layer = layer_create(bounds4);
     layer_add_child(window_layer, time_area_layer);
     layer_set_update_proc(time_area_layer, update_time_area_layer);

  s_canvas = layer_create(bounds4);
    layer_set_update_proc(s_canvas, layer_update_proc);
    layer_add_child(window_layer, s_canvas);

  s_canvas_sunset_icon = layer_create(bounds4);
    layer_set_update_proc (s_canvas_sunset_icon, layer_update_proc_sunset);
    layer_add_child(window_layer, s_canvas_sunset_icon);

  s_canvas_bt_icon = layer_create(bounds4);
    layer_set_update_proc (s_canvas_bt_icon, layer_update_proc_bt);
    layer_add_child(window_layer, s_canvas_bt_icon);

  s_canvas_qt_icon = layer_create(bounds4);
    layer_set_update_proc (s_canvas_qt_icon, layer_update_proc_qt);
    layer_add_child(window_layer, s_canvas_qt_icon);

  #if defined(PBL_PLATFORM_EMERY)
  
  s_step_layer = text_layer_create (GRect(87, 178, 75, 27));
    text_layer_set_background_color(s_step_layer, GColorClear);
    text_layer_set_font(s_step_layer,
                      FontDaySunsetSteps);
    text_layer_set_text_alignment(s_step_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(s_step_layer));
    
  #else
  s_step_layer = text_layer_create (PBL_IF_ROUND_ELSE(
    GRect(100, 28, 72, 20),
    GRect(0+108-27-18, 132, 54, 20)));
    text_layer_set_background_color(s_step_layer, GColorClear);
    text_layer_set_font(s_step_layer,
                      FontDaySunsetSteps);
    text_layer_set_text_alignment(s_step_layer, (PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter)));
    layer_add_child(window_layer, text_layer_get_layer(s_step_layer));
  #endif    

}


static void window_unload(Window * window){
  layer_destroy (s_canvas_background);
  text_layer_destroy (s_step_layer);
  layer_destroy(s_canvas);
  layer_destroy(time_area_layer);
  layer_destroy(s_canvas_sunset_icon);
  layer_destroy(s_canvas_bt_icon);
  layer_destroy(s_canvas_qt_icon);
  window_destroy(s_window);
  fonts_unload_custom_font(FontIcon);
  fonts_unload_custom_font(FontIcon2);
  fonts_unload_custom_font(FontWeatherIcons);
  ffont_destroy(time_font);
}

void main_window_push(){
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
}

void main_window_update(int hours, int minutes, int weekday, int day){
  s_hours = hours;
  s_minutes = minutes;
  s_day = day;
  s_weekday = weekday;

  layer_mark_dirty(s_canvas_background);
  layer_mark_dirty(s_canvas);
  layer_mark_dirty(s_canvas_sunset_icon);
  layer_mark_dirty(s_canvas_bt_icon);
  layer_mark_dirty(s_canvas_qt_icon);
  layer_mark_dirty(time_area_layer);
}

static void tick_handler(struct tm * time_now, TimeUnits changed){

  main_window_update(time_now -> tm_hour, time_now -> tm_min, time_now -> tm_wday, time_now -> tm_mday);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Tick at %d", time_now -> tm_min);

  if (s_minutes % 10 == 0) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "10-minute health update trigger.");

    // 1. Fetch the latest count from the Health Service
    get_step_count();
    // 2. Refresh the display layer
    display_step_count();

    
  }

  s_loop = 0;
  if (s_countdown == 0 && (settings.WeatherOn || settings.ForecastWeatherOn)) {
      s_countdown = settings.UpSlider;
  } else if (s_countdown == 0){
    //Reset
    s_countdown = 60;
  } else{
    s_countdown = s_countdown - 1;
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Countdown to update %d", s_countdown);
  // Evaluate if is day or night
  int nowthehouris = s_hours * 100 + s_minutes;
  if (HourSunrise <= nowthehouris && nowthehouris <= HourSunset){
    IsNightNow = false;
  } else{
    IsNightNow = true;
  }
  // Extra catch on sunrise and sunset
  if (nowthehouris == HourSunrise || nowthehouris == HourSunset){
    s_countdown = 1;
  }
  if (GPSOn && settings.NightTheme){
    //Extra catch around 1159 to gather information of today
    if (nowthehouris == 1159 && s_countdown > 5){
      s_countdown = 1;
    };
    // Change Color of background
    layer_mark_dirty(s_canvas_background);
    layer_mark_dirty(s_canvas);
    layer_mark_dirty(s_canvas_sunset_icon);
    layer_mark_dirty(s_canvas_bt_icon);
    layer_mark_dirty(s_canvas_qt_icon);
    layer_mark_dirty(time_area_layer);

  }
  // Get weather update every requested minutes and extra request 5 minutes earlier
  if (s_countdown == 0 || s_countdown == 5){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Update weather at %d", time_now -> tm_min);
      request_watchjs();
  }
  //If GPS was off request weather every 15 minutes
  if (!GPSOn){
          if (s_countdown % 15 == 0){
          APP_LOG(APP_LOG_LEVEL_DEBUG, "Attempt to request GPS on %d", time_now -> tm_min);
          request_watchjs();
        }

    }
 }

static void init(){
  prv_load_settings();
  // Listen for AppMessages
  //Starting loop at 0
  s_loop = 0;

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  s_hours=t->tm_hour;
  s_minutes=t->tm_min;
  s_day=t->tm_mday;
  s_weekday=t->tm_wday;
  //Register and open
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(512, 512);
  // Load Fonts
   // allocate fonts
 
  time_font =  ffont_create_from_resource(RESOURCE_ID_FONT_STEELFISH);
 
#ifdef PBL_PLATFORM_EMERY

  FontDaySunsetSteps = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  FontDateNumber = fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS);
  FontBattery= fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  FontFore = fonts_get_system_font(FONT_KEY_GOTHIC_24);
  FontWeatherIcons = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHERICONS_22));
  FontIcon = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHERICONS_28));
  FontIcon2 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DRIPICONS_18));
  
#else

  FontDaySunsetSteps = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  FontDateNumber = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  FontBattery= fonts_get_system_font(FONT_KEY_GOTHIC_14);
  FontFore = PBL_IF_ROUND_ELSE(fonts_get_system_font(FONT_KEY_GOTHIC_14),fonts_get_system_font(FONT_KEY_GOTHIC_18));
  FontWeatherIcons = fonts_load_custom_font(resource_get_handle(PBL_IF_ROUND_ELSE(RESOURCE_ID_FONT_WEATHERICONS_12,RESOURCE_ID_FONT_WEATHERICONS_16)));
  FontIcon = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHERICONS_20));
  FontIcon2 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DRIPICONS_16));
  
  
#endif

  main_window_push();
  // Register with Event Services
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  if (!settings.HealthOff && step_data_is_available())  {
    health_service_events_subscribe(health_handler,NULL);
    health_handler(HealthEventSignificantUpdate, NULL);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "health is on, and steps data is subscribed");
  }
  connection_service_subscribe((ConnectionHandlers){
    .pebble_app_connection_handler = bluetooth_vibe_icon
  });
  bluetooth_vibe_icon(connection_service_peek_pebble_app_connection());
  accel_tap_service_subscribe(accel_tap_handler);
}
 
static void deinit(){
  tick_timer_service_unsubscribe();
  app_message_deregister_callbacks(); //Destroy the callbacks for clean up
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
  health_service_events_unsubscribe();
  accel_tap_service_unsubscribe();
}
int main(){
  init();
  app_event_loop();
  deinit();
}
