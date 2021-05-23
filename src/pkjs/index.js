//function () { "use strict";

// shortcuts for easier to read formulas

var PI   = Math.PI,
    sin  = Math.sin,
    cos  = Math.cos,
    tan  = Math.tan,
    asin = Math.asin,
    atan = Math.atan2,
    acos = Math.acos,
    rad  = PI / 180;

// sun calculations are based on http://aa.quae.nl/en/reken/zonpositie.html formulas


// date/time constants and conversions

var dayMs = 1000 * 60 * 60 * 24,
    J1970 = 2440588,
    J2000 = 2451545;

function toJulian(date) {
    return date.valueOf() / dayMs - 0.5 + J1970;
}
function fromJulian(j) {
    return new Date((j + 0.5 - J1970) * dayMs);
}
function toDays(date) {
    return toJulian(date) - J2000;
}


// general calculations for position

var e = rad * 23.4397; // obliquity of the Earth

function getRightAscension(l, b) {
    return atan(sin(l) * cos(e) - tan(b) * sin(e), cos(l));
}
function getDeclination(l, b) {
    return asin(sin(b) * cos(e) + cos(b) * sin(e) * sin(l));
}
function getAzimuth(H, phi, dec) {
    return atan(sin(H), cos(H) * sin(phi) - tan(dec) * cos(phi));
}
function getAltitude(H, phi, dec) {
    return asin(sin(phi) * sin(dec) + cos(phi) * cos(dec) * cos(H));
}
function getSiderealTime(d, lw) {
    return rad * (280.16 + 360.9856235 * d) - lw;
}


// general sun calculations

function getSolarMeanAnomaly(d) {
    return rad * (357.5291 + 0.98560028 * d);
}
function getEquationOfCenter(M) {
    return rad * (1.9148 * sin(M) + 0.02 * sin(2 * M) + 0.0003 * sin(3 * M));
}
function getEclipticLongitude(M, C) {
    var P = rad * 102.9372; // perihelion of the Earth
    return M + C + P + PI;
}
function getSunCoords(d) {

    var M = getSolarMeanAnomaly(d),
        C = getEquationOfCenter(M),
        L = getEclipticLongitude(M, C);

    return {
        dec: getDeclination(L, 0),
        ra: getRightAscension(L, 0)
    };
}


var SunCalc = {};


// calculates sun position for a given date and latitude/longitude

SunCalc.getPosition = function (date, lat, lng) {

    var lw  = rad * -lng,
        phi = rad * lat,
        d   = toDays(date),

        c  = getSunCoords(d),
        H  = getSiderealTime(d, lw) - c.ra;

    return {
        azimuth: getAzimuth(H, phi, c.dec),
        altitude: getAltitude(H, phi, c.dec)
    };
};


// sun times configuration (angle, morning name, evening name)

var times = [
    [-0.83, 'sunrise',       'sunset'      ],
    [ -0.3, 'sunriseEnd',    'sunsetStart' ],
    [   -6, 'dawn',          'dusk'        ],
    [  -12, 'nauticalDawn',  'nauticalDusk'],
    [  -18, 'nightEnd',      'night'       ],
    [    6, 'goldenHourEnd', 'goldenHour'  ]
];

// adds a custom time to the times config

SunCalc.addTime = function (angle, riseName, setName) {
    times.push([angle, riseName, setName]);
};


// calculations for sun times

var J0 = 0.0009;

function getJulianCycle(d, lw) {
    return Math.round(d - J0 - lw / (2 * PI));
}
function getApproxTransit(Ht, lw, n) {
    return J0 + (Ht + lw) / (2 * PI) + n;
}
function getSolarTransitJ(ds, M, L) {
    return J2000 + ds + 0.0053 * sin(M) - 0.0069 * sin(2 * L);
}
function getHourAngle(h, phi, d) {
    return acos((sin(h) - sin(phi) * sin(d)) / (cos(phi) * cos(d)));
}


// calculates sun times for a given date and latitude/longitude

SunCalc.getTimes = function (date, lat, lng) {

    var lw  = rad * -lng,
        phi = rad * lat,
        d   = toDays(date),

        n  = getJulianCycle(d, lw),
        ds = getApproxTransit(0, lw, n),

        M = getSolarMeanAnomaly(ds),
        C = getEquationOfCenter(M),
        L = getEclipticLongitude(M, C),

        dec = getDeclination(L, 0),

        Jnoon = getSolarTransitJ(ds, M, L);


    // returns set time for the given sun altitude
    function getSetJ(h) {
        var w = getHourAngle(h, phi, dec),
            a = getApproxTransit(w, lw, n);

        return getSolarTransitJ(a, M, L);
    }


    var result = {
        solarNoon: fromJulian(Jnoon),
        nadir: fromJulian(Jnoon - 0.5)
    };

    var i, len, time, angle, morningName, eveningName, Jset, Jrise;

    for (i = 0, len = times.length; i < len; i += 1) {
        time = times[i];

        Jset = getSetJ(time[0] * rad);
        Jrise = Jnoon - (Jset - Jnoon);

        result[time[1]] = fromJulian(Jrise);
        result[time[2]] = fromJulian(Jset);
    }

    return result;
};


