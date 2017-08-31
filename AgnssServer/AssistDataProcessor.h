/*
 * AssistDataProcessor.h
 *
 *  Created on: 2017年4月20日
 *      Author: chuanjiang.zh
 */

#ifndef ASSISTDATAPROCESSOR_H_
#define ASSISTDATAPROCESSOR_H_

#include "DataProcessor.h"
#include "AssistData.h"
#include "TCriticalSection.h"


class AssistDataProcessor: public DataProcessor
{
public:
	AssistDataProcessor();
	virtual ~AssistDataProcessor();

	virtual void input(AgnssFrameHeader& header, const void* data, size_t length);

	//virtual bool getOutput(AgnssParam& param, AgnssAssistData& data);
	virtual bool getOutput(AgnssParam& param, DgnssDatAll& data);

	//virtual bool getOutputDGNSS(DgnssDatAll& data);

protected:
	raw_t	m_data;
	comn::CriticalSection	m_cs;

};

#endif /* ASSISTDATAPROCESSOR_H_ */
