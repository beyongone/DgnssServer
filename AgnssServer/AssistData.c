//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>
#include "AssistData.h"
#include <string.h>  
//#include <WTypesbase.h>
#include <windows.h>  
#include <math.h>
#ifndef WIN32
#include <dirent.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#else
#pragma comment( lib,"winmm.lib" )
#endif

#define NLBS_GPSEPH       ("#GPSEPHEMA")
#define NLBS_BDSEPH       ("#BD2EPHEMA")
#define MXTSTR_GNTXT       ("$GNTXT")

#define OEM4SYNC1   0xAA        /* oem4 message start sync code 1 */
#define OEM4SYNC2   0x44        /* oem4 message start sync code 2 */
#define OEM4SYNC3   0x12        /* oem4 message start sync code 3 */
//////////////////////////////////////////////////////////////////////////
#define ID_RANGE    43          /* message id: oem4 range measurement */
/* get fields (little-endian) ------------------------------------------------*/
#define U1(p) (*((unsigned char *)(p)))
#define I1(p) (*((char *)(p)))

const static double gpst0[] = { 1980,1, 6,0,0,0 }; /* gps time reference */
const static double bdt0[] = { 2006,1, 1,0,0,0 }; /* beidou time reference */

#define DTTOL       0.005               /* tolerance of time difference (s) */

#define _PI  ( 3.1415926535897932384626433832795028841971 )
/* constants -----------------------------------------------------------------*/
#define POLYCRC32   0xEDB88320u /* CRC32 polynomial */
#define MAXLEAPS    64                  /* max number of leap seconds table */

#define OEM4HLEN    28          /* oem4 message header length (bytes) */

#define SYS_NONE    0x00                /* navigation system: none */
#define SYS_GPS     0x01                /* navigation system: GPS */
#define SYS_CMP     0x20                /* navigation system: BeiDou */
#define SYS_GLO     0x04                /* navigation system: GLONASS */
#define SYS_GAL     0x08                /* navigation system: Galileo */
#define SYS_QZS     0x10                /* navigation system: QZSS */

#define SYS_ALL     0xFF                /* navigation system: all */

#define CODE_NONE   0                   /* obs code: none or unknown */
#define CODE_L1C    1                   /* obs code: L1C/A,G1C/A,E1C (GPS,GLO,GAL,QZS,SBS) */
#define CODE_L1P    2                   /* obs code: L1P,G1P    (GPS,GLO) */
#define CODE_L1W    3                   /* obs code: L1 Z-track (GPS) */
#define CODE_L1Y    4                   /* obs code: L1Y        (GPS) */
#define CODE_L1M    5                   /* obs code: L1M        (GPS) */
#define CODE_L1N    6                   /* obs code: L1codeless (GPS) */
#define CODE_L1S    7                   /* obs code: L1C(D)     (GPS,QZS) */
#define CODE_L1L    8                   /* obs code: L1C(P)     (GPS,QZS) */
#define CODE_L1E    9                   /* obs code: L1-SAIF    (QZS) */
#define CODE_L1A    10                  /* obs code: E1A        (GAL) */
#define CODE_L1B    11                  /* obs code: E1B        (GAL) */
#define CODE_L1X    12                  /* obs code: E1B+C,L1C(D+P) (GAL,QZS) */
#define CODE_L1Z    13                  /* obs code: E1A+B+C,L1SAIF (GAL,QZS) */
#define CODE_L2C    14                  /* obs code: L2C/A,G1C/A (GPS,GLO) */
#define CODE_L2D    15                  /* obs code: L2 L1C/A-(P2-P1) (GPS) */
#define CODE_L2S    16                  /* obs code: L2C(M)     (GPS,QZS) */
#define CODE_L2L    17                  /* obs code: L2C(L)     (GPS,QZS) */
#define CODE_L2X    18                  /* obs code: L2C(M+L),B1I+Q (GPS,QZS,CMP) */
#define CODE_L2P    19                  /* obs code: L2P,G2P    (GPS,GLO) */
#define CODE_L2W    20                  /* obs code: L2 Z-track (GPS) */
#define CODE_L2Y    21                  /* obs code: L2Y        (GPS) */
#define CODE_L2M    22                  /* obs code: L2M        (GPS) */
#define CODE_L2N    23                  /* obs code: L2codeless (GPS) */
#define CODE_L5I    24                  /* obs code: L5/E5aI    (GPS,GAL,QZS,SBS) */
#define CODE_L5Q    25                  /* obs code: L5/E5aQ    (GPS,GAL,QZS,SBS) */
#define CODE_L5X    26                  /* obs code: L5/E5aI+Q  (GPS,GAL,QZS,SBS) */
#define CODE_L7I    27                  /* obs code: E5bI,B2I   (GAL,CMP) */
#define CODE_L7Q    28                  /* obs code: E5bQ,B2Q   (GAL,CMP) */
#define CODE_L7X    29                  /* obs code: E5bI+Q,B2I+Q (GAL,CMP) */
#define CODE_L6A    30                  /* obs code: E6A        (GAL) */
#define CODE_L6B    31                  /* obs code: E6B        (GAL) */
#define CODE_L6C    32                  /* obs code: E6C        (GAL) */
#define CODE_L6X    33                  /* obs code: E6B+C,LEXS+L,B3I+Q (GAL,QZS,CMP) */
#define CODE_L6Z    34                  /* obs code: E6A+B+C    (GAL) */
#define CODE_L6S    35                  /* obs code: LEXS       (QZS) */
#define CODE_L6L    36                  /* obs code: LEXL       (QZS) */
#define CODE_L8I    37                  /* obs code: E5(a+b)I   (GAL) */
#define CODE_L8Q    38                  /* obs code: E5(a+b)Q   (GAL) */
#define CODE_L8X    39                  /* obs code: E5(a+b)I+Q (GAL) */
#define CODE_L2I    40                  /* obs code: B1I        (CMP) */
#define CODE_L2Q    41                  /* obs code: B1Q        (CMP) */
#define CODE_L6I    42                  /* obs code: B3I        (CMP) */
#define CODE_L6Q    43                  /* obs code: B3Q        (CMP) */
#define CODE_L3I    44                  /* obs code: G3I        (GLO) */
#define CODE_L3Q    45                  /* obs code: G3Q        (GLO) */
#define CODE_L3X    46                  /* obs code: G3I+Q      (GLO) */
#define CODE_L1I    47                  /* obs code: B1I        (BDS) */
#define CODE_L1Q    48                  /* obs code: B1Q        (BDS) */
#define MAXCODE     48                  /* max number of obs code */

static double leaps[MAXLEAPS + 1][7] = { /* leap seconds (y,m,d,h,m,s,utc-gpst) */
	{ 2017,1,1,0,0,0,-18 },
	{ 2015,7,1,0,0,0,-17 },
	{ 2012,7,1,0,0,0,-16 },
	{ 2009,1,1,0,0,0,-15 },
	{ 2006,1,1,0,0,0,-14 },
	{ 1999,1,1,0,0,0,-13 },
	{ 1997,7,1,0,0,0,-12 },
	{ 1996,1,1,0,0,0,-11 },
	{ 1994,7,1,0,0,0,-10 },
	{ 1993,7,1,0,0,0, -9 },
	{ 1992,7,1,0,0,0, -8 },
	{ 1991,1,1,0,0,0, -7 },
	{ 1990,1,1,0,0,0, -6 },
	{ 1988,1,1,0,0,0, -5 },
	{ 1985,7,1,0,0,0, -4 },
	{ 1983,7,1,0,0,0, -3 },
	{ 1982,7,1,0,0,0, -2 },
	{ 1981,7,1,0,0,0, -1 },
	{ 0 }
};

#define CLIGHT      299792458.0         /* speed of light (m/s) */
#define SC2RAD      3.1415926535898     /* semi-circle to radian (IS-GPS) */
#define NSATQZS     0

#define MAXVAL      8388608.0

#define ID_RANGECMP 140         /* message id: oem4 range compressed */

#define SYS_GLO     0x04                /* navigation system: GLONASS */

#define ID_BDSEPHEMERIS 1047   /* message id: TureCORS decoded bds ephemeris */
#define ID_GPSEPHEMERIS  7      /* message id: TureCORS decoded gps ephemeris */
#define ID_RAWEPHEM 41          /* message id: oem4 raw ephemeris */
#define ID_BD2IONUTC 2010

#define P2_5        0.03125             /* 2^-5 */
#define P2_6        0.015625            /* 2^-6 */
#define P2_11       4.882812500000000E-04 /* 2^-11 */
#define P2_15       3.051757812500000E-05 /* 2^-15 */
#define P2_17       7.629394531250000E-06 /* 2^-17 */
#define P2_19       1.907348632812500E-06 /* 2^-19 */
#define P2_20       9.536743164062500E-07 /* 2^-20 */
#define P2_21       4.768371582031250E-07 /* 2^-21 */
#define P2_23       1.192092895507810E-07 /* 2^-23 */
#define P2_24       5.960464477539063E-08 /* 2^-24 */
#define P2_27       7.450580596923828E-09 /* 2^-27 */
#define P2_29       1.862645149230957E-09 /* 2^-29 */
#define P2_30       9.313225746154785E-10 /* 2^-30 */
#define P2_31       4.656612873077393E-10 /* 2^-31 */
#define P2_32       2.328306436538696E-10 /* 2^-32 */
#define P2_33       1.164153218269348E-10 /* 2^-33 */
#define P2_35       2.910383045673370E-11 /* 2^-35 */
#define P2_38       3.637978807091710E-12 /* 2^-38 */
#define P2_39       1.818989403545856E-12 /* 2^-39 */
#define P2_40       9.094947017729280E-13 /* 2^-40 */
#define P2_43       1.136868377216160E-13 /* 2^-43 */
#define P2_48       3.552713678800501E-15 /* 2^-48 */
#define P2_50       8.881784197001252E-16 /* 2^-50 */
#define P2_55       2.775557561562891E-17 /* 2^-55 */

#define RTCM3PREAMB 0xD3        /* rtcm ver.3 frame preamble */

#define ROUND(x)    ((int)floor((x)+0.5))
#define ROUND_U(x)  ((unsigned int)floor((x)+0.5))
#define MIN(x,y)    ((x)<(y)?(x):(y))

#define RANGE_MS    (CLIGHT*0.001)      /* range in 1 ms */
#define P2_10       0.0009765625          /* 2^-10 */
#define P2_34       5.820766091346740E-11 /* 2^-34 */
#define P2_46       1.421085471520200E-14 /* 2^-46 */
#define P2_59       1.734723475976810E-18 /* 2^-59 */

#define FREQ1_CMP   1.561098E9          /* BeiDou B1 frequency (Hz) */
#define FREQ1       1.57542E9           /* L1/E1  frequency (Hz) */
#define FREQ2       1.22760E9           /* L2     frequency (Hz) */
#define FREQ5       1.17645E9           /* L5/E5a frequency (Hz) */
#define FREQ6       1.27875E9           /* E6/LEX frequency (Hz) */
#define FREQ7       1.20714E9           /* E5b    frequency (Hz) */
#define FREQ8       1.191795E9          /* E5a+b  frequency (Hz) */

const double lam_carr[] = {       /* carrier wave length (m) */
	CLIGHT / FREQ1,CLIGHT / FREQ2,CLIGHT / FREQ5,CLIGHT / FREQ6,CLIGHT / FREQ7,CLIGHT / FREQ8
};

