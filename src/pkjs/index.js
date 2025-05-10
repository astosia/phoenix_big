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
var owm_WindToId = {
    '0': 0,
    '1': 0,
    '2': 0,
    '3': 0,
    '4': 0,
    '5': 0,
    '6': 0,
    '7': 0,
    '8': 0,
    '9': 0,
    '10': 0,
    '11': 0,
    '12': 0,
    '13': 0,
    '14': 0,
    '15': 0,
    '16': 0,
    '17': 0,
    '18': 0,
    '19': 0,
    '20': 0,
    '21': 0,
    '22': 0,
    '23': 2,
    '24': 2,
    '25': 2,
    '26': 2,
    '27': 2,
    '28': 2,
    '29': 2,
    '30': 2,
    '31': 2,
    '32': 2,
    '33': 2,
    '34': 2,
    '35': 2,
    '36': 2,
    '37': 2,
    '38': 2,
    '39': 2,
    '40': 2,
    '41': 2,
    '42': 2,
    '43': 2,
    '44': 2,
    '45': 2,
    '46': 2,
    '47': 2,
    '48': 2,
    '49': 2,
    '50': 2,
    '51': 2,
    '52': 2,
    '53': 2,
    '54': 2,
    '55': 2,
    '56': 2,
    '57': 2,
    '58': 2,
    '59': 2,
    '60': 2,
    '61': 2,
    '62': 2,
    '63': 2,
    '64': 2,
    '65': 2,
    '66': 2,
    '67': 2,
    '68': 4,
    '69': 4,
    '70': 4,
    '71': 4,
    '72': 4,
    '73': 4,
    '74': 4,
    '75': 4,
    '76': 4,
    '77': 4,
    '78': 4,
    '79': 4,
    '80': 4,
    '81': 4,
    '82': 4,
    '83': 4,
    '84': 4,
    '85': 4,
    '86': 4,
    '87': 4,
    '88': 4,
    '89': 4,
    '90': 4,
    '91': 4,
    '92': 4,
    '93': 4,
    '94': 4,
    '95': 4,
    '96': 4,
    '97': 4,
    '98': 4,
    '99': 4,
    '100': 4,
    '101': 4,
    '102': 4,
    '103': 4,
    '104': 4,
    '105': 4,
    '106': 4,
    '107': 4,
    '108': 4,
    '109': 4,
    '110': 4,
    '111': 4,
    '112': 4,
    '113': 6,
    '114': 6,
    '115': 6,
    '116': 6,
    '117': 6,
    '118': 6,
    '119': 6,
    '120': 6,
    '121': 6,
    '122': 6,
    '123': 6,
    '124': 6,
    '125': 6,
    '126': 6,
    '127': 6,
    '128': 6,
    '129': 6,
    '130': 6,
    '131': 6,
    '132': 6,
    '133': 6,
    '134': 6,
    '135': 6,
    '136': 6,
    '137': 6,
    '138': 6,
    '139': 6,
    '140': 6,
    '141': 6,
    '142': 6,
    '143': 6,
    '144': 6,
    '145': 6,
    '146': 6,
    '147': 6,
    '148': 6,
    '149': 6,
    '150': 6,
    '151': 6,
    '152': 6,
    '153': 6,
    '154': 6,
    '155': 6,
    '156': 6,
    '157': 6,
    '158': 8,
    '159': 8,
    '160': 8,
    '161': 8,
    '162': 8,
    '163': 8,
    '164': 8,
    '165': 8,
    '166': 8,
    '167': 8,
    '168': 8,
    '169': 8,
    '170': 8,
    '171': 8,
    '172': 8,
    '173': 8,
    '174': 8,
    '175': 8,
    '176': 8,
    '177': 8,
    '178': 8,
    '179': 8,
    '180': 8,
    '181': 8,
    '182': 8,
    '183': 8,
    '184': 8,
    '185': 8,
    '186': 8,
    '187': 8,
    '188': 8,
    '189': 8,
    '190': 8,
    '191': 8,
    '192': 8,
    '193': 8,
    '194': 8,
    '195': 8,
    '196': 8,
    '197': 8,
    '198': 8,
    '199': 8,
    '200': 8,
    '201': 8,
    '202': 8,
    '203': 10,
    '204': 10,
    '205': 10,
    '206': 10,
    '207': 10,
    '208': 10,
    '209': 10,
    '210': 10,
    '211': 10,
    '212': 10,
    '213': 10,
    '214': 10,
    '215': 10,
    '216': 10,
    '217': 10,
    '218': 10,
    '219': 10,
    '220': 10,
    '221': 10,
    '222': 10,
    '223': 10,
    '224': 10,
    '225': 10,
    '226': 10,
    '227': 10,
    '228': 10,
    '229': 10,
    '230': 10,
    '231': 10,
    '232': 10,
    '233': 10,
    '234': 10,
    '235': 10,
    '236': 10,
    '237': 10,
    '238': 10,
    '239': 10,
    '240': 10,
    '241': 10,
    '242': 10,
    '243': 10,
    '244': 10,
    '245': 10,
    '246': 10,
    '247': 10,
    '248': 12,
    '249': 12,
    '250': 12,
    '251': 12,
    '252': 12,
    '253': 12,
    '254': 12,
    '255': 12,
    '256': 12,
    '257': 12,
    '258': 12,
    '259': 12,
    '260': 12,
    '261': 12,
    '262': 12,
    '263': 12,
    '264': 12,
    '265': 12,
    '266': 12,
    '267': 12,
    '268': 12,
    '269': 12,
    '270': 12,
    '271': 12,
    '272': 12,
    '273': 12,
    '274': 12,
    '275': 12,
    '276': 12,
    '277': 12,
    '278': 12,
    '279': 12,
    '280': 12,
    '281': 12,
    '282': 12,
    '283': 12,
    '284': 12,
    '285': 12,
    '286': 12,
    '287': 12,
    '288': 12,
    '289': 12,
    '290': 12,
    '291': 12,
    '292': 12,
    '293': 14,
    '294': 14,
    '295': 14,
    '296': 14,
    '297': 14,
    '298': 14,
    '299': 14,
    '300': 14,
    '301': 14,
    '302': 14,
    '303': 14,
    '304': 14,
    '305': 14,
    '306': 14,
    '307': 14,
    '308': 14,
    '309': 14,
    '310': 14,
    '311': 14,
    '312': 14,
    '313': 14,
    '314': 14,
    '315': 14,
    '316': 14,
    '317': 14,
    '318': 14,
    '319': 14,
    '320': 14,
    '321': 14,
    '322': 14,
    '323': 14,
    '324': 14,
    '325': 14,
    '326': 14,
    '327': 14,
    '328': 14,
    '329': 14,
    '330': 14,
    '331': 14,
    '332': 14,
    '333': 14,
    '334': 14,
    '335': 14,
    '336': 14,
    '337': 14,
    '338': 0,
    '339': 0,
    '340': 0,
    '341': 0,
    '342': 0,
    '343': 0,
    '344': 0,
    '345': 0,
    '346': 0,
    '347': 0,
    '348': 0,
    '349': 0,
    '350': 0,
    '351': 0,
    '352': 0,
    '353': 0,
    '354': 0,
    '355': 0,
    '356': 0,
    '357': 0,
    '358': 0,
    '359': 0,
    '360': 0,

};
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

  // Request for Open-Meteo
  function locationSuccessDS(pos){
        //Request OWM
      //  var lat=pos.coords.latitude;
      //  var lon= pos.coords.longitude;
        var settings3 = JSON.parse(localStorage.getItem('clay-settings')) || {};
        var useWeather = settings3.UseWeather;
        var manuallat = settings3.Lat;
        var manuallong = settings3.Long;
        if(manuallat != null && manuallat != '' && manuallong != null && manuallong != '' ){
          var lat= manuallat;
          var lon= manuallong;
        }
        else {
          var lat=pos.coords.latitude;
          var lon= pos.coords.longitude;
        }
              var d = new Date();
              var sunTimes = SunCalc.getTimes(d, lat, lon);
              var sunsetStrhr = ('0'+sunTimes.sunset.getHours()).substr(-2);
              var sunsetStrmin = ('0'+sunTimes.sunset.getMinutes()).substr(-2);
              var sunsetStr = String(sunsetStrhr + ":" + sunsetStrmin);
              var sunriseStrhr = ('0'+sunTimes.sunrise.getHours()).substr(-2);
              var sunriseStrmin = ('0'+sunTimes.sunrise.getMinutes()).substr(-2);
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
        var keyAPIds="OpenMeteo";
        var userKeyApi="OpenMeteo";
        var endapikey=apikeytouse(userKeyApi,keyAPIds);
        var units = unitsToString(settings3.WeatherUnit);
       // var unitsOWM=unitsToStringOWM(settings3.WeatherUnit);
        var windunits = windunitsToString(settings3.WindUnit);
      //  var rainunits = rainunitsToString(settings3.RainUnit);
        var langtouse=translate(navigator.language);
        // Construct URL
        var urlds = "https://api.open-meteo.com/v1/forecast?"+"latitude="
            + lat + "&longitude=" + lon +
            "&models=best_match&daily=weather_code,temperature_2m_max,temperature_2m_min,sunrise,sunset,uv_index_max,wind_direction_10m_dominant,wind_speed_10m_mean,precipitation_sum,precipitation_hours,precipitation_probability_mean&current=temperature_2m,precipitation,weather_code,wind_speed_10m,wind_direction_10m,is_day&forecast_days=1&timeformat=unixtime&wind_speed_unit=ms";

        console.log("DSUrl= " + urlds);
        // Send request to OpenWeatherMap
        xhrRequest(encodeURI(urlds), 'GET',function(responseText) {
          // responseText contains a JSON object with current weather info
          var json = JSON.parse(responseText);
          localStorage.setItem("OKAPI", 0);
          // Temperature
          var tempf = Math.round((json.current.temperature_2m * 9/5) + 32);//+'\xB0'+units;
          var tempc = Math.round(json.current.temperature_2m);
          var tempds=String(temptousewu(units,tempf,tempc))+'\xB0';
          var cityds = String((Math.round(lat*100))/100 + ',' + (Math.round(lon*100))/100);
          //var cityds = String(json.timezone);// Conditions
          var condds=json.current.weather_code;//description;
          //var condclean=replaceDiacritics(condds);
          var icon_ds = ds_iconToId[String(json.current.weather_code)+','+String(json.current.is_day)];
          // Sunrise and Sunset
          var auxsunds =new Date(json.daily.sunrise[0]*1000);
          var sunriseds=auxsunds.getHours()*100+auxsunds.getMinutes();
          var auxsetds =new Date(json.daily.sunset[0]*1000);
          var sunsetds=auxsetds.getHours()*100+auxsetds.getMinutes();
      //current conditions
          var windkts = Math.round(json.current.wind_speed_10m * 1.9438444924574);
          var windkph = Math.round(json.current.wind_speed_10m * 3.6);
          var windms = Math.round(json.current.wind_speed_10m);
          var windmph = Math.round(json.current.wind_speed_10m * 2.2369362920544);
          var wind = String(windtousewu(windunits,windkph,windmph,windms,windkts))+windunits;
          var windround = String(windtousewu(windunits,windkph,windmph,windms,windkts));//+windunits;
          var winddeg = String(json.current.wind_direction_10m);
          var winddir_num = owm_WindToId[winddeg];
      //forecast
          var forecast_icon_ds = ds_iconToId[String(json.daily.weather_code[0])+',1']; //always show forecast as daytime version
          var forecast_high_tempf = Math.round((json.daily.temperature_2m_max[0] * 9/5) +32);       //+'\xB0';
          var forecast_low_tempf = Math.round((json.daily.temperature_2m_min[0] * 9/5) +32);        //+'\xB0';
          var forecast_high_tempc = Math.round((json.daily.temperature_2m_max[0]));              //+ '\xB0';
          var forecast_low_tempc = Math.round((json.daily.temperature_2m_min[0]));              //+ '\xB0';
          var highds = String(temptousewu(units,forecast_high_tempf,forecast_high_tempc));
          var lowds = String(temptousewu(units,forecast_low_tempf,forecast_low_tempc));
          var highlowds = highds + '|'+ lowds+'\xB0';
          var forecast_ave_wind_mph = Math.round(json.daily.wind_speed_10m_mean[0] *2.2369362920544);
          var forecast_ave_wind_kts = Math.round(json.daily.wind_speed_10m_mean[0] *1.9438444924574);
          var forecast_ave_wind_kph = Math.round(json.daily.wind_speed_10m_mean[0] *3.6);
          var forecast_ave_wind_ms = Math.round(json.daily.wind_speed_10m_mean[0]);
          var forecast_wind_deg = String(json.daily.wind_direction_10m_dominant[0]);
          var forecast_wind_dir_num = owm_WindToId[forecast_wind_deg];
          var forecast_ave_wind_ds = String(windtousewu(windunits,forecast_ave_wind_kph,forecast_ave_wind_mph,forecast_ave_wind_ms,forecast_ave_wind_kts))+windunits;
          var forecast_ave_wind_ds_round = String(windtousewu(windunits,forecast_ave_wind_kph,forecast_ave_wind_mph,forecast_ave_wind_ms,forecast_ave_wind_kts));//+windunits;
          var auxtimeds =new Date(json.current.time*1000);
          var dstime =auxtimeds.getHours()*100+auxtimeds.getMinutes();
          //var precip_chance_next_hour=Math.round(json.daily.precipitation_probability_mean[0] *100);
          //var rain_chance_next_hour=String(precip_chance_next_hour)+'\x25';
          //var icon_next_hour = owm_iconToId[json.hourly[1].weather[0].icon];
          //var raintimeowm =new Date(json.minutely[0].dt*1000);
          //var raintimetaken=auxtimeowm.getHours()*100+auxtimeowm.getMinutes();
          //var raintimehr = ('0'+auxtimeowm.getHours()).substr(-2);
          //var raintimemin = ('0'+auxtimeowm.getMinutes()).substr(-2);
          //var raintimeStr24h = String(raintimehr + ":" + raintimemin);
          //var rainStrhr12 = parseInt(auxtimeowm.getHours());
          //if(rainStrhr12 > 12 ){
          //  var raintimeStr12h = String (rainStrhr12 - 12 + ":" + raintimemin);// +"pm");
          //  }
          //else{
          //  var raintimeStr12h = String (rainStrhr12  + ":" + raintimemin);// + "am");
          //  }

        //  var minutely = json.minutely;


      //    var rain_next_hour=Math.round(json.hourly[1].rain.1h);


          //console.log(minutely);
          console.log(icon_ds);
          console.log(forecast_icon_ds);
          console.log(condds);
          console.log(sunsetds);
          console.log(sunriseds);
          console.log(sunsetStr);
          console.log(sunriseStr);
          console.log(moonphase);
          console.log(wind);
          console.log(winddir_num);
          console.log(winddeg);
          console.log(forecast_ave_wind_ds);
          console.log(forecast_wind_dir_num);
          console.log(forecast_wind_deg);
          console.log(tempds);
          console.log(highds);
          console.log(lowds);
          console.log(highlowds);
          console.log(dstime);
          //console.log(icon_next_hour);
          //console.log(raintimetaken);
          //console.log(rain_chance_next_hour);

          localStorage.setItem("OKAPI", 1);
          console.log("OK API");

        //    xhrRequest(encodeURI(urlForecast), 'GET',function(forecastresponseText) {
            // forecastresponseText contains a JSON object with forecast weather info
        //    var jsonf = JSON.parse(forecastresponseText);
        //    localStorage.setItem("OKAPIForecast", 0);
              // ForOWMecast Conditions
          //              var condowm=jsonf.weather[0].main;//description;


          // Assemble dictionary using our keys
            var dictionary = {
            "WeatherTemp": tempds,
            "WeatherCond": condds,
            "HourSunset": sunsetds,
            "HourSunrise":sunriseds,
            "WeatherWind" : wind,
          //  "WeatherWindRound" : windround,
            "WEATHER_SUNSET_KEY":sunsetStr,
            "WEATHER_SUNRISE_KEY":sunriseStr,
            "WEATHER_SUNSET_KEY_12H":sunsetStr12h,
            "WEATHER_SUNRISE_KEY_12H":sunriseStr12h,
            "IconNow":icon_ds,
            "IconFore":forecast_icon_ds,
            "TempFore": highlowds,//hi_low,
            "TempForeLow": lowds,
            "WindFore": forecast_ave_wind_ds,
          //  "WindForeRound" : forecast_ave_wind_ds_round,
            "WindIconNow":winddir_num,
            "WindIconAve":forecast_wind_dir_num,
            "Weathertime":dstime,
            "MoonPhase": moonphase,
        //    "NameLocation": cityds
          //  "Cond1h": icon_next_hour,
          //  "pop1h": rain_chance_next_hour,
        //    "rain1h": rain_next_60,
          //  "raintime24h": raintimeStr24h,
          //  "raintime12h": raintimeStr12h,

          };

          // Send to Pebble
          Pebble.sendAppMessage(dictionary,
                                function(e) {console.log("Weather from Open-Meteo sent to Pebble successfully!");},
                                function(e) { console.log("Error sending Open-Meteo info to Pebble!");}
                               );
        //    });
        });
      }

