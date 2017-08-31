/*
 * AssistDataProcessor.cpp
 *
 *  Created on: 2017年4月20日
 *      Author: chuanjiang.zh
 */

#include<iostream>
#include "AssistDataProcessor.h"
#include "DateTime.h"

using namespace std;
AssistDataProcessor::AssistDataProcessor()
{
	AssistData_Init(&m_data);
}

AssistDataProcessor::~AssistDataProcessor()
{
}

void AssistDataProcessor::input(AgnssFrameHeader& header, const void* data, size_t length)
{
	comn::AutoCritSec lock(m_cs);
	AssistData_Input(&m_data, 0, (char*)data, length);
}

/*
bool AssistDataProcessor::getOutputDGNSS(DgnssDatAll& data)
{
	comn::AutoCritSec lock(m_cs);

	comn::DateTime dt = comn::DateTime::now();

	if (0 == m_data.dgnssDat.dgnss_flag) return false;

	//DgnssData_Manage(&m_data.dgnssDat);

	data.dgnss_gpslen = m_data.dgnssDat.dgnss_gpslen;
	memcpy(data.dgnss_gps, m_data.dgnssDat.dgnss_gps, m_data.dgnssDat.dgnss_gpslen);

	data.dgnss_bdslen = m_data.dgnssDat.dgnss_bdslen;
	memcpy(data.dgnss_bds, m_data.dgnssDat.dgnss_bds, m_data.dgnssDat.dgnss_bdslen);

	return true;
	
}
*/

bool AssistDataProcessor::getOutput(AgnssParam& param, DgnssDatAll& data)
{
	comn::AutoCritSec lock(m_cs);

	comn::DateTime dt = comn::DateTime::now();

	UTCTM utctm;
	utctm.year = dt.getYear();
	utctm.month = dt.getMonth();
	utctm.day = dt.getDay();
	utctm.hour = dt.getHour();
	utctm.minute = dt.getMinute();
	utctm.sec = dt.getSecond();

	if(m_data.dgnssDat.dgnss_flag == 0) return false;

   // (3600 < 15*(m_data.manage_tick - m_data.manage_tick_data)) return false;
   //if(m_data.assistData.ADdec.aidupdate == 0 )return false;
   // if (0 == m_data.assistData.aideph_gpsbds_len) return false;
   //AssistData_Manage(&m_data.assistData, &utctm);

	param.basetime = 3;
    param.flag = 1;
    param.year = utctm.year;
    param.month = utctm.month;
    param.day = utctm.day;
    param.hour = utctm.hour;
    param.minute = utctm.minute;
    param.sec = utctm.sec;
    param.minsec = 0;
    param.secacc = 60;
    param.lat = 0;
    param.lon = 0;
    param.alt = 0;
    param.posacc = 0;

	param.bdsmask_low = 0;
	param.bdsmask_high = 0;
	param.gpsmask = 0;

	param.bdsmask_low = 0;
	param.bdsmask_high = 0;
	param.gpsmask = 0;

	data.dgnss_bdslen = m_data.dgnssDat.dgnss_bdslen;
	data.dgnss_gpslen = m_data.dgnssDat.dgnss_gpslen;
	data.dgnss_gpsbdslen = 0;

	memcpy(data.dgnss_gpsbds, m_data.dgnssDat.dgnss_refstat, m_data.dgnssDat.ref_stationlen);
	data.dgnss_gpsbdslen += m_data.dgnssDat.ref_stationlen;

	memcpy(data.dgnss_gps, m_data.dgnssDat.dgnss_gps, m_data.dgnssDat.dgnss_gpslen);
	memcpy(data.dgnss_gpsbds + data.dgnss_gpsbdslen, m_data.dgnssDat.dgnss_gps, m_data.dgnssDat.dgnss_gpslen);
	data.dgnss_gpsbdslen += m_data.dgnssDat.dgnss_gpslen;

	memcpy(data.dgnss_bds, m_data.dgnssDat.dgnss_bds, m_data.dgnssDat.dgnss_bdslen);
	memcpy(data.dgnss_gpsbds + data.dgnss_gpsbdslen, m_data.dgnssDat.dgnss_bds, m_data.dgnssDat.dgnss_bdslen);
	data.dgnss_gpsbdslen += m_data.dgnssDat.dgnss_bdslen;

	if (1 == m_data.dgnssDat.dgnss_flag)
	{
		data.dgnss_flag = 1;
		m_data.dgnssDat.dgnss_flag = 0;
	}

#if 0
	param.bdsmask_low = m_data.assistData.bdsmask_low;
	param.bdsmask_high = m_data.assistData.bdsmask_high;
	param.gpsmask = m_data.assistData.gpsmask;

	data.aideph_gps_len = m_data.assistData.aideph_gps_len;
	memcpy(data.aideph_gps, m_data.assistData.aideph_gps, m_data.assistData.aideph_gps_len);

	data.aideph_bds_len = m_data.assistData.aideph_bds_len;
	memcpy(data.aideph_bds, m_data.assistData.aideph_bds, m_data.assistData.aideph_bds_len);

	data.aideph_gpsbds_len = m_data.assistData.aideph_gpsbds_len;
	memcpy(data.aideph_gpsbds, m_data.assistData.aideph_gpsbds, m_data.assistData.aideph_gpsbds_len);

	data.all_aideph_gpsbds_len = m_data.assistData.all_aideph_gpsbds_len;
	memcpy(data.all_aideph_gpsbds, m_data.assistData.all_aideph_gpsbds, m_data.assistData.all_aideph_gpsbds_len);

	if (1 == m_data.assistData.ADdec.aidupdate)
	{
		data.aideph_flag = 1;
		AssistData_Init(&m_data);
	}
#endif

	return true;
}