static const unsigned int tbl_CRC24Q[] = {
	0x000000,0x864CFB,0x8AD50D,0x0C99F6,0x93E6E1,0x15AA1A,0x1933EC,0x9F7F17,
	0xA18139,0x27CDC2,0x2B5434,0xAD18CF,0x3267D8,0xB42B23,0xB8B2D5,0x3EFE2E,
	0xC54E89,0x430272,0x4F9B84,0xC9D77F,0x56A868,0xD0E493,0xDC7D65,0x5A319E,
	0x64CFB0,0xE2834B,0xEE1ABD,0x685646,0xF72951,0x7165AA,0x7DFC5C,0xFBB0A7,
	0x0CD1E9,0x8A9D12,0x8604E4,0x00481F,0x9F3708,0x197BF3,0x15E205,0x93AEFE,
	0xAD50D0,0x2B1C2B,0x2785DD,0xA1C926,0x3EB631,0xB8FACA,0xB4633C,0x322FC7,
	0xC99F60,0x4FD39B,0x434A6D,0xC50696,0x5A7981,0xDC357A,0xD0AC8C,0x56E077,
	0x681E59,0xEE52A2,0xE2CB54,0x6487AF,0xFBF8B8,0x7DB443,0x712DB5,0xF7614E,
	0x19A3D2,0x9FEF29,0x9376DF,0x153A24,0x8A4533,0x0C09C8,0x00903E,0x86DCC5,
	0xB822EB,0x3E6E10,0x32F7E6,0xB4BB1D,0x2BC40A,0xAD88F1,0xA11107,0x275DFC,
	0xDCED5B,0x5AA1A0,0x563856,0xD074AD,0x4F0BBA,0xC94741,0xC5DEB7,0x43924C,
	0x7D6C62,0xFB2099,0xF7B96F,0x71F594,0xEE8A83,0x68C678,0x645F8E,0xE21375,
	0x15723B,0x933EC0,0x9FA736,0x19EBCD,0x8694DA,0x00D821,0x0C41D7,0x8A0D2C,
	0xB4F302,0x32BFF9,0x3E260F,0xB86AF4,0x2715E3,0xA15918,0xADC0EE,0x2B8C15,
	0xD03CB2,0x567049,0x5AE9BF,0xDCA544,0x43DA53,0xC596A8,0xC90F5E,0x4F43A5,
	0x71BD8B,0xF7F170,0xFB6886,0x7D247D,0xE25B6A,0x641791,0x688E67,0xEEC29C,
	0x3347A4,0xB50B5F,0xB992A9,0x3FDE52,0xA0A145,0x26EDBE,0x2A7448,0xAC38B3,
	0x92C69D,0x148A66,0x181390,0x9E5F6B,0x01207C,0x876C87,0x8BF571,0x0DB98A,
	0xF6092D,0x7045D6,0x7CDC20,0xFA90DB,0x65EFCC,0xE3A337,0xEF3AC1,0x69763A,
	0x578814,0xD1C4EF,0xDD5D19,0x5B11E2,0xC46EF5,0x42220E,0x4EBBF8,0xC8F703,
	0x3F964D,0xB9DAB6,0xB54340,0x330FBB,0xAC70AC,0x2A3C57,0x26A5A1,0xA0E95A,
	0x9E1774,0x185B8F,0x14C279,0x928E82,0x0DF195,0x8BBD6E,0x872498,0x016863,
	0xFAD8C4,0x7C943F,0x700DC9,0xF64132,0x693E25,0xEF72DE,0xE3EB28,0x65A7D3,
	0x5B59FD,0xDD1506,0xD18CF0,0x57C00B,0xC8BF1C,0x4EF3E7,0x426A11,0xC426EA,
	0x2AE476,0xACA88D,0xA0317B,0x267D80,0xB90297,0x3F4E6C,0x33D79A,0xB59B61,
	0x8B654F,0x0D29B4,0x01B042,0x87FCB9,0x1883AE,0x9ECF55,0x9256A3,0x141A58,
	0xEFAAFF,0x69E604,0x657FF2,0xE33309,0x7C4C1E,0xFA00E5,0xF69913,0x70D5E8,
	0x4E2BC6,0xC8673D,0xC4FECB,0x42B230,0xDDCD27,0x5B81DC,0x57182A,0xD154D1,
	0x26359F,0xA07964,0xACE092,0x2AAC69,0xB5D37E,0x339F85,0x3F0673,0xB94A88,
	0x87B4A6,0x01F85D,0x0D61AB,0x8B2D50,0x145247,0x921EBC,0x9E874A,0x18CBB1,
	0xE37B16,0x6537ED,0x69AE1B,0xEFE2E0,0x709DF7,0xF6D10C,0xFA48FA,0x7C0401,
	0x42FA2F,0xC4B6D4,0xC82F22,0x4E63D9,0xD11CCE,0x575035,0x5BC9C3,0xDD8538
};

static unsigned short U2(unsigned char *p) { 
	unsigned short u;
	memcpy(&u, p, 2); 
	return u; 
}

static unsigned char obsfreqs[] = { /* 1:L1,2:L2,3:L5,4:L6,5:L7,6:L8,7:L3 */

	0, 1, 1, 1, 1,  1, 1, 1, 1, 1, /*  0- 9 */
	1, 1, 1, 1, 2,  2, 2, 2, 2, 2, /* 10-19 */
	2, 2, 2, 2, 3,  3, 3, 5, 5, 5, /* 20-29 */
	4, 4, 4, 4, 4,  4, 4, 6, 6, 6, /* 30-39 */
	2, 2, 4, 4, 3,  3, 3, 1, 1, 0  /* 40-49 */
};

static char *obscodes[] = {       /* observation code strings */

	""  ,"1C","1P","1W","1Y", "1M","1N","1S","1L","1E", /*  0- 9 */
	"1A","1B","1X","1Z","2C", "2D","2S","2L","2X","2P", /* 10-19 */
	"2W","2Y","2M","2N","5I", "5Q","5X","7I","7Q","7X", /* 20-29 */
	"6A","6B","6C","6X","6Z", "6S","6L","8L","8Q","8X", /* 30-39 */
	"2I","2Q","6I","6Q","3I", "3Q","3X","1I","1Q",""    /* 40-49 */
};

/* msm signal id table -------------------------------------------------------*/
const char *msm_sig_gps[32] = {
	/* GPS: ref [13] table 3.5-87, ref [14][15] table 3.5-91 */
	""  ,"1C","1P","1W","1Y","1M",""  ,"2C","2P","2W","2Y","2M", /*  1-12 */
	""  ,""  ,"2S","2L","2X",""  ,""  ,""  ,""  ,"5I","5Q","5X", /* 13-24 */
	""  ,""  ,""  ,""  ,""  ,"1S","1L","1X"                      /* 25-32 */
};

const char *msm_sig_cmp[32] = {
	/* BeiDou: ref [15] table 3.5-106 */
	""  ,"1I","1Q","1X",""  ,""  ,""  ,"6I","6Q","6X",""  ,""  ,
	""  ,"7I","7Q","7X",""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,
	""  ,""  ,""  ,""  ,""  ,""  ,""  ,""
};

static unsigned int   U4(unsigned char *p) { unsigned int   u; memcpy(&u, p, 4); return u; }
static int            I4(unsigned char *p) { int            i; memcpy(&i, p, 4); return i; }
static float          R4(unsigned char *p) { float          r; memcpy(&r, p, 4); return r; }
static double         R8(unsigned char *p) { double         r; memcpy(&r, p, 8); return r; }


static int fswapmargin = 30;  /* file swap margin (s) */

/* extend sign ---------------------------------------------------------------*/
static int exsign(unsigned int v, int bits)
{
	return (int)(v&(1 << (bits - 1)) ? v | (~0u << bits) : v);
}

unsigned char MxtChkSum_Cal(const char* str, int len)
{
    unsigned char CheckSum = 0;
    unsigned int i = 0;
    for(i=0; i<len; i++) CheckSum ^= str[i];
    return CheckSum;
}

unsigned char CalCheckSum(const char* str)
{
	unsigned int strLength = 0;
	unsigned int  bCheckSum = 0;
	unsigned int  i = 0;
	strLength = strlen(str);
	for (i = 0; i<strLength; i++)
	{
		bCheckSum ^= str[i];
	}
	return (unsigned char)(bCheckSum & 0xff);
}


/* convert calendar day/time to time -------------------------------------------
* convert calendar day/time to gtime_t struct
* args   : double *ep       I   day/time {year,month,day,hour,min,sec}
* return : gtime_t struct
* notes  : proper in 1970-2037 or 1970-2099 (64bit time_t)
*-----------------------------------------------------------------------------*/
extern gtime_t epoch2time(const double *ep)
{
	const int doy[] = { 1,32,60,91,121,152,182,213,244,274,305,335 };
	gtime_t time = { 0 };
	int days, sec, year = (int)ep[0], mon = (int)ep[1], day = (int)ep[2];

	if (year<1970 || 2099<year || mon<1 || 12<mon) return time;

	/* leap year if year%4==0 in 1901-2099 */
	days = (year - 1970) * 365 + (year - 1969) / 4 + doy[mon - 1] + day - 2 + (year % 4 == 0 && mon >= 3 ? 1 : 0);
	sec = (int)floor(ep[5]);
	time.time = (time_t)days * 86400 + (int)ep[3] * 3600 + (int)ep[4] * 60 + sec;
	time.sec = ep[5] - sec;
	return time;
}

/* add time --------------------------------------------------------------------
* add time to gtime_t struct
* args   : gtime_t t        I   gtime_t struct
*          double sec       I   time to add (s)
* return : gtime_t struct (t+sec)
*-----------------------------------------------------------------------------*/
extern gtime_t timeadd(gtime_t t, double sec)
{
	double tt;

	t.sec += sec; tt = floor(t.sec); t.time += (int)tt; t.sec -= tt;
	return t;
}

extern gtime_t gpst2time(int week, double sec)
{
	gtime_t t = epoch2time(gpst0);

	if (sec<-1E9 || 1E9<sec) sec = 0.0;
	t.time += 86400 * 7 * week + (int)sec;
	t.sec = sec - (int)sec;
	return t;
}

static double timeoffset_ = 0.0;        /* time offset (s) */

extern gtime_t timeget(void)
{
	double ep[6] = { 0 };
#ifdef WIN32
	SYSTEMTIME ts;

	GetSystemTime(&ts); /* utc */
	ep[0] = ts.wYear; ep[1] = ts.wMonth;  ep[2] = ts.wDay;
	ep[3] = ts.wHour; ep[4] = ts.wMinute; ep[5] = ts.wSecond + ts.wMilliseconds*1E-3;
#else
	struct timeval tv;
	struct tm *tt;

	if (!gettimeofday(&tv, NULL) && (tt = gmtime(&tv.tv_sec))) {
		ep[0] = tt->tm_year + 1900; ep[1] = tt->tm_mon + 1; ep[2] = tt->tm_mday;
		ep[3] = tt->tm_hour; ep[4] = tt->tm_min; ep[5] = tt->tm_sec + tv.tv_usec*1E-6;
	}
#endif
	return timeadd(epoch2time(ep), timeoffset_);
}

/* time to gps time ------------------------------------------------------------
* convert gtime_t struct to week and tow in gps time
* args   : gtime_t t        I   gtime_t struct
*          int    *week     IO  week number in gps time (NULL: no output)
* return : time of week in gps time (s)
*-----------------------------------------------------------------------------*/
extern double time2gpst(gtime_t t, int *week)
{
	gtime_t t0 = epoch2time(gpst0);
	time_t sec = t.time - t0.time;
	int w = (int)(sec / (86400 * 7));

	if (week) *week = w;
	return (double)(sec - w * 86400 * 7) + t.sec;
}

/* time difference -------------------------------------------------------------
* difference between gtime_t structs
* args   : gtime_t t1,t2    I   gtime_t structs
* return : time difference (t1-t2) (s)
*-----------------------------------------------------------------------------*/
extern double timediff(gtime_t t1, gtime_t t2)
{
	return difftime(t1.time, t2.time) + t1.sec - t2.sec;
}

/* utc to gpstime --------------------------------------------------------------
* convert utc to gpstime considering leap seconds
* args   : gtime_t t        I   time expressed in utc
* return : time expressed in gpstime
* notes  : ignore slight time offset under 100 ns
*-----------------------------------------------------------------------------*/
extern gtime_t utc2gpst(gtime_t t)
{
	int i;

	for (i = 0; leaps[i][0]>0; i++) {
		if (timediff(t, epoch2time(leaps[i])) >= 0.0) return timeadd(t, -leaps[i][6]);
	}
	return t;
}

/* adjust gps week number ------------------------------------------------------
* adjust gps week number using cpu time
* args   : int   week       I   not-adjusted gps week number
* return : adjusted gps week number
*-----------------------------------------------------------------------------*/
extern int adjgpsweek(int week)
{
	int w;
	(void)time2gpst(utc2gpst(timeget()), &w);
	if (w<1560) w = 1560; /* use 2009/12/1 if time is earlier than 2009/12/1 */
	return week + (w - week + 512) / 1024 * 1024;
}



int MxtChkSum_Chk(const char* str)
{
    unsigned int  CheckSum = 0;
    int len = strlen(str);
    if ('*'==str[len-5] && '\r'==str[len-2] && '\n'==str[len-1]) {
        sscanf(&(str[len-4]), "%x", &CheckSum);
        if (CheckSum == MxtChkSum_Cal(str+1, len-6)) {
            return 1;
        }
    }
    return 0;
}

/* gpstime to utc --------------------------------------------------------------
* convert gpstime to utc considering leap seconds
* args   : gtime_t t        I   time expressed in gpstime
* return : time expressed in utc
* notes  : ignore slight time offset under 100 ns
*-----------------------------------------------------------------------------*/
extern gtime_t gpst2utc(gtime_t t)
{
	gtime_t tu;
	int i;

	for (i = 0; leaps[i][0]>0; i++) {
		tu = timeadd(t, leaps[i][6]);
		if (timediff(tu, epoch2time(leaps[i])) >= 0.0) return tu;
	}
	return t;
}

