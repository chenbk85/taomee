/**
 * =====================================================================================
 *      @file  StorageUnit.h
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  Nov 30, 2011
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  jacke (), jacke@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef STORAGEUNIT_H_
#define STORAGEUNIT_H_

#include <vector>
#include <string>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <sys/time.h>

#define STORAGE_VERSION 1

/*
 *KVdb::StorageUnit
 * a essential StorageUint hook the leveldb value;
 */
namespace KVdb {

class StorageUnit {
public:
	StorageUnit(uint8_t ver, uint32_t exptime, std::string value) :
		version(ver), m_uExptime(exptime), Data(value) {
	}
	;
	StorageUnit(std::string value) {
		unpack(value);
	}
	;
	std::string pack() {
		uint32_t size = Data.length() + sizeof(m_uExptime) + sizeof(version);
		std::vector<char> tmp(size);
		tmp[0] = version;
		memcpy(&tmp[sizeof(version)], &m_uExptime, 4);
		memcpy(&tmp[sizeof(m_uExptime) + sizeof(version)], Data.c_str(), Data.length());
		return std::string(&tmp[0], size);
	}

	void unpack(std::string& value) {
		if (value.length() >= sizeof(m_uExptime) + sizeof(version)) {
			version = value[0]; 
			memcpy((void*)&m_uExptime, value.c_str() + 1, sizeof(m_uExptime));
			Data = value.substr(sizeof(m_uExptime) + sizeof(version), value.length() - sizeof(m_uExptime) + sizeof(version));
		}
	}

	bool time_available(){
		struct timeval now;
		gettimeofday(&now, 0);
		/*m_uExptime:0 is means infinit saved*/
		if(m_uExptime && (uint32_t)now.tv_sec >= m_uExptime){
			return false;
		}
		return true;
	}

	uint32_t get_exptime() {
		return m_uExptime;
	}

	std::string & data(){
		return Data;
	}

private:
	uint8_t version;
	uint32_t m_uExptime;
	std::string Data;
};

}

#endif /* STORAGEUNIT_H_ */
