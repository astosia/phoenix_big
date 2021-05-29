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

//Static and initial vars
static GFont //FontHour,
  FontDate, FontDate2, FontSunset, FontWeather, FontBattery, FontSteps, FontIcon, FontIcon2, FontIcon3,FontCity, FontWeatherIcons, FontWind, FontFore;
/*char
  sunsetstring[44],
  sunrisestring[44],
  sunsetstring12[44],
  sunrisestring12[44],
  moonstring[44];*/

FFont* time_font;

static Window * s_window;


//GColor color_loser;
//GColor color_winner;

static Layer * s_canvas_background;
//static Layer * s_canvas_to_be_rotated;
static Layer * s_canvas;
static Layer * s_canvas_sunset_icon;
static Layer * s_canvas_bt_icon;
static Layer * s_canvas_qt_icon;
//static Layer * s_canvas_step_icon;
static TextLayer *s_step_layer;
Layer * hour_area_layer;
Layer * minute_area_layer;


static int s_hours, s_minutes, s_weekday, s_day, s_loop;//, s_countdown;

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
    "\U0000F00D", // 'clear': 1,
    "\U0000F00D", // 'sunny': 2,
    "\U0000F002", // 'partlycloudy': 3,
    "\U0000F041", // 'mostlycloudy': 4,
    "\U0000F00C", // 'mostlysunny': 5,
    "\U0000F002", // 'partlysunny': 6,
    "\U0000F013", // 'cloudy': 7,
    "\U0000F019", // 'rain': 8,
    "\U0000F01B", // 'snow': 9,
    "\U0000F01D", // 'tstorms': 10,
    "\U0000F0b5", // 'sleat': 11,
    "\U0000F00A", // 'flurries': 12,
    "\U0000F0b6", // 'hazy': 13,
    "\U0000F01D", // 'chancetstorms': 14,
    "\U0000F01B", // 'chancesnow': 15,
    "\U0000F0b5", // 'chancesleat': 16,
    "\U0000F008", // 'chancerain': 17,
    "\U0000F01B", // 'chanceflurries': 18,
    "\U0000F07B", // 'nt_unknown': 19,
    "\U0000F02E", // 'nt_clear': 20,
    "\U0000F02E", // 'nt_sunny': 21,
    "\U0000F083", // 'nt_partlycloudy': 22,
    "\U0000F086", // 'nt_mostlycloudy': 23,
    "\U0000F081", // 'nt_mostlysunny': 24,
    "\U0000F086", // 'nt_partlysunny': 25,
    "\U0000F013", // 'nt_cloudy': 26,
    "\U0000F019", // 'nt_rain': 27,
    "\U0000F01B", // 'nt_snow': 28,
    "\U0000F01D", // 'nt_tstorms': 29,
    "\U0000F0b5", // 'nt_sleat': 30,
    "\U0000F038", // 'nt_flurries': 31,
    "\U0000F04A", // 'nt_hazy': 32,
    "\U0000F01D", // 'nt_chancetstorms': 33,
    "\U0000F038", // 'nt_chancesnow': 34,
    "\U0000F0B3", // 'nt_chancesleat': 35,
    "\U0000F036", // 'nt_chancerain': 36,
    "\U0000F038", // 'nt_chanceflurries': 37,
    "\U0000F003", // 'fog': 38,
    "\U0000F04A", // 'nt_fog': 39,
    "\U0000F050", // 'strong wind': 40,
    "\U0000F015", // 'hail': 41,
    "\U0000F056", // 'tornado': 42,
};
//////Init Configuration///
//Init Clay
ClaySettings settings;
// Initialize the default settings
static void prv_default_settings(){
  //settings.Back1Color = GColorBlack;
//  settings.FrameColor = GColorCobaltBlue;
  settings.FrameColor1 = GColorBlack;
//  settings.FrameColor2 = GColorCobaltBlue;
  settings.SideColor1 = GColorBlack;
  settings.SideColor2 = GColorBlack;
  settings.Text1Color = GColorWhite;
  settings.Text2Color = GColorWhite;
  settings.Text3Color = GColorWhite;
  settings.Text4Color = GColorWhite;
  settings.Text5Color = GColorWhite;
  settings.Text6Color = GColorWhite;
  settings.Text7Color = GColorWhite;
  settings.Text8Color = GColorWhite;
  settings.HourColor = GColorWhite;
  settings.MinColor = GColorWhite;
  settings.HourColorN = GColorBlack;
  settings.MinColorN = GColorBlack;
  //settings.Back1ColorN = GColorWhite;
//  settings.FrameColorN = GColorWhite;
  settings.FrameColor1N = GColorWhite;
//  settings.FrameColor2N = GColorWhite;
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
//  settings.WeatherUnit = 0;
//  settings.WindUnit = 0;
  settings.UpSlider = 30;
  settings.NightTheme = false;
  settings.HealthOff = true;
  settings.AddZero12h = false;
  settings.RemoveZero24h = false;
  settings.WeatherOn = false;
}
int HourSunrise=700;
int HourSunset=2200;
int moonphase=0;
bool BTOn=true;
bool GPSOn=true;
bool IsNightNow=false;
//bool showWeather = false;
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

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  // A tap event occured

  //showWeather = showweather +1;
  if (showWeather == 2){
    //Reset
    showWeather = 0;
  } else{
    showWeather = showWeather + 1;
  }

  //  showWeather = !showWeather;
  layer_mark_dirty (s_canvas);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "showweather is %d", showWeather);

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
static int s_step_count = 0, s_step_goal = 0, s_step_average = 0;

// Is step data available?
bool step_data_is_available() {
  return HealthServiceAccessibilityMaskAvailable &
    health_service_metric_accessible(HealthMetricStepCount,
      time_start_of_today(), time(NULL));
}

// Daily step goal
static void get_step_goal() {
  const time_t start = time_start_of_today();
  const time_t end = start + SECONDS_PER_DAY;
  s_step_goal = (int)health_service_sum_averaged(HealthMetricStepCount,
    start, end, HealthServiceTimeScopeDaily);
}