/* set unsigned/signed bits ----------------------------------------------------
* set unsigned/signed bits to byte data
* args   : unsigned char *buff IO byte data
*          int    pos    I      bit position from start of data (bits)
*          int    len    I      bit length (bits) (len<=32)
*         (unsigned) int I      unsigned/signed data
* return : none
*-----------------------------------------------------------------------------*/
void setbitu(unsigned char *buff, int pos, int len, unsigned int data)
{
	unsigned int mask = 1u << (len - 1);
	int i;
	if (len <= 0 || 32<len) return;
	for (i = pos; i<pos + len; i++, mask >>= 1) {
		if (data&mask) buff[i / 8] |= 1u << (7 - i % 8); else buff[i / 8] &= ~(1u << (7 - i % 8));
	}
}


/* extract unsigned/signed bits ------------------------------------------------
* extract unsigned/signed bits from byte data
* args   : unsigned char *buff I byte data
*          int    pos    I      bit position from start of data (bits)
*          int    len    I      bit length (bits) (len<=32)
* return : extracted unsigned/signed bits
*-----------------------------------------------------------------------------*/
unsigned int getbitu(const unsigned char *buff, int pos, int len)
{
	unsigned int bits = 0;
	int i;
	for (i = pos; i<pos + len; i++) bits = (bits << 1) + ((buff[i / 8] >> (7 - i % 8)) & 1u);
	return bits;
}

extern int getbits(const unsigned char *buff, int pos, int len)
{
	unsigned int bits = getbitu(buff, pos, len);
	if (len <= 0 || 32 <= len || !(bits&(1u << (len - 1)))) return (int)bits;
	return (int)(bits | (~0u << len)); /* extend sign */
}

/* sync header ---------------------------------------------------------------*/
static int sync_oem4(unsigned char *buff, unsigned char data)
{
	buff[0] = buff[1]; buff[1] = buff[2]; buff[2] = data;
	return buff[0] == OEM4SYNC1&&buff[1] == OEM4SYNC2&&buff[2] == OEM4SYNC3;
}


/* free receiver raw data control ----------------------------------------------
* free observation and ephemeris buffer in receiver raw data control struct
* args   : raw_t  *raw   IO     receiver raw data control struct
* return : none
*-----------------------------------------------------------------------------*/
extern void free_raw(raw_t *raw)
{
	free(raw->obs.data); raw->obs.data = NULL; raw->obs.n = 0;
	free(raw->obuf.data); raw->obuf.data = NULL; raw->obuf.n = 0;
	free(raw->nav.eph); raw->nav.eph = NULL; raw->nav.n = 0;
	free(raw->nav.alm); raw->nav.alm = NULL; raw->nav.na = 0;
}


//////////////////////////////////////////////////////////////////////////
void DgnssData_Init(rtcm_t* dgnssData)
{
	int i, j;
	gtime_t time0 = { 0 };
	obsd_t data0 = { { 0 } };

	dgnssData->staid = 0;
	dgnssData->stah = 0;
	dgnssData->seqno = 0;
	dgnssData->outtype = 0;

	dgnssData->obs.data = NULL;

	for (i = 0; i<MAXSAT; i++) {
		for (j = 0; j<NFREQ; j++) dgnssData->cp[i][j] = 0.0;
		for (j = 0; j<NFREQ; j++) dgnssData->lock[i][j] = 0;
		for (j = 0; j<NFREQ; j++) dgnssData->loss[i][j] = 0;
		for (j = 0; j < NFREQ; j++){
			dgnssData->lltime[i][j].time = 0;
			dgnssData->lltime[i][j].sec = 0;
		}
	}
	if (!(dgnssData->obs.data = (obsd_t *)malloc(sizeof(obsd_t)*MAXOBS))) {
		free_raw(dgnssData);
		return 0;
	}
	//for (i = 0; i<MAXOBS; i++) dgnssData->obs.data[i] = data0;
	for (i = 0; i < MAXOBS; i++) {
		dgnssData->obs.data[i] = data0;
	}

	dgnssData->buff[0] = '\0';/* message buffer */
	for (j = 0; j < 2; j++) {
		dgnssData->gpsbdsbuff[j][0] = '\0';
	}
	dgnssData->opt[0] = '\0';
	dgnssData->msg[0] = '\0';
	dgnssData->msgtype[0] = '\0';
	for (i = 0; i<6; i++) dgnssData->msmtype[i][0] = '\0';

	dgnssData->obsflag = 0;/* obs data complete flag (1:ok,0:not complete) */
	dgnssData->ephsat = 0; /* update satellite of ephemeris */
	dgnssData->len = 0;
	dgnssData->nbyte = 0;  /* number of bytes in message buffer */
	dgnssData->nbit = 0; /* message length (bytes) */
    
	dgnssData->nmsg3[0] = '\0';/* message count of RTCM 3 (1-299:1001-1299,0:ohter) */
}

//////////////////////////////////////////////////////////////////////////
void AssistData_Init(raw_t* rawData)
{
	gtime_t time0 = { 0 };
	obsd_t data0 = { { 0 } };
	eph_t  eph0 = { 0,-1,-1 };
	alm_t  alm0 = { 0,-1 };
	int i, j, sys;

	memset(&rawData->assistData, 0, sizeof(rawData->assistData));
	memset(&rawData->assistData.ADdec, 0, sizeof(rawData->assistData.ADdec));

	memset(&rawData->dgnssDat, 0, sizeof(rawData->dgnssDat));

	rawData->bdsmask_high = 0;
	rawData->bdsmask_low = 0;
	rawData->gpsmask = 0;

	rawData->time = rawData->tobs = time0;
	rawData->ephsat = 0;
	rawData->msgtype[0] = '\0';
	for (i = 0; i<MAXSAT; i++) {
		for (j = 0; j<380; j++) rawData->subfrm[i][j] = 0;
		for (j = 0; j<NFREQ; j++) rawData->lockt[i][j] = 0.0;
		for (j = 0; j<NFREQ; j++) rawData->halfc[i][j] = 0;
		rawData->icpp[i] = rawData->off[i] = rawData->prCA[i] = rawData->dpCA[i] = 0.0;
	}
	for (i = 0; i<MAXOBS; i++) rawData->freqn[i] = 0;
	rawData->icpc = 0.0;
	rawData->nbyte = rawData->len = 0;
	rawData->iod = rawData->flag = rawData->tbase = rawData->outtype = 0;
	rawData->tod = -1;
	for (i = 0; i<MAXRAWLEN; i++) rawData->buff[i] = 0;
	rawData->opt[0] = '\0';
	rawData->receive_time = 0.0;
	rawData->plen = rawData->pbyte = rawData->page = rawData->reply = 0;
	rawData->week = 0;

	rawData->obs.data = NULL;
	rawData->obuf.data = NULL;
	rawData->nav.eph = NULL;
	rawData->nav.alm = NULL;


	if (!(rawData->obs.data = (obsd_t *)malloc(sizeof(obsd_t)*MAXOBS)) ||
		!(rawData->obuf.data = (obsd_t *)malloc(sizeof(obsd_t)*MAXOBS)) ||
		!(rawData->nav.eph = (eph_t  *)malloc(sizeof(eph_t)*MAXSAT)) ||
		!(rawData->nav.alm = (alm_t  *)malloc(sizeof(alm_t)*MAXSAT))) {
		free_raw(rawData);
		return 0;
	}
	rawData->obs.n = 0;
	rawData->obuf.n = 0;
	rawData->nav.n = MAXSAT;
	rawData->nav.na = MAXSAT;

	for (i = 0; i<MAXOBS; i++) rawData->obs.data[i] = data0;
	for (i = 0; i<MAXOBS; i++) rawData->obuf.data[i] = data0;
	for (i = 0; i<MAXSAT; i++) rawData->nav.eph[i] = eph0;
	for (i = 0; i<MAXSAT; i++) rawData->nav.alm[i] = alm0;
	for (i = 0; i<MAXSAT; i++) for (j = 0; j<NFREQ; j++) {
		if (!(sys = satsys(i + 1, NULL))) continue;
	}
	rawData->sta.name[0] = rawData->sta.marker[0] = '\0';
	rawData->sta.antdes[0] = rawData->sta.antsno[0] = '\0';
	rawData->sta.rectype[0] = rawData->sta.recver[0] = rawData->sta.recsno[0] = '\0';
	rawData->sta.antsetup = rawData->sta.itrf = rawData->sta.deltype = 0;
	for (i = 0; i<3; i++) {
		rawData->sta.pos[i] = rawData->sta.del[i] = 0.0;
	}
	rawData->sta.hgt = 0.0;

	memset(&rawData->gps_dgnssdata, 0, sizeof(rtcm_t));
	DgnssData_Init(&rawData->gps_dgnssdata);

	memset(&rawData->bds_dgnssdata, 0, sizeof(rtcm_t));
	DgnssData_Init(&rawData->bds_dgnssdata);

	memset(&rawData->refstation, 0, sizeof(rtcm_t));
	DgnssData_Init(&rawData->refstation);

}

void decode_type43(TureCORS_Messaage_Range* messageRange, char* buff, int len)
{
	TureCORS_Messaage_Range* PmessageRange = messageRange;
}

/* satellite number to satellite system ----------------------------------------
* convert satellite number to satellite system
* args   : int    sat       I   satellite number (1-MAXSAT)
*          int    *prn      IO  satellite prn/slot number (NULL: no output)
* return : satellite system (SYS_GPS,SYS_GLO,...)
*-----------------------------------------------------------------------------*/


extern int satsys(int sat, int *prn)
{
	int sys = SYS_NONE;
	if (sat <= 0 || MAXSAT<sat) sat = 0;
	else if (sat <= NSATGPS) {
		sys = SYS_GPS; sat += MINPRNGPS - 1;
	}
	else if ((sat -= NSATGPS) <= NSATGLO) {
		sys = SYS_GLO; sat += MINPRNGLO - 1;
	}
	else if ((sat -= NSATGLO) <= NSATGAL) {
		sys = SYS_GAL; sat += MINPRNGAL - 1;
	}
	else if ((sat -= NSATGAL) <= NSATQZS) {
		sys = SYS_QZS; sat += MINPRNQZS - 1;
	}
	else if ((sat -= NSATQZS) <= NSATCMP) {
		sys = SYS_CMP; sat += 0;
	}
	else sat = 0;
	if (prn) *prn = sat;
	return sys;
}


/* crc-32 parity ---------------------------------------------------------------
* compute crc-32 parity for novatel raw
* args   : unsigned char *buff I data
*          int    len    I      data length (bytes)
* return : crc-32 parity
* notes  : see NovAtel OEMV firmware manual 1.7 32-bit CRC
*-----------------------------------------------------------------------------*/
extern unsigned int crc32(const unsigned char *buff, int len)
{
	unsigned int crc = 0;
	int i, j;

	for (i = 0; i<len; i++) {
		crc ^= buff[i];
		for (j = 0; j<8; j++) {
			if (crc & 1) crc = (crc >> 1) ^ POLYCRC32; else crc >>= 1;
		}
	}
	return crc;
}

/* decode oem4 tracking status -------------------------------------------------
* deocode oem4 tracking status
* args   : unsigned int stat I  tracking status field
*          int    *sys   O      system (SYS_???)
*          int    *code  O      signal code (CODE_L??)
*          int    *track O      tracking state
*                         (oem4/5)
*                         0=L1 idle                   8=L2 idle
*                         1=L1 sky search             9=L2 p-code align
*                         2=L1 wide freq pull-in     10=L2 search
*                         3=L1 narrow freq pull-in   11=L2 pll
*                         4=L1 pll                   12=L2 steering
*                         5=L1 reacq
*                         6=L1 steering
*                         7=L1 fll
*                         (oem6)
*                         0=idle                      7=freq-lock loop
*                         2=wide freq band pull-in    9=channel alignment
*                         3=narrow freq band pull-in 10=code search
*                         4=phase lock loop          11=aided phase lock loop
*          int    *plock O      phase-lock flag   (0=not locked, 1=locked)
*          int    *clock O      code-lock flag    (0=not locked, 1=locked)
*          int    *parity O     parity known flag (0=not known,  1=known)
*          int    *halfc O      phase measurement (0=half-cycle not added,
*                                                  1=added)
* return : signal frequency (0:L1,1:L2,2:L5,3:L6,4:L7,5:L8,-1:error)
* notes  : refer [1][3]
*-----------------------------------------------------------------------------*/
static int decode_trackstat(unsigned int stat, int *sys, int *code, int *track,
	int *plock, int *clock, int *parity, int *halfc)
{
	int satsys, sigtype, freq = 0;

	*track = stat & 0x1F;
	*plock = (stat >> 10) & 1;
	*parity = (stat >> 11) & 1;
	*clock = (stat >> 12) & 1;
	satsys = (stat >> 16) & 7;
	*halfc = (stat >> 28) & 1;
	sigtype = (stat >> 21) & 0x1F;

	switch (satsys) {
	case 0: *sys = SYS_GPS; break;
	case 4: *sys = SYS_CMP; break; /* OEM6 F/W 6.400 */
	default:
		return -1;
	}
	if (*sys == SYS_GPS) {
		switch (sigtype) {
		case  0: freq = 0; *code = CODE_L1C; break; /* L1C/A */
		default: freq = -1; break;
		}
	}
	else if (*sys == SYS_CMP) {
		switch (sigtype) {
		case  0: freq = 0; *code = CODE_L1I; break; /* B1 with D1 (OEM6) */
		case  4: freq = 0; *code = CODE_L1I; break; /* B1 with D2 (OEM6) */
		default: freq = -1; break;
		}
	}
	if (freq<0) {
		return -1;
	}
	return freq;
}

