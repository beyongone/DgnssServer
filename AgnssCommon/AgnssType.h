/*
 * AgnssType.h
 *
 *  Created on: 2017年4月19日
 *      Author: chuanjiang.zh
 */

#ifndef AGNSSTYPE_H_
#define AGNSSTYPE_H_

#include "BasicType.h"
#include <string>



/// 帧类型
enum AgnssFrameTag
{
	AGNSS_TAG_NONE,
	AGNSS_TAG_STATUS,	/// 状态
	AGNSS_TAG_DATA,		/// 数据
};

/// 帧子类型
enum AgnssSubType
{
	AGNSS_SUBTYPE_NONE = 0
};

/// 状态
enum AgnssStatus
{
	AGNSS_STATUS_OFFLINE = 0,	/// 下线
	AGNSS_STATUS_ONLINE			/// 上线
};


/// 设备类型
enum AgnssDeviceType
{
	AGNSS_DEVICE_NONE = 0,
    AGNSS_DEVICE_ASSIST			/// 其他种类的设备
};

/// 帧头
struct AgnssFrameHeader
{
	int		tag;		/// 主类型 @see AgnssFrameTag
	int		subtype;	/// 子类型
	int		deviceType;	/// 设备类型
	char 	device[32];	/// 设备ID
	time_t  time;		/// 时间戳
	int		length;		/// 数据长度
};


struct AgnssParam
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
};


struct AgnssAssistData
{
	int aideph_flag;
    int aideph_gps_len;
    char aideph_gps[4000];

    int aideph_bds_len;
    char aideph_bds[4000];

    int aideph_gpsbds_len;
    char aideph_gpsbds[8000];

	int all_aideph_gpsbds_len;
	char all_aideph_gpsbds[9000];
};

struct DgnssDatAll {

	int dgnss_flag;

	int dgnss_gpslen;
	char dgnss_gps[1200];

	int dgnss_bdslen;
	char dgnss_bds[1200];

	int dgnss_gpsbdslen;
	unsigned char dgnss_gpsbds[2400];

	int dgnss_gpsbdslenout;
	char dgnss_gpsbdsout[2400];
} ;



#endif /* AGNSSTYPE_H_ */
