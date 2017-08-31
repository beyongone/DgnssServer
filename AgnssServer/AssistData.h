#ifndef ASSISTDATA_H_
#define ASSISTDATA_H_

//////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C"
{
#endif

#define ENAQZS
#define ENAGLO
#define ENAQZS
#define ENACMP
#define ENAGAL
#ifndef NFREQ
#define NFREQ       3                   /* number of carrier frequencies */
#endif

#ifndef NEXOBS
#define NEXOBS      0                   /* number of extended obs codes */
#endif

#define MINPRNGPS   1                   /* min satellite PRN number of GPS */
#define MAXPRNGPS   32                  /* max satellite PRN number of GPS */
#define NSATGPS     (MAXPRNGPS-MINPRNGPS+1) /* number of GPS satellites */

#ifdef ENACMP
#define MINPRNCMP   161                  /* min satellite sat number of BeiDou */
#define MAXPRNCMP   197                 /* max satellite sat number of BeiDou */
#define NSATCMP     (MAXPRNCMP-MINPRNCMP+1) /* number of BeiDou satellites */
#define NSYSCMP     1
#else
#define MINPRNCMP   0
#define MAXPRNCMP   0
#define NSATCMP     0
#define NSYSCMP     0
#endif

#define MINPRNGLO   1                   /* min satellite slot number of GLONASS */
#define MAXPRNGLO   24                  /* max satellite slot number of GLONASS */
#define NSATGLO     (MAXPRNGLO-MINPRNGLO+1) /* number of GLONASS satellites */
#define NSYSGLO     1


#ifdef ENAGAL
#define MINPRNGAL   1                   /* min satellite PRN number of Galileo */
#define MAXPRNGAL   27                  /* max satellite PRN number of Galileo */
#define NSATGAL    (MAXPRNGAL-MINPRNGAL+1) /* number of Galileo satellites */
#define NSYSGAL     1
#else
#define MINPRNGAL   0
#define MAXPRNGAL   0
#define NSATGAL     0
#define NSYSGAL     0
#endif


#ifdef ENAQZS
#define MINPRNQZS   193                 /* min satellite PRN number of QZSS */
#define MAXPRNQZS   199                 /* max satellite PRN number of QZSS */
#define MINPRNQZS_S 183                 /* min satellite PRN number of QZSS SAIF */
#define MAXPRNQZS_S 189                 /* max satellite PRN number of QZSS SAIF */
#define NSATQZS     (MAXPRNQZS-MINPRNQZS+1) /* number of QZSS satellites */
#define NSYSQZS     1
#else
#define MINPRNQZS   0
#define MAXPRNQZS   0
#define MINPRNQZS_S 0
#define MAXPRNQZS_S 0
#define NSATQZS     0
#define NSYSQZS     0
#endif

//#define MAXSAT      (NSATGPS+NSATCMP)
#define MAXSAT      (NSATGPS+NSATGLO+NSATGAL+NSATQZS+NSATCMP)
#define MAXANT      64                  /* max length of station name/antenna type */
#define MAXCODE     48                  /* max number of obs code */

#ifndef MAXOBS
#define MAXOBS      64                  /* max number of obs in an epoch */
#endif
	/* max satellite number (1 to MAXSAT) */

#define MAXRAWLEN   4096                /* max length of receiver raw message */

#define MAXSTRPATH  1024                /* max length of stream path */

/////////////////////////////////////////////////////////////////////////////

typedef unsigned long long U64;
typedef unsigned char U8;
typedef long int S32;
typedef unsigned long int U32;
typedef unsigned short U16;
typedef signed short S16;
typedef unsigned char U8;
typedef signed char S8;

typedef struct _tagUTCTM {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int sec;
}UTCTM;

typedef struct _tagAIDIniParam
{
    int basetime;
    int flag;
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int sec;
    int minsec;
    double secacc;
    double lat;
    double lon;
    double alt;
    double posacc;
    unsigned int bdsmask_low;
    unsigned int bdsmask_high;
    unsigned int gpsmask;
}AIDIniParam, *pAIDIniParam;


#define DECODE_BUFF_LEN     (1200)

typedef struct _tagADdecode {
    int aideph_gpscnt;
	int aideph_bdscnt;
	unsigned char aidupdate;
    char aideph_gps[32][256];
    char aideph_bds[14][256];
	unsigned int bdsmask_low;
	unsigned int bdsmask_high;
	unsigned int gpsmask;
} ADdecode;

typedef struct _tagDgnssData {
	int dgnss_flag;

	int ref_stationlen;
	char dgnss_refstat[200];

	int dgnss_gpslen;
	char dgnss_gps[1200];

	int dgnss_bdslen;
	char dgnss_bds[1200];
} DgnssDat;

typedef struct _tagAssistData {
    long long manage_tick;

    long long manage_tick_data;

    unsigned int gpsmask;
    unsigned int bdsmask_low;
    unsigned int bdsmask_high;

    int aideph_gps_len;
    char aideph_gps[4000];

    int aideph_bds_len;
    char aideph_bds[4000];

    int aideph_gpsbds_len;
    char aideph_gpsbds[8000];

	int all_aideph_gpsbds_len;
	char all_aideph_gpsbds[9000];

    ADdecode ADdec;
	int aidflag;

} AssistData;
/*
//TureCORS Receiver Header
typedef struct _tagTureCORS_Log_Header {
	char firstSync;
	char secondSync;
	char thirdSync;

	unsigned char HeaderLen;
	unsigned short int messageID;//2byte

	char messageType;// 00 = Binary;01 = ASCII;10 = Abbreviated ASCII;
	unsigned char portAddress;

	unsigned short int messagelen;//length of message(Bytes),not include log header and CRC bit;
	unsigned short int reservedMesg;//Reserved

	//Porcessor idle time betwween two logs with same message ID in the last second;
	//Time percentage (0~100%) is calcuated as time(0~200) divided by 2;
	unsigned char idleTime;

	//GPS time quality
	enum timeStatus {good = 1,bad = 2};
	unsigned short int gpsWeek;
	unsigned long int gpsWeekms;

	unsigned long int reservedUlong;

	unsigned short int bd2LeapSec;
	unsigned short int reservedUShort;
	unsigned char buff[DECODE_BUFF_LEN];

} TureCORS_Log_Header;
*/
/* type definitions ----------------------------------------------------------*/
typedef struct {        /* time struct */
	time_t time;        /* time (s) expressed by standard time_t */
	double sec;         /* fraction of second under 1 s */
} gtime_t;

typedef struct {        /* observation data record */
	gtime_t time;       /* receiver sampling time (GPST) */
	unsigned char sat, rcv; /* satellite/receiver number */
	unsigned char SNR[NFREQ + NEXOBS]; /* signal strength (0.25 dBHz) */
	unsigned char LLI[NFREQ + NEXOBS]; /* loss of lock indicator */
	unsigned char code[NFREQ + NEXOBS]; /* code indicator (CODE_???) */
	double L[NFREQ + NEXOBS]; /* observation data carrier-phase (cycle) */
	double P[NFREQ + NEXOBS]; /* observation data pseudorange (m) */
	float  D[NFREQ + NEXOBS]; /* observation data doppler frequency (Hz) */
} obsd_t;

typedef struct {        /* observation data */
	int n, nmax;         /* number of obervation data/allocated */
	obsd_t *data;       /* observation data records */
} obs_t;

//typedef enum{ false = 0,true = 1 }bool;

typedef struct {        /* GPS/QZS/GAL broadcast ephemeris type */
	int sat;            /* satellite number */
	int iode, iodc;      /* IODE,IODC */
	int sva;            /* SV accuracy (URA index) */
	int svh;            /* SV health (0:ok) */
	int week;           /* GPS/QZS: gps week, GAL: galileo week */
	int code;           /* GPS/QZS: code on L2, GAL/CMP: data sources */
	int flag;           /* GPS/QZS: L2 P data flag, CMP: nav type */
	gtime_t toe, toc, ttr; /* Toe,Toc,T_trans */
						   /* SV orbit parameters */
	double A, e, i0, OMG0, omg, M0, deln, OMGd, idot,N;
	double crc, crs, cuc, cus, cic, cis;
	double toes;        /* Toe (s) in week */
	double tocs;
	double fit;         /* fit interval (h) */
	double f0, f1, f2;    /* SV clock parameters (af0,af1,af2) */
	double tgd[4];      /* group delay parameters */
						/* GPS/QZS:tgd[0]=TGD */
						/* GAL    :tgd[0]=BGD E5a/E1,tgd[1]=BGD E5b/E1 */
						/* CMP    :tgd[0]=BGD1,tgd[1]=BGD2 */
	double Adot, ndot;   /* Adot,ndot for CNAV */
	unsigned char fitinterval;
} eph_t;

typedef struct {        /* GPS/CMP broadcast ephemeris type */
	int sat;            /* satellite number */
	double tow;
	int iode1, iode2;      /* IODE,IODC */
	int iodc;
	int sva;            /* SV accuracy (URA index) */
	int svh;            /* SV health (0:ok) */
	int week;           /* GPS: gps week, CMP: cmp week */
	int zweek;          /* GPS: gps zweek, CMP: cmp zweek */
	int code;           /* GPS/QZS: code on L2, GAL/CMP: data sources */
	int flag;           /* GPS/QZS: L2 P data flag, CMP: nav type */
	gtime_t toe, toc, ttr; /* Toe,Toc,T_trans */
//	bool AS;
						   /* SV orbit parameters */
	double A, e, i0, OMG0, omg, M0, deln, OMGd, idot;
	double crc, crs, cuc, cus, cic, cis;
	double toes;        /* Toe (s) in week */
	double fit;         /* fit interval (h) */
	double f0, f1, f2;    /* SV clock parameters (af0,af1,af2) */
	double tgd[4];      /* group delay parameters */
						/* GPS/QZS:tgd[0]=TGD */
						/* GAL    :tgd[0]=BGD E5a/E1,tgd[1]=BGD E5b/E1 */
						/* CMP    :tgd[0]=BGD1,tgd[1]=BGD2 */
	double Adot, ndot;   /* Adot,ndot for CNAV */
} ephem_t;



typedef struct {        /* precise ephemeris type */
	gtime_t time;       /* time (GPST) */
	int index;          /* ephemeris index for multiple files */
	double pos[MAXSAT][4]; /* satellite position/clock (ecef) (m|s) */
	float  std[MAXSAT][4]; /* satellite position/clock std (m|s) */
	double vel[MAXSAT][4]; /* satellite velocity/clk-rate (m/s|s/s) */
	float  vst[MAXSAT][4]; /* satellite velocity/clk-rate std (m/s|s/s) */
	float  cov[MAXSAT][3]; /* satellite position covariance (m^2) */
	float  vco[MAXSAT][3]; /* satellite velocity covariance (m^2) */
} peph_t;

typedef struct {        /* precise clock type */
	gtime_t time;       /* time (GPST) */
	int index;          /* clock index for multiple files */
	double clk[MAXSAT][1]; /* satellite clock (s) */
	float  std[MAXSAT][1]; /* satellite clock std (s) */
} pclk_t;

typedef struct {        /* almanac type */
	int sat;            /* satellite number */
	int svh;            /* sv health (0:ok) */
	int svconf;         /* as and sv config */
	int week;           /* GPS/QZS: gps week, GAL: galileo week */
	gtime_t toa;        /* Toa */
						/* SV orbit parameters */
	double A, e, i0, OMG0, omg, M0, OMGd;
	double toas;        /* Toa (s) in week */
	double f0, f1;       /* SV clock parameters (af0,af1) */
} alm_t;

typedef struct {        /* TEC grid type */
	gtime_t time;       /* epoch time (GPST) */
	int ndata[3];       /* TEC grid data size {nlat,nlon,nhgt} */
	double rb;          /* earth radius (km) */
	double lats[3];     /* latitude start/interval (deg) */
	double lons[3];     /* longitude start/interval (deg) */
	double hgts[3];     /* heights start/interval (km) */
	double *data;       /* TEC grid data (tecu) */
	float *rms;         /* RMS values (tecu) */
} tec_t;

typedef struct {        /* stec data type */
	gtime_t time;       /* time (GPST) */
	unsigned char sat;  /* satellite number */
	unsigned char slip; /* slip flag */
	float iono;         /* L1 ionosphere delay (m) */
	float rate;         /* L1 ionosphere rate (m/s) */
	float rms;          /* rms value (m) */
} stecd_t;

typedef struct {        /* stec grid type */
	double pos[2];      /* latitude/longitude (deg) */
	int index[MAXSAT];  /* search index */
	int n, nmax;         /* number of data */
	stecd_t *data;      /* stec data */
} stec_t;

typedef struct {        /* earth rotation parameter data type */
	double mjd;         /* mjd (days) */
	double xp, yp;       /* pole offset (rad) */
	double xpr, ypr;     /* pole offset rate (rad/day) */
	double ut1_utc;     /* ut1-utc (s) */
	double lod;         /* length of day (s/day) */
} erpd_t;

typedef struct {        /* earth rotation parameter type */
	int n, nmax;         /* number and max number of data */
	erpd_t *data;       /* earth rotation parameter data */
} erp_t;

typedef struct {        /* antenna parameter type */
	int sat;            /* satellite number (0:receiver) */
	char type[MAXANT];  /* antenna type */
	char code[MAXANT];  /* serial number or satellite code */
	gtime_t ts, te;      /* valid time start and end */
	double off[NFREQ][3]; /* phase center offset e/n/u or x/y/z (m) */
	double var[NFREQ][19]; /* phase center variation (m) */
						   /* el=90,85,...,0 or nadir=0,1,2,3,... (deg) */
} pcv_t;

typedef struct {        /* DGPS/GNSS correction type */
	gtime_t t0;         /* correction time */
	double prc;         /* pseudorange correction (PRC) (m) */
	double rrc;         /* range rate correction (RRC) (m/s) */
	int iod;            /* issue of data (IOD) */
	double udre;        /* UDRE */
} dgps_t;

typedef struct {        /* SSR correction type */
	gtime_t t0[5];      /* epoch time (GPST) {eph,clk,hrclk,ura,bias} */
	double udi[5];      /* SSR update interval (s) */
	int iod[5];         /* iod ssr {eph,clk,hrclk,ura,bias} */
	int iode;           /* issue of data */
	int iodcrc;         /* issue of data crc for beidou/sbas */
	int ura;            /* URA indicator */
	int refd;           /* sat ref datum (0:ITRF,1:regional) */
	double deph[3];    /* delta orbit {radial,along,cross} (m) */
	double ddeph[3];    /* dot delta orbit {radial,along,cross} (m/s) */
	double dclk[3];    /* delta clock {c0,c1,c2} (m,m/s,m/s^2) */
	double hrclk;       /* high-rate clock corection (m) */
	float cbias[MAXCODE]; /* code biases (m) */
	unsigned char update; /* update flag (0:no update,1:update) */
} ssr_t;

typedef struct {        /* navigation data type */
	int n, nmax;         /* number of broadcast ephemeris */
	int ne, nemax;       /* number of precise ephemeris */
	int nc, ncmax;       /* number of precise clock */
	int na, namax;       /* number of almanac data */
	int nt, ntmax;       /* number of tec grid data */
	int nn, nnmax;       /* number of stec grid data */

	eph_t *eph;         /* GPS/CMP ephemeris information */
	//ephem_t *ephem;     /* GPS/CMP ephemeris add new information in new eph */
	peph_t *peph;       /* precise ephemeris */
	pclk_t *pclk;       /* precise clock */
	alm_t *alm;         /* almanac data */
	tec_t *tec;         /* tec grid data */
	stec_t *stec;       /* stec grid data */
	erp_t  erp;         /* earth rotation parameters */
	double utc_gps[4];  /* GPS delta-UTC parameters {A0,A1,T,W} */
	double utc_cmp[4];  /* BeiDou UTC parameters */
	double ion_gps[8];  /* GPS iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
	double ion_cmp[8];  /* BeiDou iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
	int leaps;          /* leap seconds (s) */
	double lam[MAXSAT][NFREQ]; /* carrier wave lengths (m) */
	double cbias[MAXSAT][3];   /* code bias (0:p1-p2,1:p1-c1,2:p2-c2) (m) */
	double wlbias[MAXSAT];     /* wide-lane bias (cycle) */
	pcv_t pcvs[MAXSAT]; /* satellite antenna pcv */
	dgps_t dgps[MAXSAT]; /* DGPS corrections */
	ssr_t ssr[MAXSAT];  /* SSR corrections */
} nav_t;

typedef struct {        /* station parameter type */
	char name[MAXANT]; /* marker name */
	char marker[MAXANT]; /* marker number */
	char antdes[MAXANT]; /* antenna descriptor */
	char antsno[MAXANT]; /* antenna serial number */
	char rectype[MAXANT]; /* receiver type descriptor */
	char recver[MAXANT]; /* receiver firmware version */
	char recsno[MAXANT]; /* receiver serial number */
	int antsetup;       /* antenna setup id */
	int itrf;           /* ITRF realization year */
	int deltype;        /* antenna delta type (0:enu,1:xyz) */
	double pos[3];      /* station position (ecef) (m) */
	double del[3];      /* antenna position delta (e/n/u or x/y/z) (m) */
	double hgt;         /* antenna height (m) */
} sta_t;


typedef struct {        /* RTCM control struct type */
	int staid;          /* station id */
	int stah;           /* station health */
	int seqno;          /* sequence number for rtcm 2 or iods msm */
	int outtype;        /* output message type */
	gtime_t time;       /* message time */
	gtime_t time_s;     /* message start time */
	obs_t obs;          /* observation data (uncorrected) */
	nav_t nav;          /* satellite ephemerides */
	char msg[128];      /* special message */
	char msgtype[256];  /* last message type */
	char msmtype[6][128]; /* msm signal types */
	int obsflag;        /* obs data complete flag (1:ok,0:not complete) */
	int ephsat;         /* update satellite of ephemeris */
	double cp[MAXSAT][NFREQ + NEXOBS]; /* carrier-phase measurement */
	unsigned char lock[MAXSAT][NFREQ + NEXOBS]; /* lock time */
	unsigned char loss[MAXSAT][NFREQ + NEXOBS]; /* loss of lock count */
	gtime_t lltime[MAXSAT][NFREQ + NEXOBS]; /* last lock time */
	int nbyte;          /* number of bytes in message buffer */
	int nbit;           /* number of bits in word buffer */
	int len;            /* message length (bytes) */
	unsigned char buff[1200]; /* message buffer */
	unsigned char gpsbdsbuff[2][1200];
	unsigned int nmsg3[300]; /* message count of RTCM 3 (1-299:1001-1299,0:ohter) */
	char opt[256];      /* RTCM dependent options */
} rtcm_t;

typedef struct {        /* receiver raw data control type */
	gtime_t time;       /* message time */
	gtime_t tobs;       /* observation data time */
	obs_t obs;          /* observation data */
	obs_t obuf;         /* observation data buffer */
	nav_t nav;          /* satellite ephemerides */
	sta_t sta;          /* station parameters */
	int ephsat;         /* sat number of update ephemeris (0:no satellite) */
	char msgtype[256];  /* last message type */
	unsigned char subfrm[MAXSAT][380];  /* subframe buffer */
	double lockt[MAXSAT][NFREQ + NEXOBS]; /* lock time (s) */
	double icpp[MAXSAT], off[MAXSAT], icpc; /* carrier params for ss2 */
	double prCA[MAXSAT], dpCA[MAXSAT]; /* L1/CA pseudrange/doppler for javad */
	unsigned char halfc[MAXSAT][NFREQ + NEXOBS]; /* half-cycle add flag */
	char freqn[MAXOBS]; /* frequency number for javad */
	int nbyte;          /* number of bytes in message buffer */
	int len;            /* message length (bytes) */
	int iod;            /* issue of data */
	int tod;            /* time of day (ms) */
	int tbase;          /* time base (0:gpst,1:utc(usno),2:glonass,3:utc(su) */
	int flag;           /* general purpose flag */
	int outtype;        /* output message type */
	unsigned char buff[MAXRAWLEN]; /* message buffer */
	char opt[256];      /* receiver dependent options */
	double receive_time;/* RT17: Reiceve time of week for week rollover detection */
	unsigned int plen;  /* RT17: Total size of packet to be read */
	unsigned int pbyte; /* RT17: How many packet bytes have been read so far */
	unsigned int page;  /* RT17: Last page number */
	unsigned int reply; /* RT17: Current reply number */
	int week;           /* RT17: week number */
	unsigned char pbuff[255 + 4 + 2]; /* RT17: Packet buffer */

	unsigned int bdsmask_low;
	unsigned int bdsmask_high;
	unsigned int gpsmask;

	AssistData assistData;
	DgnssDat   dgnssDat;

	rtcm_t  gps_dgnssdata;
	rtcm_t  bds_dgnssdata;
	rtcm_t  refstation;
} raw_t;


//TureCORS Receiver Message_Range
typedef struct _tagTureCORS_Messaage_Range {
	long int obs;
	unsigned int PRN;//GPS:1-32 SBAS:120-138 GLONASS:38-61 BD2 161-197
	unsigned int glofreq;
	double psr;
	float psr_std;
	double adr;//Carrier phase in cycles,accumulated Doppler range
	float adr_std;
	float dopp;
	float CN0;
	float locktime;
} TureCORS_Messaage_Range;


// 初始化
void AssistData_Init(raw_t* rawData);

// 输入接收机数据
void AssistData_Input(raw_t* rawData, int recid, char* buff, int len);

void decode_type43(TureCORS_Messaage_Range* messageRange, char* buff, int len);

// 15秒调用一次
void AssistData_Manage(AssistData* assistData, const UTCTM* putctm);

//void DgnssData_Manage(DgnssDat* dgnssdata);

static unsigned short U2(unsigned char *p);
static unsigned int   U4(unsigned char *p);
static int            I4(unsigned char *p);
static float          R4(unsigned char *p);
static double         R8(unsigned char *p);

unsigned char MxtChkSum_Cal(const char* str, int len);
int MxtChkSum_Chk(const char* str);
unsigned char CalCheckSum(const char* str);

extern gtime_t epoch2time(const double *ep);
extern gtime_t timeadd(gtime_t t, double sec);
extern gtime_t gpst2time(int week, double sec);
extern gtime_t timeget(void);

extern double time2gpst(gtime_t t, int *week);
extern double timediff(gtime_t t1, gtime_t t2);
extern gtime_t utc2gpst(gtime_t t);
extern int adjgpsweek(int week);
extern void time2epoch(gtime_t t, double *ep);
extern void time2str(gtime_t t, char *s, int n);
extern char *time_str(gtime_t t, int n);
extern gtime_t bdt2gpst(gtime_t t);
extern gtime_t bdt2time(int week, double sec);
static int uraindex(double value);
static int decode_gpsephemerisb(raw_t *raw);

void setbitu(unsigned char *buff, int pos, int len, unsigned int data);
unsigned int getbitu(const unsigned char *buff, int pos, int len);
extern int getbits(const unsigned char *buff, int pos, int len);

static int sync_oem4(unsigned char *buff, unsigned char data);
extern void free_raw(raw_t *raw);
//int AgnssData_MxtInput(AssistData* assistData, unsigned char data);
//void AgnssData_MxtAideph(AssistData* assistData, char* str);
void AssistData_Init(raw_t* rawData);

void decode_type43(TureCORS_Messaage_Range* messageRange, char* buff, int len);
static int decode_trackstat(unsigned int stat, int *sys, int *code, int *track,
	int *plock, int *clock, int *parity, int *halfc);
static int decode_rangeb(raw_t *rawData);
static int decode_rangecmpb(raw_t *rawData);

extern int satsys(int sat, int *prn);
extern unsigned int crc32(const unsigned char *buff, int len);
static int checkpri(const char *opt, int sys, int code, int freq);
static int obsindex(obs_t *obs, gtime_t time, int sat);
extern int satno(int sys, int prn);

static int decode_subfrm1(const unsigned char *buff, eph_t *eph);
static int decode_subfrm2(const unsigned char *buff, eph_t *eph);
static int decode_subfrm3(const unsigned char *buff, eph_t *eph);
static void decode_almanac(const unsigned char *buff, int sat, alm_t *alm);
static void decode_gps_subfrm4(const unsigned char *buff, alm_t *alm,
	double *ion, double *utc, int *leaps);
static void decode_gps_subfrm5(const unsigned char *buff, alm_t *alm);
static int decode_subfrm4(const unsigned char *buff, alm_t *alm, double *ion,
	double *utc, int *leaps);
static int decode_subfrm5(const unsigned char *buff, alm_t *alm, double *ion,
	double *utc, int *leaps);
extern int decode_frame(const unsigned char *buff, eph_t *eph, alm_t *alm,
	double *ion, double *utc, int *leaps);

static int decode_rawephemb(raw_t *raw);
static int decode_bdsephemerisb(raw_t *raw);
static int decode_oem4(raw_t *rawData);
extern int input_oem4(raw_t *raw, unsigned char data);

double ScaleDoubleDate(double InputData, U8 BitNum);
double ScaleDoubleDateDivPi(double InputData, unsigned int BitNum);
//oid OutputAidEph(raw_t* pEphIn, U8 navSys, U8 datatype);

void AssistData_Input(raw_t* rawData, int recid, char* buff, int len);
void DgnssData_Init(rtcm_t* dgnssData);


// 返回辅助数据
//int AssistData_Ouput(AssistData* assistData, char* buff, int gps, int bds, float pos[2], float elev);


/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif //ASSISTDATA_H_