/* check code priority and return obs position -------------------------------*/
static int checkpri(const char *opt, int sys, int code, int freq)
{
	int nex = NEXOBS; /* number of extended obs data */

	if (sys == SYS_GPS) {
		if (strstr(opt, "-GL1P") && freq == 0) return code == CODE_L1P ? 0 : -1;
		if (strstr(opt, "-GL2X") && freq == 1) return code == CODE_L2X ? 1 : -1;
		if (code == CODE_L1P) return nex<1 ? -1 : NFREQ;
		if (code == CODE_L2X) return nex<2 ? -1 : NFREQ + 1;
	}
	return freq<NFREQ ? freq : -1;
}

/* time to calendar day/time ---------------------------------------------------
* convert gtime_t struct to calendar day/time
* args   : gtime_t t        I   gtime_t struct
*          double *ep       O   day/time {year,month,day,hour,min,sec}
* return : none
* notes  : proper in 1970-2037 or 1970-2099 (64bit time_t)
*-----------------------------------------------------------------------------*/
extern void time2epoch(gtime_t t, double *ep)
{
	const int mday[] = { /* # of days in a month */
		31,28,31,30,31,30,31,31,30,31,30,31,31,28,31,30,31,30,31,31,30,31,30,31,
		31,29,31,30,31,30,31,31,30,31,30,31,31,28,31,30,31,30,31,31,30,31,30,31
	};
	int days, sec, mon, day;

	/* leap year if year%4==0 in 1901-2099 */
	days = (int)(t.time / 86400);
	sec = (int)(t.time - (time_t)days * 86400);
	for (day = days % 1461, mon = 0; mon<48; mon++) {
		if (day >= mday[mon]) day -= mday[mon]; else break;
	}
	ep[0] = 1970 + days / 1461 * 4 + mon / 12; ep[1] = mon % 12 + 1; ep[2] = day + 1;
	ep[3] = sec / 3600; ep[4] = sec % 3600 / 60; ep[5] = sec % 60 + t.sec;
}

/* time to string --------------------------------------------------------------
* convert gtime_t struct to string
* args   : gtime_t t        I   gtime_t struct
*          char   *s        O   string ("yyyy/mm/dd hh:mm:ss.ssss")
*          int    n         I   number of decimals
* return : none
*-----------------------------------------------------------------------------*/
extern void time2str(gtime_t t, char *s, int n)
{
	double ep[6];

	if (n<0) n = 0; else if (n>12) n = 12;
	if (1.0 - t.sec<0.5 / pow(10.0, n)) { t.time++; t.sec = 0.0; };
	time2epoch(t, ep);
	sprintf(s, "%04.0f/%02.0f/%02.0f %02.0f:%02.0f:%0*.*f", ep[0], ep[1], ep[2],
		ep[3], ep[4], n <= 0 ? 2 : n + 3, n <= 0 ? 0 : n, ep[5]);
}

/* get time string -------------------------------------------------------------
* get time string
* args   : gtime_t t        I   gtime_t struct
*          int    n         I   number of decimals
* return : time string
* notes  : not reentrant, do not use multiple in a function
*-----------------------------------------------------------------------------*/
extern char *time_str(gtime_t t, int n)
{
	static char buff[64];
	time2str(t, buff, n);
	return buff;
}

/* get observation data index ------------------------------------------------*/
static int obsindex(obs_t *obs, gtime_t time, int sat)
{
	int i, j;

	if (obs->n >= MAXOBS) return -1;
	for (i = 0; i<obs->n; i++) {
		if (obs->data[i].sat == sat) return i;
	}
	obs->data[i].time = time;
	obs->data[i].sat = sat;
	for (j = 0; j<NFREQ + NEXOBS; j++) {
		obs->data[i].L[j] = obs->data[i].P[j] = 0.0;
		obs->data[i].D[j] = 0.0;
		obs->data[i].SNR[j] = obs->data[i].LLI[j] = 0;
		obs->data[i].code[j] = CODE_NONE;
	}
	obs->n++;
	return i;
}

/* satellite system+prn/slot number to satellite number ------------------------
* convert satellite system+prn/slot number to satellite number
* args   : int    sys       I   satellite system (SYS_GPS,SYS_GLO,...)
*          int    prn       I   satellite prn/slot number
* return : satellite number (0:error)
*-----------------------------------------------------------------------------*/
extern int satno(int sys, int prn)
{
	if (prn <= 0) return 0;
	switch (sys) {
	case SYS_GPS:
		if (prn<MINPRNGPS || MAXPRNGPS<prn) return 0;
		return prn - MINPRNGPS + 1;
	case SYS_GLO:
		if (prn<MINPRNGLO || MAXPRNGLO<prn) return 0;
		return NSATGPS + prn - MINPRNGLO + 1;
	case SYS_GAL:
		if (prn<MINPRNGAL || MAXPRNGAL<prn) return 0;
		return NSATGPS + NSATGLO + prn - MINPRNGAL + 1;
	case SYS_QZS:
		if (prn<MINPRNQZS || MAXPRNQZS<prn) return 0;
		return NSATGPS + NSATGLO + NSATGAL + prn - MINPRNQZS + 1;
	case SYS_CMP:
		if (prn<MINPRNCMP || MAXPRNCMP<prn) return 0;
		return NSATGPS + NSATGLO + NSATGAL + NSATQZS + prn - MINPRNCMP + 1;
	}
	return 0;
}
/*-----------------------------------------------------------
extern int satno(int sys, int prn)
{
	if (prn <= 0) return 0;
	switch (sys) {
	case SYS_GPS:
		if (prn<MINPRNGPS || MAXPRNGPS<prn) return 0;
		return prn - MINPRNGPS + 1;
	case SYS_CMP:
		if (prn<MINPRNCMP || MAXPRNCMP<prn) return 0;
		return NSATGPS + prn - MINPRNCMP + 1;
	}
	return 0;
}
------------------------------*/
/* decode rangeb -------------------------------------------------------------*/
static int decode_rangeb(raw_t *rawData)
{
	double psr, adr, dop, snr, lockt, tt;
	char *msg;
	int i, index, nobs, prn, sat, sys, code, freq, pos;
	int track, plock, clock, parity, halfc, lli, gfrq;
	unsigned char *p = rawData->buff + OEM4HLEN;

	printf("decode_rangeb: len=%d\n", rawData->len);
	nobs = U4(p);

	if (rawData->outtype) {
		msg = rawData->msgtype + strlen(rawData->msgtype);
		sprintf(msg, " nobs=%2d", nobs);
	}

	if (rawData->len<OEM4HLEN + 4 + nobs * 44) {
		printf("oem4 rangeb length error: len=%d nobs=%d\n", rawData->len, nobs);
		return -1;
	}

	p += 4;
	for (i = 0; i<nobs; i++, p += 44) {

		/* decode tracking status */
		if ((freq = decode_trackstat(U4(p + 40), &sys, &code, &track, &plock, &clock,
			&parity, &halfc))<0) continue;

		/* obs position */
		if ((pos = checkpri(rawData->opt, sys, code, freq))<0) continue;

		prn = U2(p);
		if (sys == SYS_GLO) continue;
		if (!(sat = satno(sys, prn))) {
			continue;
		}

		if (sys == SYS_GLO&&!parity) continue; /* invalid if GLO parity unknown */

		gfrq = U2(p + 2);
		psr = R8(p + 4);
		adr = R8(p + 16);
		dop = R4(p + 28);
		snr = R4(p + 32);
		lockt = R4(p + 36);

		/* set glonass frequency channel number */

		tt = timediff(rawData->time, rawData->tobs);
		if (rawData->tobs.time != 0) {
			lli = lockt - rawData->lockt[sat - 1][pos] + 0.05 <= tt ||
				halfc != rawData->halfc[sat - 1][pos];
		}
		else {
			lli = 0;
		}
		if (!parity) lli |= 2;
		rawData->lockt[sat - 1][pos] = lockt;
		rawData->halfc[sat - 1][pos] = halfc;
		if (!clock) psr = 0.0;     /* code unlock */
		if (!plock) adr = dop = 0.0; /* phase unlock */

		if (fabs(timediff(rawData->obs.data[0].time, rawData->time))>1E-9) {
			rawData->obs.n = 0;
		}
		if ((index = obsindex(&rawData->obs, rawData->time, sat)) >= 0) {
			rawData->obs.data[index].L[pos] = -adr;
			rawData->obs.data[index].P[pos] = psr;
			rawData->obs.data[index].D[pos] = (float)dop;
			rawData->obs.data[index].SNR[pos] =
				0.0 <= snr&&snr<255.0 ? (unsigned char)(snr*4.0 + 0.5) : 0;
			rawData->obs.data[index].LLI[pos] = (unsigned char)lli;
			rawData->obs.data[index].code[pos] = code;
#if 0
			/* L2C phase shift correction */
			if (code == CODE_L2X) {
				raw->obs.data[index].L[pos] += 0.25;
				trace(3, "oem4 L2C phase shift corrected: prn=%2d\n", prn);
			}
#endif
		}
	}
	rawData->tobs = rawData->time;
	return 43;
}


/* decode rangecmpb ----------------------------------------------------------*/
static int decode_rangecmpb(raw_t *rawData)
{
	double psr, adr, adr_rolls, lockt, tt, dop, snr, wavelen;
	int i, index, nobs, prn, sat, sys, code, freq, pos;
	int track, plock, clock, parity, halfc, lli;
	char *msg;
	unsigned char *p = rawData->buff + OEM4HLEN;


	nobs = U4(p);

	if (rawData->outtype) {
		msg = rawData->msgtype + strlen(rawData->msgtype);
		sprintf(msg, " nobs=%2d", nobs);
	}
	if (rawData->len<OEM4HLEN + 4 + nobs * 24) {
		return -1;
	}
	for (i = 0, p += 4; i<nobs; i++, p += 24) {

		/* decode tracking status */
		if ((freq = decode_trackstat(U4(p), &sys, &code, &track, &plock, &clock,
			&parity, &halfc))<0) continue;

		/* obs position */
		if ((pos = checkpri(rawData->opt, sys, code, freq))<0) continue;

		prn = U1(p + 17);

		if (!(sat = satno(sys, prn))) {
			continue;
		}

		dop = exsign(U4(p + 4) & 0xFFFFFFF, 28) / 256.0;
		psr = (U4(p + 7) >> 4) / 128.0 + U1(p + 11)*2097152.0;

		adr = I4(p + 12) / 256.0;
		adr_rolls = (psr / wavelen + adr) / MAXVAL;
		adr = -adr + MAXVAL*floor(adr_rolls + (adr_rolls <= 0 ? -0.5 : 0.5));

		lockt = (U4(p + 18) & 0x1FFFFF) / 32.0; /* lock time */

		tt = timediff(rawData->time, rawData->tobs);
		if (rawData->tobs.time != 0) {
			lli = (lockt<65535.968&&lockt - rawData->lockt[sat - 1][pos] + 0.05 <= tt) ||
				halfc != rawData->halfc[sat - 1][pos];
		}
		else {
			lli = 0;
		}
		if (!parity) lli |= 2;
		rawData->lockt[sat - 1][pos] = lockt;
		rawData->halfc[sat - 1][pos] = halfc;

		snr = ((U2(p + 20) & 0x3FF) >> 5) + 20.0;
		if (!clock) psr = 0.0;     /* code unlock */
		if (!plock) adr = dop = 0.0; /* phase unlock */

		if (fabs(timediff(rawData->obs.data[0].time, rawData->time))>1E-9) {
			rawData->obs.n = 0;
		}
		if ((index = obsindex(&rawData->obs, rawData->time, sat)) >= 0) {
			rawData->obs.data[index].L[pos] = adr;
			rawData->obs.data[index].P[pos] = psr;
			rawData->obs.data[index].D[pos] = (float)dop;
			rawData->obs.data[index].SNR[pos] =
				0.0 <= snr&&snr<255.0 ? (unsigned char)(snr*4.0 + 0.5) : 0;
			rawData->obs.data[index].LLI[pos] = (unsigned char)lli;
			rawData->obs.data[index].code[pos] = code;

		}
	}
	rawData->tobs = rawData->time;
	return 43;
}