// Request for OWM
function locationSuccessOWM(pos){
  //Request OWM
//  var lat=pos.coords.latitude;
//  var lon= pos.coords.longitude;
  var settings3 = JSON.parse(localStorage.getItem('clay-settings')) || {};
  var useWeather = settings3.UseWeather;
  var manuallat = settings3.Lat;
  var manuallong = settings3.Long;
  if(manuallat != null && manuallat != '' && manuallong != null && manuallong != '' ){
    var lat= manuallat;
    var lon= manuallong;
  }
  else {
    var lat=pos.coords.latitude;
    var lon= pos.coords.longitude;
  }
        var d = new Date();
        var sunTimes = SunCalc.getTimes(d, lat, lon);
        var sunsetStrhr = ('0'+sunTimes.sunset.getHours()).substr(-2);
        var sunsetStrmin = ('0'+sunTimes.sunset.getMinutes()).substr(-2);
        var sunsetStr = String(sunsetStrhr + ":" + sunsetStrmin);
        var sunriseStrhr = ('0'+sunTimes.sunrise.getHours()).substr(-2);
        var sunriseStrmin = ('0'+sunTimes.sunrise.getMinutes()).substr(-2);
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
  var keyAPIowm=localStorage.getItem('owmKey');
  var userKeyApi=settings3.APIKEY_User;
  var endapikey=apikeytouse(userKeyApi,keyAPIowm);
  var units = unitsToString(settings3.WeatherUnit);
 // var unitsOWM=unitsToStringOWM(settings3.WeatherUnit);
  var windunits = windunitsToString(settings3.WindUnit);
  var langtouse=translate(navigator.language);
  // Construct URL

  var urlOWM = "http://api.openweathermap.org/data/3.0/onecall?lat=" +
      lat + "&lon=" + lon +
      '&appid=' + endapikey + "&exclude=minutely,hourly,alerts" +
      '&lang='+langtouse;

  console.log("OWMUrl= " + urlOWM);
  // Send request to OpenWeatherMap
  xhrRequest(encodeURI(urlOWM), 'GET',function(responseText) {
    // responseText contains a JSON object with current weather info
    var json = JSON.parse(responseText);
    localStorage.setItem("OKAPI", 0);
    // Temperature
    var tempf = Math.round((json.current.temp * 1.8) - 459.67);//+'\xB0'+units;
    var tempc = Math.round(json.current.temp -273.15);
    var tempowm=String(temptousewu(units,tempf,tempc))+'\xB0';
    // Conditions
    var condowm=json.current.weather[0].description;//main;
    var condclean=replaceDiacritics(condowm);
    var icon_owm = String(json.current.weather[0].icon);
    var id_owm = parseInt(json.current.weather[0].id);
    var id_icon = String(id_owm)+String(icon_owm);
    if (id_owm>899) {
      var icon2_owm = owm_iconToId[String(id_owm)];
    } else {
      var icon2_owm = owm_iconToId[String(id_owm)+String(icon_owm)];
    }
    // Sunrise and Sunset
    var auxsunowm =new Date(json.current.sunrise*1000);
    var sunriseowm=auxsunowm.getHours()*100+auxsunowm.getMinutes();
    var auxsetowm =new Date(json.current.sunset*1000);
    var sunsetowm=auxsetowm.getHours()*100+auxsetowm.getMinutes();
//current conditions
    var windkts = Math.round(json.current.wind_speed * 1.9438444924574);
    var windkph = Math.round(json.current.wind_speed * 3.6);
    var windms = Math.round(json.current.wind_speed);
    var windmph = Math.round(json.current.wind_speed * 2.2369362920544);
    var wind = String(windtousewu(windunits,windkph,windmph,windms,windkts))+windunits;
    var winddeg = String(json.current.wind_deg);
    var winddir_num = owm_WindToId[winddeg];
//forecast
    var forecondowm=json.daily[0].weather[0].description;//main;
    var forecondclean=replaceDiacritics(forecondowm);
    var forecast_icon_owm = json.daily[0].weather[0].icon;
    var forecast_id_owm = parseInt(json.daily[0].weather[0].id);
    if (forecast_id_owm>899) {
      var forecast_icon2_owm = owm_iconToId[String(forecast_id_owm)];
    } else {
      var forecast_icon2_owm = owm_iconToId[String(forecast_id_owm)+String(forecast_icon_owm)];
    }
    var forecast_high_tempf = Math.round((json.daily[0].temp.max* 1.8) - 459.67);       //+'\xB0';
    var forecast_low_tempf = Math.round((json.daily[0].temp.min* 1.8) - 459.67);        //+'\xB0';
    var forecast_high_tempc = Math.round(json.daily[0].temp.max - 273.15);              //+ '\xB0';
    var forecast_low_tempc = Math.round(json.daily[0].temp.min - 273.15);              //+ '\xB0';
    var highowm = String(temptousewu(units,forecast_high_tempf,forecast_high_tempc));
    var lowowm = String(temptousewu(units,forecast_low_tempf,forecast_low_tempc));
    var highlowowm = highowm + '|'+ lowowm;//+'\xB0';
    var forecast_ave_wind_mph = Math.round(json.daily[0].wind_speed*2.2369362920544);
    var forecast_ave_wind_kts = Math.round(json.daily[0].wind_speed *1.9438444924574);
    var forecast_ave_wind_kph = Math.round(json.daily[0].wind_speed *3.6);
    var forecast_ave_wind_ms = Math.round(json.daily[0].wind_speed);
    var forecast_wind_deg = String(json.daily[0].wind_deg);
    var forecast_wind_dir_num = owm_WindToId[forecast_wind_deg];
    var forecast_ave_wind_owm = String(windtousewu(windunits,forecast_ave_wind_kph,forecast_ave_wind_mph,forecast_ave_wind_ms,forecast_ave_wind_kts))+windunits;
    var auxtimeowm =new Date(json.current.dt*1000);
    var owmtime =auxtimeowm.getHours()*100+auxtimeowm.getMinutes();


    console.log(condclean);
    console.log(sunsetowm);
    console.log(sunriseowm);
    console.log(wind);
    console.log(winddir_num);
    console.log(tempowm);
    console.log(icon_owm);
    console.log(id_owm);
    console.log(id_icon);
    console.log(icon2_owm);
    console.log(highowm);
    console.log(forecondclean);
    console.log(forecast_icon2_owm);
    console.log(lowowm);
    console.log(highlowowm);
    console.log(forecast_wind_dir_num);
    console.log(forecast_ave_wind_owm);
    console.log(sunsetStr);
    console.log(sunriseStr);
    console.log(moonphase);
    console.log(winddeg);
    console.log(forecast_wind_deg);
    console.log(owmtime);

   // var wind = String(windkts + "kts");
    localStorage.setItem("OKAPI", 1);
    console.log("OK API");

  //    xhrRequest(encodeURI(urlForecast), 'GET',function(forecastresponseText) {
      // forecastresponseText contains a JSON object with forecast weather info
  //    var jsonf = JSON.parse(forecastresponseText);
  //    localStorage.setItem("OKAPIForecast", 0);
        // Forecast Conditions
    //              var condowm=jsonf.weather[0].main;//description;


    // Assemble dictionary using our keys
    var dictionary = {
      "WeatherTemp": tempowm,
      "WeatherCond": condclean,
      "HourSunset": sunsetowm,
      "HourSunrise":sunriseowm,
      "WeatherWind" : wind,
      "WEATHER_SUNSET_KEY":sunsetStr,
      "WEATHER_SUNRISE_KEY":sunriseStr,
      "WEATHER_SUNSET_KEY_12H":sunsetStr12h,
      "WEATHER_SUNRISE_KEY_12H":sunriseStr12h,
      "IconNow":icon2_owm,
      "IconFore":forecast_icon2_owm,
      "TempFore": highlowowm,//hi_low,
      "TempForeLow": lowowm,
      "WindFore": forecast_ave_wind_owm,
      "WindIconNow":winddir_num,
      "WindIconAve":forecast_wind_dir_num,
      "Weathertime":owmtime,
      "MoonPhase": moonphase,

    };
    // Send to Pebble
    Pebble.sendAppMessage(dictionary,
                          function(e) {console.log("Weather from OWM sent to Pebble successfully!");},
                          function(e) { console.log("Error sending OWM info to Pebble!");}
                         );
  //    });
  });
}


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
  var settings4 = JSON.parse(localStorage.getItem('clay-settings')) || {};
  var manuallat = settings4.Lat;
  var manuallong = settings4.Long;
  // Get keys from pmkey
