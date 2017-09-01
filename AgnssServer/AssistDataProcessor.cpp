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

	return true;
}