/* decode gps/qzss navigation data subframe 1 --------------------------------*/
static int decode_subfrm1(const unsigned char *buff, eph_t *eph)
{
	double tow, toc;
	int i = 48, week, iodc0, iodc1, tgd;

	tow = getbitu(buff, 24, 17)*6.0;           /* transmission time */
	week = getbitu(buff, i, 10);       i += 10;
	eph->code = getbitu(buff, i, 2);       i += 2;
	eph->sva = getbitu(buff, i, 4);       i += 4;   /* ura index */
	eph->svh = getbitu(buff, i, 6);       i += 6;
	iodc0 = getbitu(buff, i, 2);       i += 2;
	eph->flag = getbitu(buff, i, 1);       i += 1 + 87;
	tgd = getbits(buff, i, 8);       i += 8;
	iodc1 = getbitu(buff, i, 8);       i += 8;
	toc = getbitu(buff, i, 16)*16.0;  i += 16;
	eph->f2 = getbits(buff, i, 8)*P2_55; i += 8;
	eph->f1 = getbits(buff, i, 16)*P2_43; i += 16;
	eph->f0 = getbits(buff, i, 22)*P2_31;

	eph->tgd[0] = tgd == -128 ? 0.0 : tgd*P2_31; /* ref [4] */
	eph->iodc = (iodc0 << 8) + iodc1;
	eph->week = adjgpsweek(week); /* week of tow */
	eph->ttr = gpst2time(eph->week, tow);
	eph->toc = gpst2time(eph->week, toc);

	return 1;
}

/* decode gps/qzss navigation data subframe 2 --------------------------------*/
static int decode_subfrm2(const unsigned char *buff, eph_t *eph)
{
	double sqrtA;
	int i = 48;

	eph->iode = getbitu(buff, i, 8);              i += 8;
	eph->crs = getbits(buff, i, 16)*P2_5;         i += 16;
	eph->deln = getbits(buff, i, 16)*P2_43*SC2RAD; i += 16;
	eph->M0 = getbits(buff, i, 32)*P2_31*SC2RAD; i += 32;
	eph->cuc = getbits(buff, i, 16)*P2_29;        i += 16;
	eph->e = getbitu(buff, i, 32)*P2_33;        i += 32;
	eph->cus = getbits(buff, i, 16)*P2_29;        i += 16;
	sqrtA = getbitu(buff, i, 32)*P2_19;        i += 32;
	eph->toes = getbitu(buff, i, 16)*16.0;         i += 16;
	eph->fit = getbitu(buff, i, 1) ? 0.0 : 4.0; /* 0:4hr,1:>4hr */

	eph->A = sqrtA*sqrtA;

	return 2;
}

/* decode gps/qzss navigation data subframe 3 --------------------------------*/
static int decode_subfrm3(const unsigned char *buff, eph_t *eph)
{
	double tow, toc;
	int i = 48, iode;

	eph->cic = getbits(buff, i, 16)*P2_29;        i += 16;
	eph->OMG0 = getbits(buff, i, 32)*P2_31*SC2RAD; i += 32;
	eph->cis = getbits(buff, i, 16)*P2_29;        i += 16;
	eph->i0 = getbits(buff, i, 32)*P2_31*SC2RAD; i += 32;
	eph->crc = getbits(buff, i, 16)*P2_5;         i += 16;
	eph->omg = getbits(buff, i, 32)*P2_31*SC2RAD; i += 32;
	eph->OMGd = getbits(buff, i, 24)*P2_43*SC2RAD; i += 24;
	iode = getbitu(buff, i, 8);              i += 8;
	eph->idot = getbits(buff, i, 14)*P2_43*SC2RAD;

	/* check iode and iodc consistency */
	if (iode != eph->iode || iode != (eph->iodc & 0xFF)) return 0;

	/* adjustment for week handover */
	tow = time2gpst(eph->ttr, &eph->week);
	toc = time2gpst(eph->toc, NULL);
	if (eph->toes<tow - 302400.0) { eph->week++; tow -= 604800.0; }
	else if (eph->toes>tow + 302400.0) { eph->week--; tow += 604800.0; }
	eph->toe = gpst2time(eph->week, eph->toes);
	eph->toc = gpst2time(eph->week, toc);
	eph->ttr = gpst2time(eph->week, tow);

	return 3;
}

/* decode gps/qzss almanac ---------------------------------------------------*/
static void decode_almanac(const unsigned char *buff, int sat, alm_t *alm)
{
	gtime_t toa;
	double deltai, sqrtA, tt;
	int i = 50, f0;

	if (!alm || alm[sat - 1].week == 0) return;

	alm[sat - 1].sat = sat;
	alm[sat - 1].e = getbits(buff, i, 16)*P2_21;        i += 16;
	alm[sat - 1].toas = getbitu(buff, i, 8)*4096.0;       i += 8;
	deltai = getbits(buff, i, 16)*P2_19*SC2RAD; i += 16;
	alm[sat - 1].OMGd = getbits(buff, i, 16)*P2_38*SC2RAD; i += 16;
	alm[sat - 1].svh = getbitu(buff, i, 8);              i += 8;
	sqrtA = getbitu(buff, i, 24)*P2_11;        i += 24;
	alm[sat - 1].OMG0 = getbits(buff, i, 24)*P2_23*SC2RAD; i += 24;
	alm[sat - 1].omg = getbits(buff, i, 24)*P2_23*SC2RAD; i += 24;
	alm[sat - 1].M0 = getbits(buff, i, 24)*P2_23*SC2RAD; i += 24;
	f0 = getbitu(buff, i, 8);              i += 8;
	alm[sat - 1].f1 = getbits(buff, i, 11)*P2_38;        i += 11;
	alm[sat - 1].f0 = getbits(buff, i, 3)*P2_17 + f0*P2_20;
	alm[sat - 1].A = sqrtA*sqrtA;
	alm[sat - 1].i0 = 0.3*SC2RAD + deltai;

	toa = gpst2time(alm[sat - 1].week, alm[sat - 1].toas);
	tt = timediff(toa, alm[sat - 1].toa);
	if (tt<302400.0) alm[sat - 1].week--;
	else if (tt>302400.0) alm[sat - 1].week++;
	alm[sat - 1].toa = gpst2time(alm[sat - 1].week, alm[sat - 1].toas);
}
/* decode gps navigation data subframe 4 -------------------------------------*/
static void decode_gps_subfrm4(const unsigned char *buff, alm_t *alm,
	double *ion, double *utc, int *leaps)
{
	int i, sat, svid = getbitu(buff, 50, 6);

	if (25 <= svid&&svid <= 32) { /* page 2,3,4,5,7,8,9,10 */

								  /* decode almanac */
		sat = getbitu(buff, 50, 6);
		if (1 <= sat&&sat <= 32) decode_almanac(buff, sat, alm);
	}
	else if (svid == 63) { /* page 25 */

						   /* decode as and sv config */
		i = 56;
		for (sat = 1; sat <= 32; sat++) {
			if (alm) alm[sat - 1].svconf = getbitu(buff, i, 4); i += 4;
		}
		/* decode sv health */
		i = 186;
		for (sat = 25; sat <= 32; sat++) {
			if (alm) alm[sat - 1].svh = getbitu(buff, i, 6); i += 6;
		}
	}
	else if (svid == 56) { /* page 18 */

						   /* decode ion/utc parameters */
		if (ion) {
			i = 56;
			ion[0] = getbits(buff, i, 8)*P2_30;     i += 8;
			ion[1] = getbits(buff, i, 8)*P2_27;     i += 8;
			ion[2] = getbits(buff, i, 8)*P2_24;     i += 8;
			ion[3] = getbits(buff, i, 8)*P2_24;     i += 8;
			ion[4] = getbits(buff, i, 8)*pow(2, 11); i += 8;
			ion[5] = getbits(buff, i, 8)*pow(2, 14); i += 8;
			ion[6] = getbits(buff, i, 8)*pow(2, 16); i += 8;
			ion[7] = getbits(buff, i, 8)*pow(2, 16);
		}
		if (utc) {
			i = 120;
			utc[1] = getbits(buff, i, 24)*P2_50;     i += 24;
			utc[0] = getbits(buff, i, 32)*P2_30;     i += 32;
			utc[2] = getbits(buff, i, 8)*pow(2, 12); i += 8;
			utc[3] = getbitu(buff, i, 8);
		}
		if (leaps) {
			i = 192;
			*leaps = getbits(buff, i, 8);
		}
	}
}
/* decode gps navigation data subframe 5 -------------------------------------*/
static void decode_gps_subfrm5(const unsigned char *buff, alm_t *alm)
{
	double toas;
	int i, sat, week, svid = getbitu(buff, 50, 6);

	if (1 <= svid&&svid <= 24) { /* page 1-24 */

								 /* decode almanac */
		sat = getbitu(buff, 50, 6);
		if (1 <= sat&&sat <= 32) decode_almanac(buff, sat, alm);
	}
	else if (svid == 51) { /* page 25 */

		if (alm) {
			i = 56;
			toas = getbitu(buff, i, 8) * 4096; i += 8;
			week = getbitu(buff, i, 8);      i += 8;
			week = adjgpsweek(week);

			/* decode sv health */
			for (sat = 1; sat <= 24; sat++) {
				alm[sat - 1].svh = getbitu(buff, i, 6); i += 6;
			}
			for (sat = 1; sat <= 32; sat++) {
				alm[sat - 1].toas = toas;
				alm[sat - 1].week = week;
				alm[sat - 1].toa = gpst2time(week, toas);
			}
		}
	}
}

/* decode gps/qzss navigation data subframe 4 --------------------------------*/
static int decode_subfrm4(const unsigned char *buff, alm_t *alm, double *ion,
	double *utc, int *leaps)
{
	int dataid = getbitu(buff, 48, 2);

	if (dataid == 1) { /* gps */
		decode_gps_subfrm4(buff, alm, ion, utc, leaps);
	}
	return 4;
}

/* decode gps/qzss navigation data subframe 5 --------------------------------*/
static int decode_subfrm5(const unsigned char *buff, alm_t *alm, double *ion,
	double *utc, int *leaps)
{
	int dataid = getbitu(buff, 48, 2);

	if (dataid == 1) { /* gps */
		decode_gps_subfrm5(buff, alm);
	}
	return 5;
}


extern int decode_frame(const unsigned char *buff, eph_t *eph, alm_t *alm,
	double *ion, double *utc, int *leaps)
{
	int id = getbitu(buff, 43, 3); /* subframe id */

	switch (id) {
	case 1: return decode_subfrm1(buff, eph);
	case 2: return decode_subfrm2(buff, eph);
	case 3: return decode_subfrm3(buff, eph);
	case 4: return decode_subfrm4(buff, alm, ion, utc, leaps);
	case 5: return decode_subfrm5(buff, alm, ion, utc, leaps);
	}
	return 0;
}


/* decode rawephemb ----------------------------------------------------------*/
static int decode_rawephemb(raw_t *raw)
{
	unsigned char *p = raw->buff + OEM4HLEN;
	eph_t eph = { 0 };
	int prn, sat;

	if (raw->len<OEM4HLEN + 102) {
		return -1;
	}
	prn = U4(p);
	if (!(sat = satno(SYS_GPS, prn))) {
		return -1;
	}
	if (decode_frame(p + 12, &eph, NULL, NULL, NULL, NULL) != 1 ||
		decode_frame(p + 42, &eph, NULL, NULL, NULL, NULL) != 2 ||
		decode_frame(p + 72, &eph, NULL, NULL, NULL, NULL) != 3) {
		return -1;
	}
	if (!strstr(raw->opt, "-EPHALL")) {
		if (eph.iode == raw->nav.eph[sat - 1].iode) return 0; /* unchanged */
	}
	eph.sat = sat;
	raw->nav.eph[sat - 1] = eph;
	raw->ephsat = sat;
	return 2;
}

/* bdt to gpstime --------------------------------------------------------------
* convert bdt (beidou navigation satellite system time) to gpstime
* args   : gtime_t t        I   time expressed in bdt
* return : time expressed in gpstime
* notes  : see gpst2bdt()
*-----------------------------------------------------------------------------*/
extern gtime_t bdt2gpst(gtime_t t)
{
	return timeadd(t, 14.0);
}

/* beidou time (bdt) to time ---------------------------------------------------
* convert week and tow in beidou time (bdt) to gtime_t struct
* args   : int    week      I   week number in bdt
*          double sec       I   time of week in bdt (s)
* return : gtime_t struct
*-----------------------------------------------------------------------------*/
extern gtime_t bdt2time(int week, double sec)
{
	gtime_t t = epoch2time(bdt0);

	if (sec<-1E9 || 1E9<sec) sec = 0.0;
	t.time += 86400 * 7 * week + (int)sec;
	t.sec = sec - (int)sec;
	return t;
}


/* ura value (m) to ura index ------------------------------------------------*/
static int uraindex(double value)
{
	static const double ura_eph[] = {
		2.4,3.4,4.85,6.85,9.65,13.65,24.0,48.0,96.0,192.0,384.0,768.0,1536.0,
		3072.0,6144.0,0.0
	};
	int i;
	for (i = 0; i<15; i++) if (ura_eph[i] >= value) break;
	return i;
}


