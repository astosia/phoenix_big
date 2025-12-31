var Clay = require('pebble-clay');
var clayConfig = require('./config');

// --- SunCalc Library Functions ---
var PI   = Math.PI, sin = Math.sin, cos = Math.cos, tan = Math.tan,
    asin = Math.asin, atan = Math.atan2, acos = Math.acos, rad = PI / 180;
var dayMs = 1000 * 60 * 60 * 24, J1970 = 2440588, J2000 = 2451545;

function toJulian(date) { return date.valueOf() / dayMs - 0.5 + J1970; }
function toDays(date) { return toJulian(date) - J2000; }
var e = rad * 23.4397; 
function getRightAscension(l, b) { return atan(sin(l) * cos(e) - tan(b) * sin(e), cos(l)); }
function getDeclination(l, b) { return asin(sin(b) * cos(e) + cos(b) * sin(e) * sin(l)); }
function getSolarMeanAnomaly(d) { return rad * (357.5291 + 0.98560028 * d); }
function getEquationOfCenter(M) { return rad * (1.9148 * sin(M) + 0.02 * sin(2 * M) + 0.0003 * sin(3 * M)); }
function getEclipticLongitude(M, C) { var P = rad * 102.9372; return M + C + P + PI; }
function getSunCoords(d) {
    var M = getSolarMeanAnomaly(d), C = getEquationOfCenter(M), L = getEclipticLongitude(M, C);
    return { dec: getDeclination(L, 0), ra: getRightAscension(L, 0) };
}
var J0 = 0.0009;
function getJulianCycle(d, lw) { return Math.round(d - J0 - lw / (2 * PI)); }
function getApproxTransit(Ht, lw, n) { return J0 + (Ht + lw) / (2 * PI) + n; }
function getSolarTransitJ(ds, M, L) { return J2000 + ds + 0.0053 * sin(M) - 0.0069 * sin(2 * L); }
function getHourAngle(h, phi, d) { return acos((sin(h) - sin(phi) * sin(d)) / (cos(phi) * cos(d))); }
function getMoonCoords(d) {
    var L = rad * (218.316 + 13.176396 * d), M = rad * (134.963 + 13.064993 * d),
        F = rad * (93.272 + 13.229350 * d), l  = L + rad * 6.289 * sin(M),
        b  = rad * 5.128 * sin(F), dt = 385001 - 20905 * cos(M);
    return { ra: getRightAscension(l, b), dec: getDeclination(l, b), dist: dt };
}
var getMoonIllumination = function (date) {
    var d = toDays(date || new Date()), s = getSunCoords(d), m = getMoonCoords(d), sdist = 149598000,
        phi = acos(sin(s.dec) * sin(m.dec) + cos(s.dec) * cos(m.dec) * cos(s.ra - m.ra)),
        inc = atan(sdist * sin(phi), m.dist - sdist * cos(phi)),
        angle = atan(cos(s.dec) * sin(s.ra - m.ra), sin(s.dec) * cos(m.dec) - cos(s.dec) * sin(m.dec) * cos(s.ra - m.ra));
    return { fraction: (1 + cos(inc)) / 2, phase: 0.5 + 0.5 * inc * (angle < 0 ? -1 : 1) / Math.PI };
};
var times = [ [-0.83, 'sunrise', 'sunset'] ];
var getTimes = function (date, lat, lng) {
    var lw  = rad * -lng, phi = rad * lat, d = toDays(date), n = getJulianCycle(d, lw), ds = getApproxTransit(0, lw, n),
        M = getSolarMeanAnomaly(ds), C = getEquationOfCenter(M), L = getEclipticLongitude(M, C), dec = getDeclination(L, 0),
        Jnoon = getSolarTransitJ(ds, M, L);
    function getSetJ(h) {
        var w = getHourAngle(h, phi, dec), a = getApproxTransit(w, lw, n);
        return getSolarTransitJ(a, M, L);
    }
    var result = { solarNoon: new Date((Jnoon + 0.5 - J1970) * dayMs) };
    for (var i = 0; i < times.length; i += 1) {
        var time = times[i], Jset = getSetJ(time[0] * rad), Jrise = Jnoon - (Jset - Jnoon);
        result[time[1]] = new Date((Jrise + 0.5 - J1970) * dayMs);
        result[time[2]] = new Date((Jset + 0.5 - J1970) * dayMs);
    }
    return result;
};