// moon calculations, based on http://aa.quae.nl/en/reken/hemelpositie.html formulas

function getMoonCoords(d) { // geocentric ecliptic coordinates of the moon

    var L = rad * (218.316 + 13.176396 * d), // ecliptic longitude
        M = rad * (134.963 + 13.064993 * d), // mean anomaly
        F = rad * (93.272 + 13.229350 * d),  // mean distance

        l  = L + rad * 6.289 * sin(M), // longitude
        b  = rad * 5.128 * sin(F),     // latitude
        dt = 385001 - 20905 * cos(M);  // distance to the moon in km

    return {
        ra: getRightAscension(l, b),
        dec: getDeclination(l, b),
        dist: dt
    };
}

SunCalc.getMoonPosition = function (date, lat, lng) {

    var lw  = rad * -lng,
        phi = rad * lat,
        d   = toDays(date),

        c = getMoonCoords(d),
        H = getSiderealTime(d, lw) - c.ra,
        h = getAltitude(H, phi, c.dec);

    // altitude correction for refraction
    h = h + rad * 0.017 / tan(h + rad * 10.26 / (h + rad * 5.10));

    return {
        azimuth: getAzimuth(H, phi, c.dec),
        altitude: h,
        distance: c.dist
    };
};


// calculations for illuminated fraction of the moon,
// based on http://idlastro.gsfc.nasa.gov/ftp/pro/astro/mphase.pro formulas

SunCalc.getMoonIllumination = function (date) {

    var d = toDays(date || new Date()),
        s = getSunCoords(d),
        m = getMoonCoords(d),

        sdist = 149598000, // distance from Earth to Sun in km

        phi = acos(sin(s.dec) * sin(m.dec) + cos(s.dec) * cos(m.dec) * cos(s.ra - m.ra)),
        inc = atan(sdist * sin(phi), m.dist - sdist * cos(phi)),
        angle = atan(cos(s.dec) * sin(s.ra - m.ra), sin(s.dec) * cos(m.dec) -
                cos(s.dec) * sin(m.dec) * cos(s.ra - m.ra));

    return {
        fraction: (1 + cos(inc)) / 2,
        phase: 0.5 + 0.5 * inc * (angle < 0 ? -1 : 1) / Math.PI,
        angle: angle
    };
};



var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function suncalcinfo (pos){
    //suncalc stuff

  var settings = JSON.parse(localStorage.getItem('clay-settings')) || {};
  var manuallat = settings.Lat;
  var manuallong = settings.Long;
  if(manuallat !== null && manuallat !== '' && manuallong !== null && manuallong !== '' ){
    var lat= manuallat;
    var lon= manuallong;
  }
  else {
    var lat=pos.coords.latitude;
    var lon= pos.coords.longitude;
  }
  //var lat=pos.coords.latitude;
  //var lon= pos.coords.longitude;
        var d = new Date();
        var sunTimes = SunCalc.getTimes(d, lat, lon);
        var sunsetraw = sunTimes.sunset.toTimeString().split(' ')[0];
        var sunriseraw = sunTimes.sunrise.toTimeString().split(' ')[0];
        var sunsetStrhr = ('0'+sunTimes.sunset.getHours()).substr(-2);
        var sunsetStrmin = ('0'+sunTimes.sunset.getMinutes()).substr(-2);
        var sunsetStr = String(sunsetStrhr + ":" + sunsetStrmin);
        var sunriseStrhr = ('0'+sunTimes.sunrise.getHours()).substr(-2);
        var sunriseStrmin = ('0'+sunTimes.sunrise.getMinutes()).substr(-2);
        var sunriseint = sunTimes.sunrise.getHours()*100+sunTimes.sunrise.getMinutes();
        var sunsetint = sunTimes.sunset.getHours()*100+sunTimes.sunset.getMinutes();
        //var sunsetint= String((sunsetStrhr*1) + sunsetStrmin);
        //var sunriseint= String((sunriseStrhr*1) + sunriseStrmin);
        var sunriseStr = String(sunriseStrhr + ":" + sunriseStrmin);
        var sunsetStrhr12 = parseInt(sunTimes.sunset.getHours());
        var sunriseStrhr12 = parseInt(sunTimes.sunrise.getHours());
        if(sunsetStrhr12 > 12 ){
          var sunsetStr12h = String (sunsetStrhr12 - 12 + ":" + sunsetStrmin);// +"pm");
          }
        else{
          var sunsetStr12h = String (sunsetStrhr12  + ":" + sunsetStrmin);// + "am");
          }
        if(sunriseStrhr > 12 ){
          var sunriseStr12h = String(sunriseStrhr12 - 12 + ":" + sunriseStrmin);// +"pm");
          }
        else{
          var sunriseStr12h = String(sunriseStrhr12  + ":" + sunriseStrmin);// + "am");
          }

        var moonmetrics = SunCalc.getMoonIllumination(d);
        var moonphase = Math.round(moonmetrics.phase*28);
   localStorage.setItem("OKAPI", 1);
    console.log("OK API");
    console.log(moonphase);
    console.log(d);
    console.log(lat);
    console.log(lon);
    console.log(sunsetStr);
    console.log(sunriseStr);
    console.log(sunriseint);
    console.log(sunsetint);
//    console.log(rightlefts);
    // Assemble dictionary
    var dictionary = {
      "HourSunrise":sunriseint,
      "HourSunset":sunsetint,
      "WEATHER_SUNSET_KEY":sunsetStr,
      "WEATHER_SUNRISE_KEY":sunriseStr,
      "WEATHER_SUNSET_KEY_12H":sunsetStr12h,
      "WEATHER_SUNRISE_KEY_12H":sunriseStr12h,
      "MoonPhase": moonphase,
    };
    // Send to Pebble
    Pebble.sendAppMessage(dictionary,function(e) {console.log("Suncalc stuff sent to Pebble successfully!");},
                                     function(e) {console.log("Error sending suncalc stuff to Pebble!");}
                                    );
  }