/* decode bdsephemerisb ------------------------------------------------------*/
static int decode_gpsephemerisb(raw_t *raw)
{
	//ephem_t eph = { 0 };
	eph_t eph = { 0 };
	unsigned char *p = raw->buff + OEM4HLEN;
	double ura, sqrtA, tow, correctN,A2;
	char *msg;
	int prn;

	unsigned int weeknum;
	int iode1, iode2, zweek;

	if (raw->len<OEM4HLEN + 224) {/* decode the whole length */
		return -1;
	}
	prn = U4(p);   p += 4;
	tow = R8(p);   p += 8;
	eph.svh = U4(p) & 1; p += 4;
	iode1 = U4(p); p += 4;
	iode2 = U4(p); p += 4;
	eph.iode = iode1;
	weeknum = U4(p);   p += 4;
	zweek = U4(p);   p += 4;
	eph.toes = R8(p);   p += 8;
	A2 = R8(p);   p += 8;
	eph.deln = R8(p);   p += 8;
	eph.M0 = R8(p);   p += 8;
	eph.e = R8(p);   p += 8;
	eph.omg = R8(p);   p += 8;
	eph.cuc = R8(p);   p += 8;
	eph.cus = R8(p);   p += 8;
	eph.crc = R8(p);   p += 8;
	eph.crs = R8(p);   p += 8;
	eph.cic = R8(p);   p += 8;
	eph.cis = R8(p);   p += 8;
	eph.i0 = R8(p);   p += 8;
	eph.idot = R8(p);   p += 8;
	eph.OMG0 = R8(p);   p += 8;
	eph.OMGd = R8(p);   p += 8;
	eph.iodc = U4(p);   p += 4; /* AODC */
	eph.tocs = R8(p);   p += 8;
	eph.tgd[0] = R8(p);   p += 8; /* TGD1 for B1 (s) */
	eph.tgd[1] = eph.tgd[0];
	eph.f0 = R8(p);   p += 8;
	eph.f1 = R8(p);   p += 8;
	eph.f2 = R8(p);   p += 12;
	//eph.AS = (bool)(U4(p));
	correctN = R8(p); p += 8;
	ura = R8(p);
	eph.A = sqrt(A2);
	eph.week = weeknum % 1024;
	eph.sva = uraindex(ura);
	if (raw->outtype) {
		msg = raw->msgtype + strlen(raw->msgtype);
		sprintf(msg, " prn=%3d iod=%3d toes=%6.0f", prn, eph.iode, eph.toes);
	}
	if (!(eph.sat = satno(SYS_GPS, prn))) {
		return -1;
	}
	eph.toe = gpst2time(eph.week, eph.toes); /* bdt -> gpst */
	eph.toc = gpst2time(eph.week, eph.tocs);      /* bdt -> gpst */
	eph.ttr = raw->time;

	if (!strstr(raw->opt, "-EPHALL")) {
		if (timediff(raw->nav.eph[eph.sat - 1].toe, eph.toe) == 0.0) return 0; /* unchanged */
	}
	raw->nav.eph[eph.sat - 1] = eph;
	raw->ephsat = eph.sat;
	return 7;
}


/* decode bdsephemerisb ------------------------------------------------------*/
static int decode_bdsephemerisb(raw_t *raw)
{
	//ephem_t eph = { 0 };
	eph_t eph = { 0 };
	unsigned char *p = raw->buff + OEM4HLEN;
	double ura, sqrtA, tow,correctN,A2;
	char *msg;
	int prn;
	unsigned int weeknum;

	int iode1, iode2, zweek;

	if (raw->len<OEM4HLEN + 224) {/* decode the whole length */
		return -1;
	}
	prn = U4(p);   p += 4;
	tow = R8(p);   p += 8;
	eph.svh = U4(p) & 1; p += 4;
	iode1 = U4(p); p += 4;
	iode2 = U4(p); p += 4;
	eph.iode = iode1;
	weeknum = U4(p);   p += 4;
	zweek = U4(p);   p += 4;
	eph.toes = R8(p);   p += 8;
	A2 = R8(p);   p += 8;
	eph.deln = R8(p);   p += 8;
	eph.M0 = R8(p);   p += 8;
	eph.e = R8(p);   p += 8;
	eph.omg = R8(p);   p += 8;
	eph.cuc = R8(p);   p += 8;
	eph.cus = R8(p);   p += 8;
	eph.crc = R8(p);   p += 8;
	eph.crs = R8(p);   p += 8;
	eph.cic = R8(p);   p += 8;
	eph.cis = R8(p);   p += 8;
	eph.i0 = R8(p);   p += 8;
	eph.idot = R8(p);   p += 8;
	eph.OMG0 = R8(p);   p += 8;
	eph.OMGd = R8(p);   p += 8;
	eph.iodc = U4(p);   p += 4; /* AODC */
	eph.tocs  = R8(p);   p += 8;
	eph.tgd[0] = R8(p);   p += 8; /* TGD1 for B1 (s) */
	eph.tgd[1] = eph.tgd[0];
	eph.f0 = R8(p);   p += 8;
	eph.f1 = R8(p);   p += 8;
	eph.f2 = R8(p);   p += 12;
//	eph.AS = (bool)(U4(p));
	correctN = R8(p); p += 8;
	ura = R8(p);  
	eph.A = sqrt(A2);
	eph.week = weeknum % 1024;
	eph.sva = uraindex(ura);

	if (raw->outtype) {
		msg = raw->msgtype + strlen(raw->msgtype);
		sprintf(msg, " prn=%3d iod=%3d toes=%6.0f", prn, eph.iode, eph.toes);
	}
	if (!(eph.sat = satno(SYS_CMP, prn))) {
		return -1;
	}
	eph.toe = bdt2gpst(bdt2time(eph.week, eph.toes)); /* bdt -> gpst */
	eph.toc = bdt2gpst(bdt2time(eph.week, eph.tocs));      /* bdt -> gpst */
	eph.ttr = raw->time;

	if (!strstr(raw->opt, "-EPHALL")) {
		if (timediff(raw->nav.eph[eph.sat - 1].toe, eph.toe) == 0.0) return 0; /* unchanged */
	}
	eph.fitinterval = 0;
	raw->nav.eph[eph.sat - 1] = eph;
	raw->ephsat = eph.sat;
	return 1047;
}

static int decode_oem4(raw_t *rawData)
{
	double tow;
	int msg, week, type = U2(rawData->buff + 4);
	unsigned int dd;
	unsigned int cc; 
	/* check crc32 */
	cc = crc32(rawData->buff, rawData->len);
	dd = U4(rawData->buff + rawData->len);
	if (crc32(rawData->buff, rawData->len) != U4(rawData->buff + rawData->len)) {
		printf("oem4 crc error: type=%3d len=%d\n", type, rawData->len);
		return -1;
	}
	msg = (U1(rawData->buff + 6) >> 4) & 0x3;
	week = adjgpsweek(U2(rawData->buff + 14));
	tow = U4(rawData->buff + 16)*0.001;
	rawData->time = gpst2time(week, tow);

	/**/
	if (rawData->outtype) {
		sprintf(rawData->msgtype, "OEM4 %4d (%4d): msg=%d %s", type, rawData->len, msg,
			time_str(gpst2time(week, tow), 2));
	}
	
	if (msg != 0) return 0; /* message type: 0=binary,1=ascii */

	switch (type) {
	case ID_RANGE: return decode_rangeb(rawData);
//	case ID_RAWEPHEM: return decode_rawephemb(rawData);
//	case ID_BDSEPHEMERIS: return decode_bdsephemerisb(rawData);
//	case ID_GPSEPHEMERIS: return decode_gpsephemerisb(rawData);
	}
	return 0;
}

extern int input_oem4(raw_t *raw, unsigned char data)
{
	/* synchronize frame */
	if (raw->nbyte == 0) {
		if (sync_oem4(raw->buff, data)) raw->nbyte = 3;
		return 0;
	}
	raw->buff[raw->nbyte++] = data;

	if (raw->nbyte == 10 && (raw->len = U2(raw->buff + 8) + OEM4HLEN)>MAXRAWLEN - 4) {
		raw->nbyte = 0;
		return -1;
	}
	if (raw->nbyte<10 || raw->nbyte<raw->len + 4) return 0;
	raw->nbyte = 0;

	/* decode oem4 message */
	return decode_oem4(raw);
}

double ScaleDoubleDate(double InputData, U8 BitNum)
{
	double  Result;
	if ((double)0 <= InputData)
	{
		Result = (InputData * (double)((U64)1 << BitNum)) + (double)0.5;
	}
	else
	{
		Result = (InputData * (double)((U64)1 << BitNum)) - (double)0.5;
	}

	return Result;
}

double ScaleDoubleDateDivPi(double InputData, unsigned int BitNum)
{
	double  Result;
	if ((double)0 <= InputData)
	{
		Result = ((InputData * (double)((U64)1 << BitNum)) / (double)_PI) + (double)0.5;
	}
	else
	{
		Result = ((InputData * (double)((U64)1 << BitNum)) / (double)_PI) - (double)0.5;
	}

	return Result;
}


/* gpstime to bdt --------------------------------------------------------------
* convert gpstime to bdt (beidou navigation satellite system time)
* args   : gtime_t t        I   time expressed in gpstime
* return : time expressed in bdt
* notes  : ref [8] 3.3, 2006/1/1 00:00 BDT = 2006/1/1 00:00 UTC
*          no leap seconds in BDT
*          ignore slight time offset under 100 ns
*-----------------------------------------------------------------------------*/
extern gtime_t gpst2bdt(gtime_t t)
{
	return timeadd(t, -14.0);
}

/* satellite no to msm satellite id ------------------------------------------*/
static int to_satid(int sys, int sat)
{
	int prn;
	if (satsys(sat, &prn) != sys) return 0;
	return prn;
}

/* lock time indicator (ref [2] table 3.4-2) ---------------------------------*/
static int to_lock(int lock)
{
	if (lock<0) return 0;
	if (lock<24) return lock;
	if (lock<72) return (lock + 24) / 2;
	if (lock<168) return (lock + 120) / 4;
	if (lock<360) return (lock + 408) / 8;
	if (lock<744) return (lock + 1176) / 16;
	if (lock<937) return (lock + 3096) / 32;
	return 127;
}

/* lock time -----------------------------------------------------------------*/
static int locktime(gtime_t time, gtime_t *lltime, unsigned char LLI)
{
	if (!lltime->time || (LLI & 1)) *lltime = time;
	return (int)timediff(time, *lltime);
}

/* obs code to obs code string -------------------------------------------------
* convert obs code to obs code string
* args   : unsigned char code I obs code (CODE_???)
*          int    *freq  IO     frequency (1:L1,2:L2,3:L5,4:L6,5:L7,6:L8,0:err)
*                               (NULL: no output)
* return : obs code string ("1C","1P","1P",...)
* notes  : obs codes are based on reference [6] and qzss extension
*-----------------------------------------------------------------------------*/
extern char *code2obs(unsigned char code, int *freq)
{
	if (freq) *freq = 0;
	if (code <= CODE_NONE || MAXCODE<code) return "";
	if (freq) *freq = obsfreqs[code];
	return obscodes[code];
}


/* observation code to msm signal id -----------------------------------------*/
static int to_sigid(int sys, unsigned char code, int *freq)
{
	const char **msm_sig;
	char *sig;
	int i;

	/* signal conversion for undefined signal by rtcm */
	if (sys == SYS_GPS) {
		if (code == CODE_L1Y) code = CODE_L1P;
		else if (code == CODE_L1M) code = CODE_L1P;
		else if (code == CODE_L1N) code = CODE_L1P;
		else if (code == CODE_L2D) code = CODE_L2P;
		else if (code == CODE_L2Y) code = CODE_L2P;
		else if (code == CODE_L2M) code = CODE_L2P;
		else if (code == CODE_L2N) code = CODE_L2P;
	}
	if (!*(sig = code2obs(code, freq))) return 0;

	switch (sys) {
	case SYS_GPS: msm_sig = msm_sig_gps; break;
	case SYS_CMP: msm_sig = msm_sig_cmp; break;
	default: return 0;
	}
	for (i = 0; i<32; i++) {
		if (!strcmp(sig, msm_sig[i])) return i + 1;
	}
	return 0;
}