// Todays current step count
static void get_step_count() {
    s_step_count = (int)health_service_sum_today(HealthMetricStepCount);
}

// Average daily step count for this time of day
static void get_step_average() {
  const time_t start = time_start_of_today();
  const time_t end = time(NULL);
  s_step_average = (int)health_service_sum_averaged(HealthMetricStepCount,
    start, end, HealthServiceTimeScopeDaily);
}

static void display_step_count() {
  int thousands = s_step_count / 1000;
  int hundreds = (s_step_count % 1000)/100;
  int hundreds2 = (s_step_count % 1000);
//  static char s_emoji[5];

  if(s_step_count >= s_step_average) {
    text_layer_set_text_color(s_step_layer, ColorSelect(settings.Text1Color, settings.Text1ColorN));//, color_winner));
//    snprintf(s_emoji, sizeof(s_emoji), "\U0001F600");
  } else {
    text_layer_set_text_color(s_step_layer, ColorSelect(settings.Text1Color, settings.Text1ColorN));//, color_loser));
//    snprintf(s_emoji, sizeof(s_emoji), "\U0001F61E");
  }

  if(thousands > 0) {
    PBL_IF_ROUND_ELSE(
    //  snprintf(s_current_steps_buffer,sizeof(s_current_steps_buffer),
    //          "%d,%03d",thousands,hundreds2),
    snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),
     "%d.%d%s", thousands, hundreds, "k"),
      snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),
      "%d.%d%s", thousands, hundreds, "k"/*s_emoji*/));
  } else {
    snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),
      "%d"/* %s"*/, hundreds2/*, s_emoji*/);
  }
  text_layer_set_text(s_step_layer, s_current_steps_buffer);
 //  text_layer_set_text(s_step_layer, "25.4k");
}



static void health_handler(HealthEventType event, void *context) {
  if(event == HealthEventSignificantUpdate) {
    get_step_goal();
  }

  if(event != HealthEventSleepUpdate) {
    get_step_count();
    get_step_average();
    display_step_count();

  }
}

void layer_update_proc_background (Layer * back_layer, GContext * ctx){
  // Create Rects
  GRect bounds = layer_get_bounds(back_layer);
  GRect MediumBand =
    PBL_IF_ROUND_ELSE(
     GRect(0, 0, 38, bounds.size.h),
    //GRect(54, 8, 72, 72),
     GRect(0, 0, 0, 0));

 /*  GRect MediumBand2 =
    PBL_IF_ROUND_ELSE(
     GRect(54, 81, 72, 18 ),
      GRect(36, 75, 72, 18));

  GRect MediumBand3 =
    PBL_IF_ROUND_ELSE(
     GRect(56, 90, 68, bounds1.size.h/2),
      GRect(38, 84, 68, bounds1.size.h/2));*/

 GRect SideBand1 =
    PBL_IF_ROUND_ELSE(
     GRect(100, 0, 80, bounds.size.h ),
      GRect(72, 0, 72, bounds.size.h));

 GRect SideBand2 =
    PBL_IF_ROUND_ELSE(
     GRect(38, 0, 100-38, bounds.size.h ),
      GRect(0, 0, 72, bounds.size.h));

      graphics_context_set_fill_color(ctx, ColorSelect(settings.FrameColor1, settings.FrameColor1N));
      graphics_fill_rect(ctx, MediumBand,0,GCornersAll);
      //    graphics_context_set_fill_color(ctx1, ColorSelect(settings.FrameColor1, settings.FrameColor1N));
      //  graphics_fill_rect(ctx1, MediumBand2,2,GCornersAll);
      //    graphics_context_set_fill_color(ctx1, ColorSelect(settings.FrameColor2, settings.FrameColor2N));
      //  graphics_fill_rect(ctx1, MediumBand3,0,GCornersAll);

      graphics_context_set_fill_color(ctx, ColorSelect(settings.SideColor1, settings.SideColor1N));
      graphics_fill_rect(ctx, SideBand1,0,GCornersAll);
      graphics_context_set_fill_color(ctx, ColorSelect(settings.SideColor2, settings.SideColor2N));
      graphics_fill_rect(ctx, SideBand2,0,GCornersAll);
}