// --- Mappings ---
var owm_iconToId = {
  '20011d':1, //thunderstorm with light rain
  '20011n':2, //thunderstorm with light rain
  '20111d':3, //thunderstorm with rain
  '20111n':4, //thunderstorm with rain
  '20211d':5, //thunderstorm with heavy rain
  '20211n':6, //thunderstorm with heavy rain
  '21011d':7, //light thunderstorm
  '21011n':8, //light thunderstorm
  '21111d':9, //thunderstorm
  '21111n':10, //thunderstorm
  '21211d':11, //heavy thunderstorm
  '21211n':12, //heavy thunderstorm
  '22111d':13, //ragged thunderstorm
  '22111n':14, //ragged thunderstorm
  '23011d':15, //thunderstorm with light drizzle
  '23011n':16, //thunderstorm with light drizzle
  '23111d':17, //thunderstorm with drizzle
  '23111n':18, //thunderstorm with drizzle
  '23211d':19, //thunderstorm with heavy drizzle
  '23211n':20, //thunderstorm with heavy drizzle
  '30009d':21, //light intensity drizzle
  '30009n':22, //light intensity drizzle
  '30109d':23, //drizzle
  '30109n':24, //drizzle
  '30209d':25, //heavy intensity drizzle
  '30209n':26, //heavy intensity drizzle
  '31009d':27, //light intensity drizzle rain
  '31009n':28, //light intensity drizzle rain
  '31109d':29, //drizzle rain
  '31109n':30, //drizzle rain
  '31209d':31, //heavy intensity drizzle rain
  '31209n':32, //heavy intensity drizzle rain
  '31309d':33, //shower rain and drizzle
  '31309n':34, //shower rain and drizzle
  '31409d':35, //heavy shower rain and drizzle
  '31409n':36, //heavy shower rain and drizzle
  '32109d':37, //shower drizzle
  '32109n':38, //shower drizzle
  '50010d':39, //light rain
  '50010n':40, //light rain
  '50110d':41, //moderate rain
  '50110n':42, //moderate rain
  '50210d':43, //heavy intensity rain
  '50210n':44, //heavy intensity rain
  '50310d':45, //very heavy rain
  '50310n':46, //very heavy rain
  '50410d':47, //extreme rain
  '50410n':48, //extreme rain
  '51113d':49, //freezing rain
  '51113n':50, //freezing rain
  '52009d':51, //light intensity shower rain
  '52009n':52, //light intensity shower rain
  '52109d':53, //shower rain
  '52109n':54, //shower rain
  '52209d':55, //heavy intensity shower rain
  '52209n':56, //heavy intensity shower rain
  '53109d':57, //ragged shower rain
  '53109n':58, //ragged shower rain
  '60013d':59, //light snow
  '60013n':60, //light snow
  '60113d':61, //Snow
  '60113n':62, //Snow
  '60213d':63, //Heavy snow
  '60213n':64, //Heavy snow
  '61113d':65, //Sleet
  '61113n':66, //Sleet
  '61213d':67, //Light shower sleet
  '61213n':68, //Light shower sleet
  '61313d':69, //Shower sleet
  '61313n':70, //Shower sleet
  '61513d':71, //Light rain and snow
  '61513n':72, //Light rain and snow
  '61613d':73, //Rain and snow
  '61613n':74, //Rain and snow
  '62013d':75, //Light shower snow
  '62013n':76, //Light shower snow
  '62113d':77, //Shower snow
  '62113n':78, //Shower snow
  '62213d':79, //Heavy shower snow
  '62213n':80, //Heavy shower snow
  '70150d':81, //mist
  '70150n':82, //mist
  '71150d':83, //Smoke
  '71150n':84, //Smoke
  '72150d':85, //Haze
  '72150n':86, //Haze
  '73150d':87, //sand/ dust whirls
  '73150n':88, //sand/ dust whirls
  '74150d':89, //fog
  '74150n':90, //fog
  '75150d':91, //sand
  '75150n':92, //sand
  '76150d':93, //dust
  '76150n':94, //dust
  '76250d':95, //volcanic ash
  '76250n':96, //volcanic ash
  '77150d':97, //squalls
  '77150n':98, //squalls
  '78150d':99, //tornado
  '78150n':100, //tornado
  '80001d':101, //clear sky
  '80001n':102, //clear sky
  '80102d':103, //few clouds: 11-25%
  '80102n':104, //few clouds: 11-25%
  '80203d':105, //scattered clouds: 25-50%
  '80203n':106, //scattered clouds: 25-50%
  '80304d':107, //broken clouds: 51-84%
  '80304n':108, //broken clouds: 51-84%
  '80404d':109, //overcast clouds: 85-100%
  '80404n':110, //overcast clouds: 85-100%
  '900':111, //tornado
  '901':112, //storm-showers
  '902':113, //hurricane
  '903':114, //snowflake-cold
  '904':115, //hot
  '905':116, //windy
  '906':117, //hail
  '957':118, //strong-wind
};