/* generate msm satellite, signal and cell index -----------------------------*/
static void gen_msm_index(rtcm_t *rtcm, int sys, int *nsat, int *nsig,
	int *ncell, unsigned char *sat_ind,
	unsigned char *sig_ind, unsigned char *cell_ind)
{
	int i, j, sat, sig, cell, f;

	*nsat = *nsig = *ncell = 0;

	/* generate satellite and signal index */
	for (i = 0; i<rtcm->obs.n; i++) {
		if (!(sat = to_satid(sys, rtcm->obs.data[i].sat))) continue;

		for (j = 0; j<NFREQ + NEXOBS; j++) {
			if (!(sig = to_sigid(sys, rtcm->obs.data[i].code[j], &f))) continue;

			sat_ind[sat - 1] = sig_ind[sig - 1] = 1;
		}
	}
	for (i = 0; i<64; i++) {
		if (sat_ind[i]) sat_ind[i] = ++(*nsat);
	}
	for (i = 0; i<32; i++) {
		if (sig_ind[i]) sig_ind[i] = ++(*nsig);
	}
	/* generate cell index */
	for (i = 0; i<rtcm->obs.n; i++) {
		if (!(sat = to_satid(sys, rtcm->obs.data[i].sat))) continue;

		for (j = 0; j<NFREQ + NEXOBS; j++) {
			if (!(sig = to_sigid(sys, rtcm->obs.data[i].code[j], &f))) continue;

			cell = sig_ind[sig - 1] - 1 + (sat_ind[sat - 1] - 1)*(*nsig);
			cell_ind[cell] = 1;
		}
	}
	for (i = 0; i<*nsat*(*nsig); i++) {
		if (cell_ind[i] && *ncell<64) cell_ind[i] = ++(*ncell);
	}
}

/* satellite carrier wave length -----------------------------------------------
* get satellite carrier wave lengths
* args   : int    sat       I   satellite number
*          int    frq       I   frequency index (0:L1,1:L2,2:L5/3,...)
*          nav_t  *nav      I   navigation messages
* return : carrier wave length (m) (0.0: error)
*-----------------------------------------------------------------------------*/
extern double satwavelen(int sat, int frq, const nav_t *nav)
{
	int i, sys = satsys(sat, NULL);

	if (sys == SYS_CMP) {
		if (frq == 0) return CLIGHT / FREQ1_CMP; /* B1 */
	}
	else {
		if (frq == 0) return CLIGHT / FREQ1; /* L1/E1 */
	}
	return 0.0;
}


/* generate msm satellite data fields ----------------------------------------*/
static void gen_msm_sat(rtcm_t *rtcm, int sys, int nsat,
	const unsigned char *sat_ind, double *rrng,
	double *rrate, unsigned char *info)
{
	obsd_t *data;
	double lambda, rrng_s, rrate_s;
	int i, j, k, sat, sig, f, fcn;

	for (i = 0; i<64; i++) rrng[i] = rrate[i] = 0.0;

	for (i = 0; i<rtcm->obs.n; i++) {
		data = rtcm->obs.data + i;
		if (!(sat = to_satid(sys, data->sat))) continue;
		//fcn = fcn_glo(data->sat, rtcm);

		for (j = 0; j<NFREQ + NEXOBS; j++) {
			if (!(sig = to_sigid(sys, data->code[j], &f))) continue;
			k = sat_ind[sat - 1] - 1;
			lambda = satwavelen(data->sat, f - 1, &rtcm->nav);

			/* rough range (ms) and rough phase-range-rate (m/s) */
			rrng_s = ROUND(data->P[j] / RANGE_MS / P2_10)*RANGE_MS*P2_10;
			rrate_s = ROUND(-data->D[j] * lambda)*1.0;
			if (rrng[k] == 0.0&&data->P[j] != 0.0) rrng[k] = rrng_s;
			if (rrate[k] == 0.0&&data->D[j] != 0.0) rrate[k] = rrate_s;

			/* extended satellite info */
		   // info[k] = sys != SYS_GLO ? 0 : (fcn<0 ? 15 : fcn);
		}
	}
}

/* generate msm signal data fields -------------------------------------------*/
static void gen_msm_sig(rtcm_t *rtcm, int sys, int nsat, int nsig, int ncell,
	const unsigned char *sat_ind,
	const unsigned char *sig_ind,
	const unsigned char *cell_ind, const double *rrng,
	const double *rrate, double *psrng, double *phrng,
	double *rate, int *lock, unsigned char *half,
	float *cnr)
{
	obsd_t *data;
	double lambda, psrng_s, phrng_s, rate_s;
	int i, j, k, sat, sig, cell, f, lt, LLI;

	for (i = 0; i<ncell; i++) {
		if (psrng) psrng[i] = 0.0;
		if (phrng) phrng[i] = 0.0;
		if (rate) rate[i] = 0.0;
	}
	for (i = 0; i<rtcm->obs.n; i++) {
		data = rtcm->obs.data + i;

		if (!(sat = to_satid(sys, data->sat))) continue;

		for (j = 0; j<NFREQ + NEXOBS; j++) {
			if (!(sig = to_sigid(sys, data->code[j], &f))) continue;
			k = sat_ind[sat - 1] - 1;
			if ((cell = cell_ind[sig_ind[sig - 1] - 1 + k*nsig]) >= 64) continue;

			lambda = satwavelen(data->sat, f - 1, &rtcm->nav);
			psrng_s = data->P[j] == 0.0 ? 0.0 : data->P[j] - rrng[k];
			phrng_s = data->L[j] == 0.0 || lambda <= 0.0 ? 0.0 : data->L[j] * lambda - rrng[k];
			rate_s = data->D[j] == 0.0 || lambda <= 0.0 ? 0.0 : -data->D[j] * lambda - rrate[k];

			/* subtract phase - psudorange integer cycle offset */
			LLI = data->LLI[j];
			if ((LLI & 1) || fabs(phrng_s - rtcm->cp[data->sat - 1][j])>1171.0) {
				rtcm->cp[data->sat - 1][j] = ROUND(phrng_s / lambda)*lambda;
				LLI |= 1;
			}
			phrng_s -= rtcm->cp[data->sat - 1][j];

			lt = locktime(data->time, rtcm->lltime[data->sat - 1] + j, LLI);

			if (psrng&&psrng_s != 0.0) psrng[cell - 1] = psrng_s;
			if (phrng&&phrng_s != 0.0) phrng[cell - 1] = phrng_s;
			if (rate &&rate_s != 0.0) rate[cell - 1] = rate_s;
			if (lock) lock[cell - 1] = lt;
			if (half) half[cell - 1] = (data->LLI[j] & 2) ? 1 : 0;
			if (cnr) cnr[cell - 1] = (float)(data->SNR[j] * 0.25);
		}
	}
}

/* encode msm header ---------------------------------------------------------*/
static int encode_msm_head(int type, rtcm_t *rtcm, int sys, int sync, int *nsat,
	int *ncell, double *rrng, double *rrate,
	unsigned char *info, double *psrng, double *phrng,
	double *rate, int *lock, unsigned char *half,
	float *cnr)
{
	double tow;
	unsigned char sat_ind[64] = { 0 }, sig_ind[32] = { 0 }, cell_ind[32 * 64] = { 0 };
	unsigned int dow, epoch;
	int i = 24, j, tt, nsig = 0;

	switch (sys) {
	case SYS_GPS: type += 1070; break;
	case SYS_CMP: type += 1120; break;
	default: return 0;
	}
	/* generate msm satellite, signal and cell index */
	gen_msm_index(rtcm, sys, nsat, &nsig, ncell, sat_ind, sig_ind, cell_ind);

	if (sys == SYS_GLO) {
		/* glonass time (dow + tod-ms) */
		tow = time2gpst(timeadd(gpst2utc(rtcm->time), 10800.0), NULL);
		dow = (unsigned int)(tow / 86400.0);
		epoch = (dow << 27) + ROUND_U(fmod(tow, 86400.0)*1E3);
	}
	else if (sys == SYS_CMP) {
		/* beidou time (tow-ms) */
		epoch = ROUND_U(time2gpst(gpst2bdt(rtcm->time), NULL)*1E3);
	}
	else {
		/* gps, qzs and galileo time (tow-ms) */
		epoch = ROUND_U(time2gpst(rtcm->time, NULL)*1E3);
	}
	/* cumulative session transmitting time (s) */
	tt = locktime(rtcm->time, &rtcm->time_s, 0);

	/* encode msm header (ref [11] table 3.5-73) */
	setbitu(rtcm->buff, i, 12, type); i += 12; /* message number */
	setbitu(rtcm->buff, i, 12, rtcm->staid); i += 12; /* reference station id */
	setbitu(rtcm->buff, i, 30, epoch); i += 30; /* epoch time */
	setbitu(rtcm->buff, i, 1, sync); i += 1; /* multiple message bit */
	setbitu(rtcm->buff, i, 3, rtcm->seqno); i += 3; /* issue of data station */
	setbitu(rtcm->buff, i, 7, to_lock(tt)); i += 7; /* session time indicator */
	setbitu(rtcm->buff, i, 2, 0); i += 2; /* clock streering indicator */
	setbitu(rtcm->buff, i, 2, 0); i += 2; /* external clock indicator */
	setbitu(rtcm->buff, i, 1, 0); i += 1; /* smoothing indicator */
	setbitu(rtcm->buff, i, 3, 0); i += 3; /* smoothing interval */

										  /* satellite mask */
	for (j = 0; j<64; j++) {
		setbitu(rtcm->buff, i, 1, sat_ind[j] ? 1 : 0); i += 1;
	}
	/* signal mask */
	for (j = 0; j<32; j++) {
		setbitu(rtcm->buff, i, 1, sig_ind[j] ? 1 : 0); i += 1;
	}
	/* cell mask */
	for (j = 0; j<*nsat*nsig&&j<64; j++) {
		setbitu(rtcm->buff, i, 1, cell_ind[j] ? 1 : 0); i += 1;
	}
	/* generate msm satellite data fields */
	gen_msm_sat(rtcm, sys, *nsat, sat_ind, rrng, rrate, info);

	/* generate msm signal data fields */
	gen_msm_sig(rtcm, sys, *nsat, nsig, *ncell, sat_ind, sig_ind, cell_ind, rrng, rrate,
		psrng, phrng, rate, lock, half, cnr);

	return i;
}

/* msm lock time indicator (ref [11] table 3.4-1D) ---------------------------*/
static int to_msm_lock(int lock)
{
	if (lock<32) return 0;
	if (lock<64) return 1;
	if (lock<128) return 2;
	if (lock<256) return 3;
	if (lock<512) return 4;
	if (lock<1024) return 5;
	if (lock<2048) return 6;
	if (lock<4096) return 7;
	if (lock<8192) return 8;
	if (lock<16384) return 9;
	if (lock<32768) return 10;
	if (lock<65536) return 11;
	if (lock<131072) return 12;
	if (lock<262144) return 13;
	if (lock<524288) return 14;
	return 15;
}

/* encode signal cnr ---------------------------------------------------------*/
static int encode_msm_cnr(rtcm_t *rtcm, int i, const float *cnr, int ncell)
{
	int j, cnr_val;

	for (j = 0; j<ncell; j++) {
		cnr_val = ROUND(cnr[j] / 1.0);
		setbitu(rtcm->buff, i, 6, cnr_val); i += 6;
	}
	return i;
}

/* encode half-cycle-ambiguity indicator -------------------------------------*/
static int encode_msm_half_amb(rtcm_t *rtcm, int i, const unsigned char *half,
	int ncell)
{
	int j;

	for (j = 0; j<ncell; j++) {
		setbitu(rtcm->buff, i, 1, half[j]); i += 1;
	}
	return i;
}

/* encode lock-time indicator ------------------------------------------------*/
static int encode_msm_lock(rtcm_t *rtcm, int i, const int *lock, int ncell)
{
	int j, lock_val;

	for (j = 0; j<ncell; j++) {
		lock_val = to_msm_lock(lock[j]);
		setbitu(rtcm->buff, i, 4, lock_val); i += 4;
	}
	return i;
}

extern void setbits(unsigned char *buff, int pos, int len, int data)
{
	if (data<0) data |= 1 << (len - 1); else data &= ~(1 << (len - 1)); /* set sign bit */
	setbitu(buff, pos, len, (unsigned int)data);
}

/* encode fine phase-range ---------------------------------------------------*/
static int encode_msm_phrng(rtcm_t *rtcm, int i, const double *phrng, int ncell)
{
	int j, phrng_val;

	for (j = 0; j<ncell; j++) {
		if (phrng[j] == 0.0) {
			phrng_val = -2097152;
		}
		else if (fabs(phrng[j])>1171.0) {
			phrng_val = -2097152;
		}
		else {
			phrng_val = ROUND(phrng[j] / RANGE_MS / P2_29);
		}
		setbits(rtcm->buff, i, 22, phrng_val); i += 22;
	}
	return i;
}

/* encode fine pseudorange ---------------------------------------------------*/
static int encode_msm_psrng(rtcm_t *rtcm, int i, const double *psrng, int ncell)
{
	int j, psrng_val;

	for (j = 0; j<ncell; j++) {
		if (psrng[j] == 0.0) {
			psrng_val = -16384;
		}
		else if (fabs(psrng[j])>292.7) {
			psrng_val = -16384;
		}
		else {
			psrng_val = ROUND(psrng[j] / RANGE_MS / P2_24);
		}
		setbits(rtcm->buff, i, 15, psrng_val); i += 15;
	}
	return i;
}