//  if (email !== undefined && pin !== undefined) {
//    //Request API from pmkey.xyz
//    var urlpmk='https://pmkey.xyz/search/?email='+email+"&pin="+pin;
//    console.log("Url PMKEY is "+ urlpmk);
//    var keys = parseInt(localStorage.getItem("OKAPI"));
//    console.log("Flag keys is " + keys);
//    if (keys===0){
//      xhrRequest(encodeURI(urlpmk),'GET',
//                 function(responseText){
//                   var jsonpmk=JSON.parse(responseText);
//                   var owmKey=jsonpmk.keys.weather.owm;
//                   var dsKey=jsonpmk.keys.weather.forecast;
//                   localStorage.setItem("owmKey", owmKey);
//                  }
//                );
//    }
//  }
  var weatherprov=settings4.WeatherProv;

  if (weatherprov=="ds"){
    console.log("Ready from Open-Meteo");
    if(manuallat != null && manuallat != '' && manuallong != null && manuallong != '' ){
      console.log(manuallat);
      console.log(manuallong);
      suncalcinfo();
      locationSuccessDS();
    }
    else {
    navigator.geolocation.getCurrentPosition(
      suncalcinfo,
      locationError,
      {enableHighAccuracy:true,timeout: 15000, maximumAge: 1000}
    );
    navigator.geolocation.getCurrentPosition(
      locationSuccessDS,
      locationError,
      {enableHighAccuracy:true,timeout: 15000, maximumAge: 1000}
    );
    }
  }
  else if (weatherprov=="owm"){
    console.log("Ready from OWM");
    if(manuallat != null && manuallat != '' && manuallong != null && manuallong != '' ){
      console.log(manuallat);
      console.log(manuallong);
      suncalcinfo();
      locationSuccessOWM();
    }
    else {
    navigator.geolocation.getCurrentPosition(
      suncalcinfo,
      locationError,
      {enableHighAccuracy:true,timeout: 15000, maximumAge: 1000}
    );
    navigator.geolocation.getCurrentPosition(
      locationSuccessOWM,
      locationError,
      {enableHighAccuracy:true,timeout: 15000, maximumAge: 1000}
    );
    }
  }
  else
    {
    console.log("no weather provider");
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
      locationError,
      {enableHighAccuracy:true,timeout: 15000, maximumAge: 1000}
    );
    }
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

function unitsToString(unit) {
 if (unit) {
   return 'F';
 }
 return 'C';
}
function windunitsToString(windunit){
 if (windunit=='kts') {
   return 'kt';
 }
 else if (windunit=='kph'){
   return 'kph';
 }
 else if (windunit=='ms'){
   return 'ms';
 }
 return 'mph';
 }
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

function temptousewu(unit,tempf,tempc){
  if (unit=="F"){
    return tempf; }
  else return tempc;
}
function windtousewu(windunit,windkph,windmph,windms,windkts){
  if (windunit=="kph"){
    return windkph; }
  else if (windunit=="mph")
    {return windmph; }
  else if (windunit=="ms")
    {return windms; }
  else return windkts;
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
function apikeytouse(APIUser,APIPMKEY){
  if (APIUser===""){
    console.log("Using pmkey");
    return APIPMKEY;
  }
  else {
    console.log("Using Key User");
    return APIUser;
  }
}