void update_hour_area_layer(Layer *l, GContext* ctx7) {
  // check layer bounds
  GRect bounds = layer_get_unobstructed_bounds(l);
  GRect boundsobs = layer_get_bounds(l);

  #ifdef PBL_ROUND
 //   bounds = GRect(0, ROUND_VERTICAL_PADDING, bounds.size.w, bounds.size.h - ROUND_VERTICAL_PADDING * 2);

     bounds = GRect(0, 0,bounds.size.w/3*2, bounds.size.h);
  //#else
  //   bounds = GRect(0,0,bounds.size.w/2,bounds.size.h);
  #endif

  // initialize FCTX, the fancy 3rd party drawing library that all the cool kids use
  FContext fctx;

  fctx_init_context(&fctx, ctx7);
  fctx_set_color_bias(&fctx, 0);
  fctx_set_fill_color(&fctx, ColorSelect(settings.HourColor, settings.HourColorN));

// calculate font size
// int font_size = 4 * bounds.size.h / 7;
// int font_size = bounds.size.h/2.5; //opensans
// int font_size = bounds.size.h * 1.2 ; //steelfish
#ifdef PBL_ROUND
  int font_size = 168;//bounds.size.h * 1.15;
 #else
  int font_size =  boundsobs.size.h;//168;//
  #endif
// steelfish metrics
//  int v_padding = bounds.size.h / 16;
//  int h_padding = bounds.size.w / 16;
  int h_adjust = 0;
  int v_adjust = 0;

    #ifdef PBL_COLOR
      fctx_enable_aa(true);
    #endif

  // if it's a round watch, EVERYTHING CHANGES
  #ifdef PBL_ROUND
//    v_adjust = ROUND_VERTICAL_PADDING;
    v_adjust = 0;

  #else
    // for rectangular watches, adjust X position based on sidebar position
  //    h_adjust -= ACTION_BAR_WIDTH / 2 + 1;
   h_adjust = 0;
  #endif

  FPoint time_pos;
  fctx_begin_fill(&fctx);
  fctx_set_text_em_height(&fctx, time_font, font_size);
  fctx_set_color_bias(&fctx,0);
//  fctx_set_text_em_height(&fctx, minutes_font, font_size);

/*  int hourdraw;
  char hournow[8];
  if (clock_is_24h_style()){
    hourdraw=s_hours;
    snprintf(hournow,sizeof(hournow),"%02d",hourdraw);
    }
  else {
    if (s_hours==0 || s_hours==12){
      hourdraw=12;
    }
    else hourdraw=s_hours%12;
  snprintf(hournow, sizeof(hournow), "%d", hourdraw);
 // hourdraw = hourdraw1+(('0'==hourdraw1[0])?1:0));
}*/
  time_t temp = time(NULL);
  struct tm *time_now = localtime(&temp);

  char hourdraw[8];
  if(clock_is_24h_style() && settings.RemoveZero24h){
      strftime(hourdraw, sizeof(hourdraw),"%k",time_now);
  } else if (clock_is_24h_style() && !settings.RemoveZero24h) {
      strftime(hourdraw, sizeof(hourdraw),"%H",time_now);
  } else if (settings.AddZero12h) {
    strftime(hourdraw, sizeof(hourdraw),"%I",time_now);
  } else {
    strftime(hourdraw, sizeof(hourdraw),"%l",time_now);
  }
  // draw hours

//  time_pos.y = INT_TO_FIXED(v_padding + v_adjust);
//  time_pos.x = INT_TO_FIXED(bounds.size.w / 2);
 // time_pos.x = INT_TO_FIXED(bounds.size.w / 4 + h_adjust);
 //   time_pos.x = INT_TO_FIXED(bounds.size.w / 4 + h_adjust); //this one works when center justified
  time_pos.x = INT_TO_FIXED(PBL_IF_ROUND_ELSE(bounds.size.w / 2 +8, 36 + 1) + h_adjust);
  time_pos.y = INT_TO_FIXED(PBL_IF_ROUND_ELSE(bounds.size.h / 2 +1, 1)  + v_adjust);
//  fctx_set_pivot(&fctx, time_pos);

  int width;
  //width = fctx_string_width(&fctx,hourdraw,time_font);
  width = fctx_string_width(&fctx,hourdraw,time_font);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Width is %d",width);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "font size is %d",font_size);

  fctx_set_scale(&fctx,FPoint(width,font_size),FPoint(144*16,PBL_IF_ROUND_ELSE(-font_size*2.25,-font_size*2.65*bounds.size.h/boundsobs.size.h)));

  fctx_set_offset(&fctx, time_pos);
  fctx_draw_string(&fctx, hourdraw, time_font, GTextAlignmentCenter, PBL_IF_ROUND_ELSE(FTextAnchorMiddle,FTextAnchorTop));
  fctx_end_fill(&fctx);

  fctx_deinit_context(&fctx);
}

void update_minute_area_layer(Layer *a, GContext* ctx8) {
  // check layer bounds
 GRect bounds = layer_get_unobstructed_bounds(a);

  #ifdef PBL_ROUND
 //   bounds = GRect(0, ROUND_VERTICAL_PADDING, bounds.size.w, bounds.size.h - ROUND_VERTICAL_PADDING * 2);
 bounds = GRect(bounds.size.w/2, 0,bounds.size.w/2, bounds.size.h);
//#else
// bounds = GRect(bounds.size.w/2,0,bounds.size.w/2,bounds.size.h);
  #endif
  // initialize FCTX, the fancy 3rd party drawing library that all the cool kids use
  FContext fctx1;

  fctx_init_context(&fctx1, ctx8);
  fctx_set_color_bias(&fctx1, 0);
  fctx_set_fill_color(&fctx1, ColorSelect(settings.MinColor, settings.MinColorN));


  // calculate font size
 // int font_size = 4 * bounds.size.h / 7;
//  int font_size2 = bounds.size.h/2.5; //opensans
#ifdef PBL_ROUND
  int font_size2 = bounds.size.h * 0.55;
 #else
  int font_size2 = 109;//bounds.size.h * 0.65;
  #endif
  // avenir + avenir bold metrics
//  int v_padding2 = bounds.size.h / 16;
  int h_adjust2 = 0;
  int v_adjust2 = 0;


    #ifdef PBL_COLOR
      fctx_enable_aa(true);
    #endif


  // if it's a round watch, EVERYTHING CHANGES
  #ifdef PBL_ROUND
 //   v_adjust2 = ROUND_VERTICAL_PADDING;
     v_adjust2 = 0;

  #else
    // for rectangular watches, adjust X position based on sidebar position
 //     h_adjust2 -= ACTION_BAR_WIDTH / 2 + 1;
     h_adjust2 = 0;
  #endif

  FPoint time_pos2;
  fctx_begin_fill(&fctx1);
  fctx_set_text_em_height(&fctx1, time_font, font_size2);
//  fixed_t fctx_string_width(&fctx1, minnow, time_font);
//  fctx_set_text_em_height(&fctx, minutes_font, font_size);

  /*int mindraw;
  mindraw = s_minutes;
  char minnow[8];
  snprintf(minnow, sizeof(minnow), "%02d", mindraw);
*/

  time_t temp = time(NULL);
  struct tm *time_now = localtime(&temp);

  char mindraw[8];
  strftime(mindraw, sizeof(mindraw),"%M",time_now);

  //draw minutes
 // time_pos2.x = INT_TO_FIXED(bounds.size.w / 2 + h_adjust2);
  //time_pos2.y = INT_TO_FIXED(bounds.size.h - v_padding2 + v_adjust2);
//  time_pos2.x = INT_TO_FIXED(PBL_IF_ROUND_ELSE(bounds.size.w, bounds.size.w) + h_adjust2);
//  time_pos2.y = INT_TO_FIXED(bounds.size.h  + v_adjust2);
time_pos2.x = INT_TO_FIXED(PBL_IF_ROUND_ELSE(135, 108) + h_adjust2);
time_pos2.y = INT_TO_FIXED(PBL_IF_ROUND_ELSE(bounds.size.h /2 , 168/4 +6)  + v_adjust2);

//  fctx_set_scale(&fctx1,FPoint(1,UINT8_MAX),FPoint(2,-bounds.size.h ));
//  fctx_set_scale(&fctx1,FPoint(1,UINT8_MAX),FPoint(3,-bounds.size.h /0.65));
  fctx_set_offset(&fctx1, time_pos2);
  fctx_draw_string(&fctx1, mindraw, time_font, GTextAlignmentCenter, FTextAnchorMiddle);
  fctx_end_fill(&fctx1);

  fctx_deinit_context(&fctx1);
}

