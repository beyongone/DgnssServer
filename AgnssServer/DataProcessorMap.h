/*
 * DataProcessorMap.h
 *
 *  Created on: 2017年4月20日
 *      Author: chuanjiang.zh
 */

#ifndef DATAPROCESSORMAP_H_
#define DATAPROCESSORMAP_H_

#include "DataProcessor.h"
#include "SharedPtr.h"
#include "TMap.h"

typedef std::shared_ptr< DataProcessor >	DataProcessorPtr;


class DataProcessorMap : public comn::Map<std::string, DataProcessorPtr >
{
public:
	DataProcessorMap();
	virtual ~DataProcessorMap();


};

#endif /* DATAPROCESSORMAP_H_ */