///changed from dark sky to open-meteo
var ds_iconToId = {
  //daytime
    '0,1': 101, //0 = clear sky ok
    '1,1': 103, //1 = Mainly Clear ok
    '2,1': 105, //2 = partly cloudy ok
    '3,1': 110, //3 = Overcast ok
    '55,1': 34, //55 = Drizzle dense
    '57,1': 34, //57 = Freezing drizzle dense
    '61,1': 34, //61 = Slight Rain
    '80,1': 34, //80 = Slight Rain showers
    '63,1': 46, //63 = Moderate Rain
    '81,1': 46, //81 = Moderate Rain showers
    '73,1': 114, //73 = Moderate Snow
    '75,1': 63, //75 = Heavy Snow
    '86,1': 79, //86 = Heavy Snow showers
    '95,1': 17, //95 = Slight or moderate thunderstorm
    '45,1': 90, //45 = Fog
    '48,1': 90, //48 = Depositing rime fog (freezing fog)
    '51,1': 37, //51 = Drizzle light
    '53,1': 24, //53 = Drizzle moderate
    '56,1': 24, //56 = Freezing drizzle light
    '65,1': 46, //65 = Heavy Rain
    '82,1': 58, //82 = Violent Rain showers
    '66,1': 67, //66 = Light Freezing rain (Sleet)
    '67,1': 66, //67 = Heavy Freezing rain   (sleet)
    '71,1': 77, //71 = Slight Snow
    '77,1': 117, //77 = Snow grains (hail?)
    '85,1': 77, //85 = Slight Snow showers
    '96,1': 20, //96 = Thunderstorm with slight hail
    '99,1': 20, //99 = Thunderstorn with heavy hail
    //night
    '0,0': 102, //0 = clear sky ok
    '1,0': 104, //1 = Mainly Clear ok
    '2,0': 106, //2 = partly cloudy ok
    '3,0': 110, //3 = Overcast ok
    '55,0': 34, //55 = Drizzle dense
    '57,0': 34, //57 = Freezing drizzle dense
    '61,0': 40, //61 = Slight Rain
    '80,0': 40, //80 = Slight Rain showers
    '63,0': 46, //63 = Moderate Rain
    '81,0': 46, //81 = Moderate Rain showers
    '73,0': 114, //73 = Moderate Snow
    '75,0': 63, //75 = Heavy Snow
    '86,0': 79, //86 = Heavy Snow showers
    '95,0': 2, //95 = Slight or moderate thunderstorm
    '45,0': 90, //45 = Fog
    '48,0': 90, //48 = Depositing rime fog (freezing fog)
    '51,0': 38, //51 = Drizzle light
    '53,0': 24, //53 = Drizzle moderate
    '56,0': 24, //56 = Freezing drizzle light
    '65,0': 46, //65 = Heavy Rain
    '82,0': 58, //82 = Violent Rain showers
    '66,0': 68, //66 = Light Freezing rain (Sleet)
    '67,0': 66, //67 = Heavy Freezing rain   (sleet)
    '71,0': 78, //71 = Slight Snow
    '77,0': 117, //77 = Snow grains (hail?)
    '85,0': 78, //85 = Slight Snow showers
    '96,0': 20, //96 = Thunderstorm with slight hail
    '99,0': 20, //99 = Thunderstorn with heavy hail
};