/* encode rough range modulo 1 ms --------------------------------------------*/
static int encode_msm_mod_rrng(rtcm_t *rtcm, int i, const double *rrng,
	int nsat)
{
	unsigned int mod_ms;
	int j;

	for (j = 0; j<nsat; j++) {
		if (rrng[j] <= 0.0 || rrng[j]>RANGE_MS*255.0) {
			mod_ms = 0;
		}
		else {
			mod_ms = ROUND_U(rrng[j] / RANGE_MS / P2_10) & 0x3FFu;
		}
		setbitu(rtcm->buff, i, 10, mod_ms); i += 10;
	}
	return i;
}

/* encode rough range integer ms ---------------------------------------------*/
static int encode_msm_int_rrng(rtcm_t *rtcm, int i, const double *rrng,
	int nsat)
{
	unsigned int int_ms;
	int j;

	for (j = 0; j<nsat; j++) {
		if (rrng[j] == 0.0) {
			int_ms = 255;
		}
		else if (rrng[j]<0.0 || rrng[j]>RANGE_MS*255.0) {
			int_ms = 255;
		}
		else {
			int_ms = ROUND_U(rrng[j] / RANGE_MS / P2_10) >> 10;
		}
		setbitu(rtcm->buff, i, 8, int_ms); i += 8;
	}
	return i;
}

/* encode msm 4: full pseudorange and phaserange plus cnr --------------------*/
static int encode_msm4(rtcm_t *rtcm, int sys, int sync)
{
	double rrng[64], rrate[64], psrng[64], phrng[64];
	float cnr[64];
	unsigned char half[64];
	int i, nsat, ncell, lock[64];

	/* encode msm header */
	if (!(i = encode_msm_head(4, rtcm, sys, sync, &nsat, &ncell, rrng, rrate, NULL, psrng,
		phrng, NULL, lock, half, cnr))) {
		return 0;
	}
	/* encode msm satellite data */
	i = encode_msm_int_rrng(rtcm, i, rrng, nsat); /* rough range integer ms */
	i = encode_msm_mod_rrng(rtcm, i, rrng, nsat); /* rough range modulo 1 ms */

												  /* encode msm signal data */
	i = encode_msm_psrng(rtcm, i, psrng, ncell); /* fine pseudorange */
	i = encode_msm_phrng(rtcm, i, phrng, ncell); /* fine phase-range */
	i = encode_msm_lock(rtcm, i, lock, ncell); /* lock-time indicator */
	i = encode_msm_half_amb(rtcm, i, half, ncell); /* half-cycle-amb indicator */
	i = encode_msm_cnr(rtcm, i, cnr, ncell); /* signal cnr */
	rtcm->nbit = i;
	return 1;
}


/* encode rtcm ver.3 message -------------------------------------------------*/
extern int encode_rtcm3(rtcm_t *rtcm, int type, int sync)
{
	int ret = 0;

	switch (type) {
	case 1074: ret = encode_msm4(rtcm, SYS_GPS, sync); break;
	case 1124: ret = encode_msm4(rtcm, SYS_CMP, sync); break;
	}
	if (ret>0) {
		type -= 1000;
		if (1 <= type&&type <= 299) rtcm->nmsg3[type]++; else rtcm->nmsg3[0]++;
	}
	return ret;
}

/* crc-24q parity --------------------------------------------------------------
* compute crc-24q parity for sbas, rtcm3
* args   : unsigned char *buff I data
*          int    len    I      data length (bytes)
* return : crc-24Q parity
* notes  : see reference [2] A.4.3.3 Parity
*-----------------------------------------------------------------------------*/
extern unsigned int crc24q(const unsigned char *buff, int len)
{
	unsigned int crc = 0;
	int i;

	for (i = 0; i<len; i++) crc = ((crc << 8) & 0xFFFFFF) ^ tbl_CRC24Q[(crc >> 16) ^ buff[i]];
	return crc;
}

/* generate rtcm 3 message -----------------------------------------------------
* generate rtcm 3 message
* args   : rtcm_t *rtcm   IO rtcm control struct
*          int    type    I  message type
*          int    sync    I  sync flag (1:another message follows)
* return : status (1:ok,0:error)
*-----------------------------------------------------------------------------*/
extern int gen_rtcm3(rtcm_t *rtcm, int type, int sync)
{
	unsigned int crc;
	int i = 0;
	rtcm->nbit = rtcm->len = rtcm->nbyte = 0;

	/* set preamble and reserved */
	setbitu(rtcm->buff, i, 8, RTCM3PREAMB); i += 8;
	setbitu(rtcm->buff, i, 6, 0); i += 6;
	setbitu(rtcm->buff, i, 10, 0); i += 10;

	/* encode rtcm 3 message body */
	if (!encode_rtcm3(rtcm, type, sync)) return 0;

	/* padding to align 8 bit boundary */
	for (i = rtcm->nbit; i % 8; i++) {
		setbitu(rtcm->buff, i, 1, 0);
	}
	/* message length (header+data) (bytes) */
	if ((rtcm->len = i / 8) >= 3 + 1024) {
		rtcm->nbit = rtcm->len = 0;
		return 0;
	}
	/* message length without header and parity */
	setbitu(rtcm->buff, 14, 10, rtcm->len - 3);

	/* crc-24q */
	crc = crc24q(rtcm->buff, rtcm->len);
	setbitu(rtcm->buff, i, 24, crc);

	/* length total (bytes) */
	rtcm->nbyte = rtcm->len + 3;

	return 1;
}

/* set signed 38 bit field ---------------------------------------------------*/
static void set38bits(unsigned char *buff, int pos, double value)
{
	int word_h = (int)floor(value / 64.0);
	unsigned int word_l = (unsigned int)(value - word_h*64.0);
	setbits(buff, pos, 32, word_h);
	setbitu(buff, pos + 32, 6, word_l);
}

/* test time interval --------------------------------------------------------*/
static int is_tint(gtime_t time, double tint)
{
	if (tint <= 0.0) return 1;
	return fmod(time2gpst(time, NULL) + DTTOL, tint) <= 2.0*DTTOL;
}


/* encode type 1005: stationary rtk reference station arp --------------------*/
static int encode_type1005(rtcm_t *rtcm, int sync)
{
	double p[3] = {-2271599.5880,5009141.3718,3218828.1104};

	int i = 0;
	unsigned int crc = 0;
	rtcm->nbit = rtcm->len = rtcm->nbyte = 0;
	/* set preamble and reserved */
	setbitu(rtcm->buff, i, 8, RTCM3PREAMB); i += 8;
	setbitu(rtcm->buff, i, 6, 0); i += 6;
	setbitu(rtcm->buff, i, 10, 0); i += 10;

	setbitu(rtcm->buff, i, 12, 1005); i += 12; /* message no */
	setbitu(rtcm->buff, i, 12, rtcm->staid); i += 12; /* ref station id */
	setbitu(rtcm->buff, i, 6, 0); i += 6; /* itrf realization year */
	setbitu(rtcm->buff, i, 1, 1); i += 1; /* gps indicator */
	setbitu(rtcm->buff, i, 1, 1); i += 1; /* glonass indicator */
	setbitu(rtcm->buff, i, 1, 0); i += 1; /* galileo indicator */
	setbitu(rtcm->buff, i, 1, 0); i += 1; /* ref station indicator */
	set38bits(rtcm->buff, i, p[0] / 0.0001); i += 38; /* antenna ref point ecef-x */
	setbitu(rtcm->buff, i, 1, 1); i += 1; /* oscillator indicator */
	setbitu(rtcm->buff, i, 1, 0); i += 1; /* reserved */
	set38bits(rtcm->buff, i, p[1] / 0.0001); i += 38; /* antenna ref point ecef-y */
	setbitu(rtcm->buff, i, 2, 0); i += 2; /* quarter cycle indicator */
	set38bits(rtcm->buff, i, p[2] / 0.0001); i += 38; /* antenna ref point ecef-z */
	rtcm->nbit = i;

	/* padding to align 8 bit boundary */
	for (i = rtcm->nbit; i % 8; i++) {
		setbitu(rtcm->buff, i, 1, 0);
	}
	/* message length (header+data) (bytes) */
	if ((rtcm->len = i / 8) >= 3 + 1024) {
		rtcm->nbit = rtcm->len = 0;
		return 0;
	}
	/* message length without header and parity */
	setbitu(rtcm->buff, 14, 10, rtcm->len - 3);

	/* crc-24q */
	crc = crc24q(rtcm->buff, rtcm->len);
	setbitu(rtcm->buff, i, 24, crc);

	/* length total (bytes) */
	//	rtcm->nbyte = rtcm->len;
	rtcm->nbyte = rtcm->len + 3;

	return 1;
}

/* copy received data from receiver raw to rtcm ------------------------------*/
static void raw2rtcm(rtcm_t *gpsout,rtcm_t *bdsout, const raw_t *raw, int ret)
{
	int i, sat, prn,u;
	int gpsobs = 0;
	int bdsobs = 0;

	gpsout->time = raw->time;
	bdsout->time = raw->time;
	int gf = 0;
	int bf = 0;
	if (ret == 43) {
		for (i = 0; i<raw->obs.n; i++) {
			if (raw->obs.data[i].sat <84) {
				gpsout->time = raw->obs.data[i].time;
				gpsout->obs.data[gf] = raw->obs.data[i];
				gf = gf + 1;
			}
			else {
				bdsout->time = raw->obs.data[i].time;
				bdsout->obs.data[bf] = raw->obs.data[i];
				bf = bf + 1;
			}
		}
		gpsout->obs.n = gf;
		bdsout->obs.n = bf;
	}
#if 0
	for (u = 0; u < gpsout->obs.n; u++) {
		printf("GPSPRN:%d psr:%7.8lf L:%7.8lf D:%7.8lf SNR:%d \n", gpsout->obs.data[u].sat, gpsout->obs.data[u].P[0], gpsout->obs.data[u].L[0], gpsout->obs.data[u].D[0], gpsout->obs.data[u].SNR[0]);
	}

	for (u = 0; u < bdsout->obs.n; u++) {
		printf("BDSPRN:%d psr:%7.8lf L:%7.8lf D:%7.8lf SNR:%d \n", bdsout->obs.data[u].sat, bdsout->obs.data[u].P[0], bdsout->obs.data[u].L[0], bdsout->obs.data[u].D[0], bdsout->obs.data[u].SNR[0]);
	}
#endif
}


void AssistData_Input(raw_t* rawData, int recid, char* buff, int len)
{
	char StrFormat[16] = { "" };
	char OutString[256] = { "" };

	U8 bCheckSum = 0;
	char navSysbds = 2;
	char navSygps = 1;
	char datatype = 1;
	int i, prn, j, u;
	int bdsprn = 0;
	int ret, ret_decode;
	int nmsg = 2;
	int k, m = 2;
	static int mestype[2] = { 1074,1124 };//1074 and 1124 message type
	int begin, end;

	for (i = 0; i < len; i++) {
		ret = input_oem4(rawData, buff[i]);

		if (43 == ret) {
			//存储rtcm3e格式文件;
			raw2rtcm(&rawData->gps_dgnssdata, &rawData->bds_dgnssdata, rawData, ret);

			encode_type1005(&rawData->refstation, 1);

			for (k = 0; k < nmsg; k++) {
				if (0 == k) {
					if (!gen_rtcm3(&rawData->gps_dgnssdata, mestype[k], 0)) continue;
				}
				else if (1 == k) {
					if (!gen_rtcm3(&rawData->bds_dgnssdata, mestype[k], 1)) continue;
				}
			}

			/**/
			if ((rawData->bds_dgnssdata.obs.n > 0 && rawData->gps_dgnssdata.obs.n > 0)) {

				rawData->dgnssDat.dgnss_flag = 1;

				rawData->dgnssDat.ref_stationlen = rawData->refstation.nbyte;
				memcpy(rawData->dgnssDat.dgnss_refstat, rawData->refstation.buff, rawData->dgnssDat.ref_stationlen);

				rawData->dgnssDat.dgnss_gpslen = rawData->gps_dgnssdata.nbyte;
				memcpy(rawData->dgnssDat.dgnss_gps, rawData->gps_dgnssdata.buff, rawData->dgnssDat.dgnss_gpslen);

				rawData->dgnssDat.dgnss_bdslen = rawData->bds_dgnssdata.nbyte;
				memcpy(rawData->dgnssDat.dgnss_bds, rawData->bds_dgnssdata.buff, rawData->dgnssDat.dgnss_bdslen);

				rawData->gps_dgnssdata.obs.n = 0;
				rawData->bds_dgnssdata.obs.n = 0;
			}

			for (j = 0; j < 200; j++) {
				rawData->refstation.buff[j] = '\0';
			}

			for (j = 0; j < 1200; j++) {
				rawData->bds_dgnssdata.buff[j] = '\0';
				rawData->gps_dgnssdata.buff[j] = '\0';
			}
			/**/
		}


	}
}






