//Update main layer





static void layer_update_proc(Layer * layer, GContext * ctx){

 GRect DateRect =
  //  (0, offsetdate, bounds3.size.w, bounds1.size.h/4);
   (PBL_IF_ROUND_ELSE(
      GRect(22-22+6, 26+40+2, 32, 40),
      GRect(0+108-19-18, 0+94, 38, 16)));

  GRect DateRect2 =
    (PBL_IF_ROUND_ELSE(
      GRect(22-22+6, 16+22+40+2, 32, 40),
      GRect(0+108-19-18, 16+90, 38, 40)));

  GRect BatteryRect =
   (PBL_IF_ROUND_ELSE(
//    GRect(0,142,54,20),
  //    GRect(22-22+6,66+40,32,20),
      GRect(100,136+12,50,20),
      GRect(0+108-19-18,150, 38, 20)));


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
  graphics_draw_text(ctx, datenow, FontDate, DateRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
  graphics_context_set_text_color(ctx, ColorSelect(settings.Text8Color, settings.Text8ColorN));
  graphics_draw_text(ctx, battperc, FontBattery, BatteryRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);

  graphics_context_set_text_color(ctx, ColorSelect(settings.Text6Color, settings.Text6ColorN));
  graphics_draw_text(ctx, daynow, PBL_IF_ROUND_ELSE(FontDate2,FontDate2), DateRect2, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
}


static void layer_update_proc_sunset(Layer * layer2, GContext * ctx2){
   // Create Rects
//  GRect bounds = layer_get_bounds(layer2);
   GRect SunsetIconRect =
    (PBL_IF_ROUND_ELSE(
   //   GRect(0,104,54,20),
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
    //      GRect(100+4-20,130,75,20),
           GRect(8,106,32,40),
           GRect(0+108+18-28, 146, 54,20)));

           GRect ForeRect =  //temperature number
                (PBL_IF_ROUND_ELSE(
            //      GRect(100+4-20,130,75,20),
                   GRect(8,106+2,32,40),
                   GRect(0+108+18-28, 146, 54,20)));

    GRect IconNowRect = //weather condition icon
         (PBL_IF_ROUND_ELSE(
           GRect(28-20,139-100+6,28,20),
           GRect(0+108-19+18,16+93+10, 38, 40)));
 //onreconnection(BTOn, connection_service_peek_pebble_app_connection());
// bluetooth_callback(connection_service_peek_pebble_app_connection());

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
//int nowthehouris = s_hours * 100 + s_minutes;
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

if (!settings.WeatherOn) {
  graphics_context_set_text_color(ctx2, ColorSelect(settings.Text2Color, settings.Text2ColorN));
  graphics_draw_text(ctx2, SunsetIconToDraw, FontWeatherIcons, SunsetIconRect, GTextOverflowModeFill,PBL_IF_ROUND_ELSE(GTextAlignmentLeft,GTextAlignmentCenter), NULL);
  graphics_context_set_text_color(ctx2,ColorSelect(settings.Text4Color,settings.Text4ColorN));
  graphics_draw_text(ctx2, MoonToDraw, FontIcon, MoonRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
  graphics_context_set_text_color(ctx2, ColorSelect(settings.Text2Color, settings.Text2ColorN));
  graphics_draw_text(ctx2, SunsetToDraw, FontSunset, SunsetRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
} else {

if (showWeather==0) //show moonphase and sunset/sunrise
{
 graphics_context_set_text_color(ctx2, ColorSelect(settings.Text2Color, settings.Text2ColorN));
 graphics_draw_text(ctx2, SunsetIconToDraw, FontWeatherIcons, SunsetIconRect, GTextOverflowModeFill,PBL_IF_ROUND_ELSE(GTextAlignmentLeft,GTextAlignmentCenter), NULL);
 graphics_context_set_text_color(ctx2,ColorSelect(settings.Text4Color,settings.Text4ColorN));
 graphics_draw_text(ctx2, MoonToDraw, FontIcon, MoonRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
 graphics_context_set_text_color(ctx2, ColorSelect(settings.Text2Color, settings.Text2ColorN));
 graphics_draw_text(ctx2, SunsetToDraw, FontSunset, SunsetRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
}
else if (showWeather==1) //show current weather
{
  graphics_context_set_text_color(ctx2,ColorSelect(settings.Text7Color,settings.Text7ColorN));
  graphics_draw_text(ctx2, TempToDraw, FontSunset, TempRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
  graphics_context_set_text_color(ctx2,ColorSelect(settings.Text5Color,settings.Text5ColorN));
  graphics_draw_text(ctx2, CondToDraw, FontIcon, IconNowRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
  #ifdef PBL_ROUND
//  graphics_context_set_text_color(ctx2,ColorSelect(settings.Text4Color,settings.Text4ColorN));
//  graphics_draw_text(ctx2, MoonToDraw, FontIcon, MoonRect, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  graphics_context_set_text_color(ctx2, ColorSelect(settings.Text2Color, settings.Text2ColorN));
  graphics_draw_text(ctx2, SunsetIconToDraw, FontWeatherIcons, SunsetIconRect, GTextOverflowModeFill,PBL_IF_ROUND_ELSE(GTextAlignmentLeft,GTextAlignmentCenter), NULL);
  graphics_context_set_text_color(ctx2, ColorSelect(settings.Text2Color, settings.Text2ColorN));
  graphics_draw_text(ctx2, SunsetToDraw, FontSunset, SunsetRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
  #endif
 }
 else if (showWeather==2) //show forecast weather
 {
   graphics_context_set_text_color(ctx2,ColorSelect(settings.Text7Color,settings.Text7ColorN));
   graphics_draw_text(ctx2, HiLowToDraw, FontFore, ForeRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
   graphics_context_set_text_color(ctx2,ColorSelect(settings.Text5Color,settings.Text5ColorN));
   graphics_draw_text(ctx2, ForeToDraw, FontIcon, IconNowRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
   #ifdef PBL_ROUND
 //  graphics_context_set_text_color(ctx2,ColorSelect(settings.Text4Color,settings.Text4ColorN));
 //  graphics_draw_text(ctx2, MoonToDraw, FontIcon, MoonRect, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
   graphics_context_set_text_color(ctx2, ColorSelect(settings.Text2Color, settings.Text2ColorN));
   graphics_draw_text(ctx2, SunsetIconToDraw, FontWeatherIcons, SunsetIconRect, GTextOverflowModeFill,PBL_IF_ROUND_ELSE(GTextAlignmentLeft,GTextAlignmentCenter), NULL);
   graphics_context_set_text_color(ctx2, ColorSelect(settings.Text2Color, settings.Text2ColorN));
   graphics_draw_text(ctx2, SunsetToDraw, FontSunset, SunsetRect, GTextOverflowModeFill, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
   #endif
  }
}
}

static void layer_update_proc_bt(Layer * layer3, GContext * ctx3){
   // Create Rects
//  GRect bounds = layer_get_bounds(layer2);

  GRect BTIconRect =
    (PBL_IF_ROUND_ELSE(
    //  GRect(76,6,28,20),
    GRect(100,16,72,20),
      GRect(126-18,100-2,18,20)));

 onreconnection(BTOn, connection_service_peek_pebble_app_connection());
 bluetooth_callback(connection_service_peek_pebble_app_connection());

 graphics_context_set_text_color(ctx3, ColorSelect(settings.Text3Color, settings.Text3ColorN));
 graphics_draw_text(ctx3, "z", FontIcon2, BTIconRect, GTextOverflowModeFill,GTextAlignmentCenter, NULL);

}

static void layer_update_proc_qt(Layer * layer4, GContext * ctx4){
   // Create Rects
//  GRect bounds = layer_get_bounds(layer2);

  GRect QTIconRect =
    (PBL_IF_ROUND_ELSE(
      GRect(8,126,32,20),
      GRect(126,100-2,18,20)));

 quiet_time_icon();

 graphics_context_set_text_color(ctx4, ColorSelect(settings.Text3Color, settings.Text3ColorN));
 graphics_draw_text(ctx4, "\U0000E061", FontIcon2, QTIconRect, GTextOverflowModeFill,GTextAlignmentCenter, NULL);

}

/*static void layer_update_proc_step_icon(Layer * layer5, GContext * ctx5){
   // Create Rects
//  GRect bounds = layer_get_bounds(layer2);

  GRect StepIconRect =
    (PBL_IF_ROUND_ELSE(
      GRect(24,126+10,20,20),
        PBL_IF_MICROPHONE_ELSE (
            GRect(0,136,36,22),
            GRect(-20,-20,0,0)))) ;


 graphics_context_set_text_color(ctx5, ColorSelect(settings.Text3Color, settings.Text3ColorN));
 graphics_draw_text(ctx5, "\U0000E022", FontIcon3 , StepIconRect, GTextOverflowModeFill,GTextAlignmentCenter, NULL);

}*/

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
 // update_dont_rotate();
 // update_rotation();
}
// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator * iter, void * context){
  s_loop = s_loop + 1;
  if (s_loop == 1){
    //Clean vars
  //  strcpy(tempstring, "");
  //  strcpy(condstring, "");
  //  strcpy(citistring, " ");
  //  strcpy(windstring, "");
 //   strcpy(sunsetstring, " ");
  //  strcpy(iconnowstring, "");
  //  strcpy(iconforestring, "");
 //   strcpy(tempforestring, "");

  }
  // Background Color
  /*Tuple * bg1_color_t = dict_find(iter, MESSAGE_KEY_Back1Color);
  if (bg1_color_t){
    settings.Back1Color = GColorFromHEX(bg1_color_t-> value -> int32);
  }
  Tuple * nbg1_color_t = dict_find(iter, MESSAGE_KEY_Back1ColorN);
  if (nbg1_color_t){
    settings.Back1ColorN = GColorFromHEX(nbg1_color_t-> value -> int32);
  }*/
/*  Tuple * fr_color_t = dict_find(iter, MESSAGE_KEY_FrameColor);
  if (fr_color_t){
    settings.FrameColor = GColorFromHEX(fr_color_t-> value -> int32);
  }
  Tuple * nfr_color_t = dict_find(iter, MESSAGE_KEY_FrameColorN);
  if (nfr_color_t){
    settings.FrameColorN = GColorFromHEX(nfr_color_t-> value -> int32);
  }*/
     Tuple * fr1_color_t = dict_find(iter, MESSAGE_KEY_FrameColor1);
  if (fr1_color_t){
    settings.FrameColor1 = GColorFromHEX(fr1_color_t-> value -> int32);
  }
  Tuple * nfr1_color_t = dict_find(iter, MESSAGE_KEY_FrameColor1N);
  if (nfr1_color_t){
    settings.FrameColor1N = GColorFromHEX(nfr1_color_t-> value -> int32);
  }
/* Tuple * fr2_color_t = dict_find(iter, MESSAGE_KEY_FrameColor2);
  if (fr2_color_t){
    settings.FrameColor2 = GColorFromHEX(fr2_color_t-> value -> int32);
  }
  Tuple * nfr2_color_t = dict_find(iter, MESSAGE_KEY_FrameColor2N);
  if (nfr2_color_t){
    settings.FrameColor2N = GColorFromHEX(nfr2_color_t-> value -> int32);
  }*/
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
/*  Tuple * wcond_t = dict_find(iter, MESSAGE_KEY_WeatherCond);
  if (wcond_t){
    snprintf(condstring, sizeof(condstring), "%s", wcond_t -> value -> cstring);
  }
  // Weather Temp
  Tuple * wtemp_t = dict_find(iter, MESSAGE_KEY_WeatherTemp);
  if (wtemp_t){
    snprintf(tempstring, sizeof(tempstring), "%s", wtemp_t -> value -> cstring);
  }
   Tuple * wwind_t = dict_find(iter, MESSAGE_KEY_WeatherWind);
  if (wwind_t){
    snprintf(windstring, sizeof(windstring), "%s", wwind_t -> value -> cstring);
  }

     Tuple * wwindave_t = dict_find(iter, MESSAGE_KEY_WindFore);
  if (wwindave_t){
    snprintf(windavestring, sizeof(windavestring), "%s", wwindave_t -> value -> cstring);
  }*/
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
/*   Tuple * iconnow_tuple = dict_find(iter, MESSAGE_KEY_IconNow);
  //////////Add in icons and forecast hi/lo temp////////////
  if (iconnow_tuple){
    snprintf(iconnowstring,sizeof(iconnowstring),"%s",weather_conditions[(int)iconnow_tuple->value->int32]);
    //  snprintf(forecast_result,sizeof(forecast_result),"%s","\U0000F002");
}
  Tuple * iconfore_tuple = dict_find(iter, MESSAGE_KEY_IconFore);
  if (iconfore_tuple){
    snprintf(iconforestring,sizeof(iconforestring),"%s",weather_conditions[(int)iconfore_tuple->value->int32]);
    //  snprintf(forecast_result,sizeof(forecast_result),"%s","\U0000F002");
}
   Tuple * iconwinddirnow_tuple = dict_find(iter, MESSAGE_KEY_WindIconNow);
  if (iconwinddirnow_tuple){
    snprintf(windiconnowstring,sizeof(windiconnowstring),"%s",wind_direction[(int)iconwinddirnow_tuple->value->int32]);
    //  snprintf(forecast_result,sizeof(forecast_result),"%s","\U0000F002");
}
   Tuple * iconwinddirave_tuple = dict_find(iter, MESSAGE_KEY_WindIconAve);
  if (iconwinddirave_tuple){
    snprintf(windiconavestring,sizeof(windiconavestring),"%s",wind_direction[(int)iconwinddirave_tuple->value->int32]);
    //  snprintf(forecast_result,sizeof(forecast_result),"%s","\U0000F002");
}
  Tuple * wforetemp_t = dict_find(iter, MESSAGE_KEY_TempFore);
  if (wforetemp_t){
    snprintf(temphistring, sizeof(temphistring), "%s", wforetemp_t -> value -> cstring);
  }

  Tuple * wforetemplow_t = dict_find(iter, MESSAGE_KEY_TempForeLow);
  if (wforetemplow_t){
    snprintf(templowstring, sizeof(templowstring), "%s", wforetemplow_t -> value -> cstring);
  }*/

  Tuple * moon_tuple = dict_find(iter, MESSAGE_KEY_MoonPhase);
  if (moon_tuple){
    snprintf(settings.moonstring,sizeof(settings.moonstring),"%s",moon_phase[(int)moon_tuple->value->int32]);
    //  snprintf(forecast_result,sizeof(forecast_result),"%s","\U0000F002");
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
  //  snprintf(forecast_result,sizeof(forecast_result),"%s","\U0000F002");
}

Tuple * frequpdate = dict_find(iter, MESSAGE_KEY_UpSlider);
if (frequpdate){
  settings.UpSlider = (int) frequpdate -> value -> int32;
  //Restart the counter
  s_countdown = settings.UpSlider;
}

  // Location
/*  Tuple * neigh_t = dict_find(iter, MESSAGE_KEY_NameLocation);
  if (neigh_t){
    snprintf(citistring, sizeof(citistring), "%s", neigh_t -> value -> cstring);
  }*/
  //Control of data gathered for http
/*  APP_LOG(APP_LOG_LEVEL_DEBUG, "After loop %d temp is %s Cond is %s and City is %s and Wind is %s", s_loop, tempstring, condstring, citistring, windstring);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sunrise is %d Sunset is %d SunsetDisplay is %s", HourSunrise,HourSunset,sunsetstring);
  if (strcmp(tempstring, "") != 0 && strcmp(condstring, "") != 0 && strcmp(citistring, "")){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "GPS fully working at loop %d", s_loop);
    GPSOn = true;
  } else{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Missing info at loop %d, GPS false", s_loop);
    GPSOn = false;
  }
  //End data gathered
  // Get display handlers
  Tuple * frequpdate = dict_find(iter, MESSAGE_KEY_UpSlider);
  if (frequpdate){
    settings.UpSlider = (int) frequpdate -> value -> int32;
    //Restart the counter
    s_countdown = settings.UpSlider;
  }*/
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
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather off");
      //get_step_count();
      //display_step_count();
    } else {
      settings.WeatherOn = true;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather on");
    //  snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),
      // "%s", "");
    }
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
  /*Tuple * rot_t = dict_find(iter,MESSAGE_KEY_Rotate);
  if (rot_t){
    if(rot_t -> value -> int32==0){
      settings.Rotate = false;
   //   layer_set_hidden(effect_layer_get_layer(effect_layer_right),true);
   //   layer_set_hidden(effect_layer_get_layer(effect_layer_left),true);
   //   layer_set_hidden(effect_layer_get_layer(effect_layer_leftleft),true);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "rotation set to off");
    } else settings.Rotate = true;

  Tuple * dir_t = dict_find(iter, MESSAGE_KEY_RightLeft);
  if (dir_t){
    if (dir_t -> value -> int32 == 0 && settings.Rotate == true){
      settings.RightLeft = false;
    //  layer_set_hidden(effect_layer_get_layer(effect_layer_right),true);
   //   layer_set_hidden(effect_layer_get_layer(effect_layer_left),false);
   //   layer_set_hidden(effect_layer_get_layer(effect_layer_leftleft),false);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "rotation set to left");
    }
    else if (dir_t -> value-> int32 != 0 && settings.Rotate == true){
      settings.RightLeft = true;
   //  layer_set_hidden(effect_layer_get_layer(effect_layer_right),false);
   //  layer_set_hidden(effect_layer_get_layer(effect_layer_left),true);
   //  layer_set_hidden(effect_layer_get_layer(effect_layer_leftleft),true);
       APP_LOG(APP_LOG_LEVEL_DEBUG, "rotation set to right");
    } }}*/

  //Update colors
  //layer_mark_dirty(s_canvas_to_be_rotated);
  layer_mark_dirty(s_canvas);
  layer_mark_dirty(s_canvas_sunset_icon);
  layer_mark_dirty(s_canvas_bt_icon);
  layer_mark_dirty(s_canvas_qt_icon);
  //  layer_mark_dirty(s_canvas_step_icon);
  layer_mark_dirty(hour_area_layer);
  layer_mark_dirty(minute_area_layer);
  // Save the new settings to persistent storage

 // update_rotation();
  prv_save_settings();
}



//Load main layer
static void window_load(Window * window){
  Layer * window_layer = window_get_root_layer(window);
  GRect bounds4 = layer_get_bounds(window_layer);


//  effect_layer = layer_create(bounds);

 /* effect_layer_2 = effect_layer_create(PBL_IF_ROUND_ELSE(
      GRect(0,0,180,180),
      GRect(0, 0,144,168 )));
  effect_layer_add_effect(effect_layer_2, effect_rotate_90_degrees, (void *)true);
 // layer_set_update_proc(effect_layer_2, layer_update_proc_rotate);
  layer_add_child(window_get_root_layer(s_window), effect_layer_get_layer(effect_layer_2));*/

  s_canvas_background = layer_create(bounds4);
    layer_set_update_proc(s_canvas_background, layer_update_proc_background);
    layer_add_child(window_layer, s_canvas_background);

  hour_area_layer = layer_create(bounds4);
  //     layer_add_child(window_get_root_layer(s_window), hour_area_layer);
     layer_add_child(window_layer, hour_area_layer);
     layer_set_update_proc(hour_area_layer, update_hour_area_layer);

 minute_area_layer = layer_create(bounds4);
     layer_add_child(window_layer, minute_area_layer);
     layer_set_update_proc(minute_area_layer, update_minute_area_layer);

  /*s_canvas_to_be_rotated = layer_create(bounds4);
    layer_set_update_proc(s_canvas_to_be_rotated, layer_update_proc_pre_rotate);
    layer_add_child(window_layer, s_canvas_to_be_rotated);*/

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



/*  s_canvas_step_icon = layer_create(bounds4);
     layer_add_child(window_get_root_layer(s_window), s_canvas_step_icon);
     layer_set_update_proc (s_canvas_step_icon, layer_update_proc_step_icon);
*/

  s_step_layer = text_layer_create (PBL_IF_ROUND_ELSE(
    GRect(100, 28, 72, 20),
    GRect(0+108-27-18, 132, 54, 20)));
    text_layer_set_background_color(s_step_layer, GColorClear);
    text_layer_set_font(s_step_layer,
                      FontSteps);
            //          fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    text_layer_set_text_alignment(s_step_layer, (PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter)));
    layer_add_child(window_layer, text_layer_get_layer(s_step_layer));



/*

  effect_layer_right = effect_layer_create(PBL_IF_ROUND_ELSE(
      GRect(0,0,180,180),
      GRect(0, 0,144,168 )));
  effect_layer_add_effect(effect_layer_right, effect_rotate_90_degrees, (void*)true);
  layer_add_child(window_get_root_layer(s_window), effect_layer_get_layer(effect_layer_right));


  effect_layer_left = effect_layer_create(PBL_IF_ROUND_ELSE(
  GRect(0,0,180,180),
    GRect(0,0,144,168)));
  effect_layer_add_effect(effect_layer_left, effect_rotate_90_degrees, (void*)false);
  layer_add_child(window_get_root_layer(s_window), effect_layer_get_layer(effect_layer_left));

 */
  /*


  s_sunset_icon_layer2 = text_layer_create((PBL_IF_ROUND_ELSE(
 //   GRect((bounds.size.w/2)-24, bounds.size.h-22+2, 20, 20),
    GRect(28,126,20,20),
    GRect(0,128+3,20,20))));
  //bitmap_layer_set_bitmap(s_sunset_icon_layer, s_sunset_icon_bitmap);
  text_layer_set_text_color(s_sunset_icon_layer2,ColorSelect(settings.Text2Color,settings.Text2ColorN));
  text_layer_set_background_color(s_sunset_icon_layer2,GColorClear);
  text_layer_set_font(s_sunset_icon_layer2,FontIcon);
  text_layer_set_text_alignment(s_sunset_icon_layer2,GTextAlignmentCenter);
  text_layer_set_text(s_sunset_icon_layer2,"\U0000F052");
  //Create the Bluetooth icon GBitmap
//  s_bt_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_ICON);

  // Create the BitmapLayer to display the GBitmap
  s_bt_icon_layer2 = text_layer_create(PBL_IF_ROUND_ELSE(
//    GRect((bounds4.size.w/4*3) - 15, 30, 30, 30),
//    GRect((bounds4.size.w/4*3) - 12,10,30,30)));
     GRect(28,126,20,20),
     GRect(0,128+3,20,20)));
  text_layer_set_text_color(s_bt_icon_layer2,ColorSelect(settings.Text2Color,settings.Text2ColorN));
  text_layer_set_background_color(s_bt_icon_layer2,GColorClear);
  text_layer_set_font(s_bt_icon_layer2,FontIcon2);
  text_layer_set_text_alignment(s_bt_icon_layer2,GTextAlignmentCenter);
  text_layer_set_text(s_bt_icon_layer2, "z");

  layer_add_child(window_layer, text_layer_get_layer(s_bt_icon_layer2));
  layer_add_child(window_layer, text_layer_get_layer(s_sunset_icon_layer2));*/



}


static void window_unload(Window * window){
  layer_destroy (s_canvas_background);
//  layer_destroy(s_canvas_to_be_rotated);
 // effect_layer_destroy(effect_layer_right);
 // effect_layer_destroy(effect_layer_left);
 // effect_layer_destroy(effect_layer_leftleft);
//    effect_layer_destroy(effect_layer_3);
  //text_layer_destroy(s_sunset_icon_layer2);
  //text_layer_destroy(s_bt_icon_layer2);
  text_layer_destroy    (s_step_layer);
  //text_layer_destroy  (s_weather_icon_layer);
 // text_layer_destroy  (s_forecast_icon_layer);
  layer_destroy(s_canvas);
   layer_destroy(hour_area_layer);
  layer_destroy(minute_area_layer);
  layer_destroy(s_canvas_sunset_icon);
  layer_destroy(s_canvas_bt_icon);
  layer_destroy(s_canvas_qt_icon);
 // layer_destroy(s_canvas_step_icon);
 // if (effect_layer != NULL) {
	//  effect_layer_destroy(effect_layer);
 // }
  window_destroy(s_window);
//  fonts_unload_custom_font(FontHour);
  fonts_unload_custom_font(FontIcon);
  fonts_unload_custom_font(FontIcon2);
  fonts_unload_custom_font(FontIcon3);
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

//   update_time();
}
void main_window_update(int hours, int minutes, int weekday, int day){
  s_hours = hours;
  s_minutes = minutes;
  s_day = day;
  s_weekday = weekday;

  layer_mark_dirty(s_canvas_background);
  layer_mark_dirty(s_canvas);
//  layer_mark_dirty(s_canvas_to_be_rotated);
  layer_mark_dirty(s_canvas_sunset_icon);
  layer_mark_dirty(s_canvas_bt_icon);
  layer_mark_dirty(s_canvas_qt_icon);
 // layer_mark_dirty(s_canvas_step_icon);
  layer_mark_dirty(hour_area_layer);
  layer_mark_dirty(minute_area_layer);

 /////// update_rotation(); ///use this one

}

static void tick_handler(struct tm * time_now, TimeUnits changed){

  main_window_update(time_now -> tm_hour, time_now -> tm_min, time_now -> tm_wday, time_now -> tm_mday);
  //update_time();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Tick at %d", time_now -> tm_min);
  s_loop = 0;
  if (s_countdown == 0 && settings.WeatherOn) {
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
//    layer_mark_dirty(s_canvas_to_be_rotated);
    layer_mark_dirty(s_canvas);
    layer_mark_dirty(s_canvas_sunset_icon);
    layer_mark_dirty(s_canvas_bt_icon);
    layer_mark_dirty(s_canvas_qt_icon);
   // layer_mark_dirty(s_canvas_step_icon);
    layer_mark_dirty(hour_area_layer);
    layer_mark_dirty(minute_area_layer);

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
//  color_loser = GColorRed;
//  color_winner = GColorJaegerGreen;

  prv_load_settings();
  // Listen for AppMessages
  //Starting loop at 0
  s_loop = 0;
//  s_countdown = 60;
  //Clean vars
 // strcpy(tempstring, "");
  //strcpy(condstring, "");
//  strcpy(citistring, " ");
 // strcpy(windstring,"");
 // strcpy(sunsetstring, " ");
  // strcpy(iconnowstring, "");
  //  strcpy(iconforestring, "");
  //  strcpy(tempforestring, "");

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
 // time_font =  ffont_create_from_resource(RESOURCE_ID_OPENSANSCON_FFONT);
   time_font =  ffont_create_from_resource(RESOURCE_ID_FONT_STEELFISH);
 // time_font = ffont_create_from_resource(RESOURCE_ID_FONT_DINCONBOLD);
 /* FontHour = fonts_load_custom_font(resource_get_handle(
      PBL_IF_ROUND_ELSE(RESOURCE_ID_FONT_DINCON_BOLD_60,
              PBL_IF_MICROPHONE_ELSE(RESOURCE_ID_FONT_DINCON_BOLD_64,RESOURCE_ID_FONT_DINCON_BOLD_56))));*/
  FontDate = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  FontDate2 = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  FontBattery= fonts_get_system_font(FONT_KEY_GOTHIC_14);
  FontSunset = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  FontFore = PBL_IF_ROUND_ELSE(fonts_get_system_font(FONT_KEY_GOTHIC_14),fonts_get_system_font(FONT_KEY_GOTHIC_18));
  FontIcon = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHERICONS_20));
  FontIcon2 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DRIPICONS_16));
  FontIcon3 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DRIPICONS_18));
  FontWeather = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  FontCity = PBL_IF_ROUND_ELSE(fonts_get_system_font(FONT_KEY_GOTHIC_14),fonts_get_system_font(FONT_KEY_GOTHIC_09));
  FontWeatherIcons = fonts_load_custom_font(resource_get_handle(PBL_IF_ROUND_ELSE(RESOURCE_ID_FONT_WEATHERICONS_12,RESOURCE_ID_FONT_WEATHERICONS_16)));
  FontSteps = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  FontWind = fonts_get_system_font(FONT_KEY_GOTHIC_18);

  main_window_push();
  // Register with Event Services
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  if (!settings.HealthOff && step_data_is_available())  {
    health_service_events_subscribe(health_handler,NULL);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "health is on, and steps data is subscribed");
  }
  connection_service_subscribe((ConnectionHandlers){
    .pebble_app_connection_handler = bluetooth_vibe_icon
  });
  bluetooth_vibe_icon(connection_service_peek_pebble_app_connection());
  accel_tap_service_subscribe(accel_tap_handler);
 // handle_battery(battery_state_service_peek());
}
static void deinit(){
  tick_timer_service_unsubscribe();
  app_message_deregister_callbacks(); //Destroy the callbacks for clean up
//   tick_timer_service_unsubscribe();
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