// --- Helpers ---
function mergeDicts(t, s) { for (var k in s) { if (Object.prototype.hasOwnProperty.call(s, k)) t[k] = s[k]; } return t; }

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () { callback(null, this.responseText); };
  xhr.onerror = function () { callback("Network Error"); };
  xhr.open(type, url);
  xhr.send();
};

function unitsToString(unit) { return unit ? 'F' : 'C'; }
function translate(langloc){
  var l = langloc.substring(0,2);
  var supported = ['es', 'fr', 'de', 'it', 'pt'];
  return supported.indexOf(l) !== -1 ? l : 'en';
}
function temptousewu(u, f, c){ return u == "F" ? f : c; }

function replaceDiacritics(s){
    var diacritics = [/[\300-\306]/g, /[\340-\346]/g, /[\310-\313]/g, /[\350-\353]/g, /[\314-\317]/g, /[\354-\357]/g, /[\322-\330]/g, /[\362-\370]/g, /[\331-\334]/g, /[\371-\374]/g, /[\321]/g, /[\361]/g, /[\307]/g, /[\347]/g];
    var chars = ['A','a','E','e','I','i','O','o','U','u','N','n','C','c'];
    for (var i = 0; i < diacritics.length; i++) s = s.replace(diacritics[i], chars[i]);
    return s;
}

function formatTime(date) {
  var hr = ('0' + date.getHours()).substr(-2);
  var min = ('0' + date.getMinutes()).substr(-2);
  var hr12 = date.getHours() % 12 || 12;
  return {
    int: date.getHours() * 100 + date.getMinutes(),
    str24h: hr + ":" + min,
    str12h: hr12 + ":" + min
  };
}

// --- Logic ---

function getAstroData(pos) {
    var settings = JSON.parse(localStorage.getItem('clay-settings')) || {};
    var lat, lon;
    var loc_source;
    if (settings.Lat && settings.Long) {
        lat = settings.Lat; 
        lon = settings.Long;
        loc_source = "manual";
    } else if (pos) {
        lat = pos.coords.latitude; 
        lon = pos.coords.longitude;
        localStorage.setItem('lat', lat); 
        localStorage.setItem('lon', lon);
        loc_source = "GPS";
    } else {
        lat = localStorage.getItem('lat'); 
        lon = localStorage.getItem('lon');
        loc_source = "Last known";
    }

    if (!lat || !lon) return null;

    console.log(loc_source);

    var d = new Date();
    var sunTimes = getTimes(d, lat, lon);
    var sr = formatTime(sunTimes.sunrise);
    var ss = formatTime(sunTimes.sunset);
    var moon = getMoonIllumination(d);
    var moonphase = Math.round(moon.phase * 28);

    return {
        lat: lat, lon: lon,
        dictionary: {
            "HourSunrise": sr.int, 
            "HourSunset": ss.int,
            "WEATHER_SUNSET_KEY": ss.str24h, 
            "WEATHER_SUNRISE_KEY": sr.str24h,
            "WEATHER_SUNSET_KEY_12H": ss.str12h, 
            "WEATHER_SUNRISE_KEY_12H": sr.str12h,
            "MoonPhase": moonphase
        }
    };
}