// Request for DarkSky

// Request for OWM

function locationError(err) {
  console.log("Error requesting geolocation!");
  //Send response null
  var location="";
  // Assemble dictionary using our keys
  var dictionary = {
    "NameLocation": location};
  Pebble.sendAppMessage(dictionary,
                        function(e) {
                          console.log("Null key sent to Pebble successfully!");
                        },
                        function(e) {
                          console.log("Null key error sending to Pebble!");
                        }
                       );
}

function getinfo() {
  // Get keys from pmkey
  var settings4 = JSON.parse(localStorage.getItem('clay-settings')) || {};
  var manuallat = settings4.Lat;
  var manuallong = settings4.Long;
  console.log("Ready from getinfo");
  if(manuallat != null && manuallat != '' && manuallong != null && manuallong != '' ){
      console.log(manuallat);
      console.log(manuallong);
      suncalcinfo();
      }
  else {
      navigator.geolocation.getCurrentPosition(
      suncalcinfo,
      locationError,
      {enableHighAccuracy:true,timeout: 15000, maximumAge: 1000}
    );
    navigator.geolocation.getCurrentPosition(
    suncalcinfo,
    locationError,
    {enableHighAccuracy:true,timeout: 15000, maximumAge: 1000}
  );
    }
  }


// Listen for when the watchface is opened
Pebble.addEventListener('ready',
                        function(e) {
                          console.log("Starting Watchface!");
                          localStorage.setItem("OKAPI", 0);
                          //get suncalc
                          //suncalcinfo();
                          // Get the initial weather
                          getinfo();
                        }
                       );
// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
                        function(e) {
                          console.log("Requesting geoposition!");
                        //  suncalcinfo();
                          getinfo();
                        }
                       );
// Listen for when the Config app changes
Pebble.addEventListener('webviewclosed',
                        function(e) {
                          console.log("Updating config!");
                          //suncalcinfo();
                          getinfo();
                        }
                       );


//functions and mappings
/*function unitsToStringOWM(unit) {
  if (unit) {
    return 'imperial';
  }
  return 'metric';
}*/


/*function rotation(rightleft) {
  if (rightleft) {
    return 'false';
  }
  return 'true';
}*/

function translate(langloc){
  if (langloc==='es-ES'){
    return 'es';
  }
  else if (langloc==='fr_FR'){
    return 'fr';
  }
  else if (langloc==='de_DE'){
    return 'de';
  }
  else if (langloc==='it_IT'){
    return 'it';
  }
  else if (langloc==='pt_PT'){
    return 'pt';
  }
  else {
    return 'en';
  }
}
function translatewu(langloc){
  if (langloc==='es-ES'){
    return 'SP';
  }
  else if (langloc==='fr_FR'){
    return 'FR';
  }
  else if (langloc==='de_DE'){
    return 'DL';
  }
  else if (langloc==='it_IT'){
    return 'IT';
  }
  else if (langloc==='pt_PT'){
    return 'BR';
  }
  else {
    return 'EN';
  }
}


function replaceDiacritics(s){
    var diacritics =[
        /[\300-\306]/g, /[\340-\346]/g,  // A, a
        /[\310-\313]/g, /[\350-\353]/g,  // E, e
        /[\314-\317]/g, /[\354-\357]/g,  // I, i
        /[\322-\330]/g, /[\362-\370]/g,  // O, o
        /[\331-\334]/g, /[\371-\374]/g,  // U, u
        /[\321]/g, /[\361]/g, // N, n
        /[\307]/g, /[\347]/g, // C, c
    ];

    var chars = ['A','a','E','e','I','i','O','o','U','u','N','n','C','c'];

    for (var i = 0; i < diacritics.length; i++)
    {
        s = s.replace(diacritics[i],chars[i]);
    }
  var end=s;
  return end;
}