function fetchWeatherData(pos) {
    var astro = getAstroData(pos);
    if (!astro) return;

    var settings = JSON.parse(localStorage.getItem('clay-settings')) || {};
    var weatherOn = settings.WeatherOn;
    var sunsetOn = settings.SunsetOn;
    var ForecastWeatherOn = settings.ForecastWeatherOn;
    var weatherProv = settings.WeatherProv;

    // 1. If everything is off, exit.
    if (!weatherOn && !ForecastWeatherOn && !sunsetOn) return;

    // 2. If ONLY Suncalc is on, send Astro and stop.
    if (!WeatherOn && !ForecastWeatherOn && SunsetOn) {
        Pebble.sendAppMessage(astro.dictionary);
        return;
    }

    // 3. Prepare Weather API
    var units = unitsToString(settings.WeatherUnit);
    var url, parseFunc;

    if (weatherProv === 'ds') {
        url = "https://api.open-meteo.com/v1/forecast?latitude=" + astro.lat + "&longitude=" + astro.lon + "&current=temperature_2m,weather_code,is_day&daily=weather_code,temperature_2m_max,temperature_2m_min&forecast_days=1&timeformat=unixtime";
        parseFunc = function(j) {
            return {
                "WeatherTemp": temptousewu(units, Math.round((j.current.temperature_2m * 9/5) + 32), Math.round(j.current.temperature_2m)) + '\xB0',
                "IconNow": ds_iconToId[j.current.weather_code + ',' + j.current.is_day],
                "IconFore": ds_iconToId[j.daily.weather_code[0] + ',1'],
                "TempFore": Math.round(temptousewu(units, (j.daily.temperature_2m_max[0]*1.8)+32, j.daily.temperature_2m_max[0])) + '|' + Math.round(temptousewu(units, (j.daily.temperature_2m_min[0]*1.8)+32, j.daily.temperature_2m_min[0]))
            };
        };
    } else if (weatherProv === 'owm') {
        var key = settings.APIKEY_User || localStorage.getItem('owmKey');
        url = "http://api.openweathermap.org/data/3.0/onecall?lat=" + astro.lat + "&lon=" + astro.lon + "&appid=" + key + "&exclude=minutely,hourly,alerts&lang=" + translate(navigator.language);
        parseFunc = function(j) {
            var icon = j.current.weather[0].id > 899 ? j.current.weather[0].id : j.current.weather[0].id + j.current.weather[0].icon;
            return {
                "WeatherTemp": temptousewu(units, Math.round((j.current.temp * 1.8) - 459.67), Math.round(j.current.temp - 273.15)) + '\xB0',
                "WeatherCond": replaceDiacritics(j.current.weather[0].description),
                "IconNow": owm_iconToId[icon]
            };
        };
    }

    xhrRequest(encodeURI(url), 'GET', function(err, data) {
        if (err) {
            Pebble.sendAppMessage(astro.dictionary); // Fallback to astro only
            return;
        }
        try {
            var res = parseFunc(JSON.parse(data));
            Pebble.sendAppMessage(mergeDicts(astro.dictionary, res));
        } catch(e) {
            Pebble.sendAppMessage(astro.dictionary);
        }
    });
}

function getinfo() {
    var settings = JSON.parse(localStorage.getItem('clay-settings')) || {};
    if (settings.Lat && settings.Long) {
        fetchWeatherData(null);
    } else {
        navigator.geolocation.getCurrentPosition(fetchWeatherData, 
            function() { Pebble.sendAppMessage({"NameLocation": ""}); }, 
            { enableHighAccuracy: true, timeout: 15000, maximumAge: 1000 });
    }
}

// --- Events ---
Pebble.addEventListener('ready', getinfo);
Pebble.addEventListener('appmessage', getinfo);
Pebble.addEventListener('webviewclosed', getinfo);