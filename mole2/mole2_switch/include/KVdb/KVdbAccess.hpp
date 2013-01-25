/**
 * =====================================================================================
 *      @file  KVdbAccess.h
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  Nov 29, 2011
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  jacke (), jacke@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#include <string>
#include <stdint.h>
#include <map>
#include "Protocal.hpp"
#include "Column.hpp"
#include "StorageUnit.hpp"
#include "sigslot.h"

#ifndef KVDBACCESS_H_
#define KVDBACCESS_H_

/*
 *KVbase::KVdbAccess
 */
namespace KVdb {

class KVdbAccess {
public:
	KVdbAccess(ColumnManager* manager);

	template<class KeyType, class ValueType, class ColumnType>
	void get(KeyType key, ColumnType column_name, ValueType& value);

	template<class KeyType, class ColumnType>
	void get(KeyType key, ColumnType column_name, RawKVdbBuffer& value);

	template<class KeyType, class ValueType, class ColumnType>
	void set(KeyType key, ColumnType column_name, ValueType value, uint32_t exptime);

	template<class KeyType, class ColumnType>
	void set(KeyType key, ColumnType column_name, RawKVdbBuffer& value, uint32_t exptime);

	template<class KeyType, class ValueType, class ColumnType>
	void append(KeyType key, ColumnType column_name, ValueType value, uint32_t exptime);
	
	template<class KeyType, class ColumnType>
	void append(KeyType key, ColumnType column_name, RawKVdbBuffer& value, uint32_t exptime);
	
	template<class KeyType, class ValueType, class ColumnType>
	void uniq_append(KeyType key, ColumnType column_name, ValueType value, uint32_t exptime);
	
	template<class KeyType, class ValueType, class ColumnType>
	void erase(KeyType key, ColumnType column_name, ValueType value);

	template<class ColumnType>
	void get_random(ColumnType column_name, RawKVdbBuffer& value);

	template<class ColumnType, class ValueType>
	void get_random(ColumnType column_name, ValueType& value);

	template<class KeyType, class ColumnType>
	void del(KeyType key, ColumnType column_name);

	uint32_t count(std::string column_name);

	template<class KeyType, class ValueType, class ColumnType>
	void get_all(ColumnType column_name, std::multimap<KeyType,ValueType>& value_map);

	template<class KeyType, class ValueType, class ColumnType>
    void get_all_count(ColumnType column_name, std::map<KeyType,uint32_t>& value_map);

private:
	Column* get_column(std::string column_name);
	Column* get_column(const char* column_name);
	
	ColumnManager* m_columnManager;
};

template<class KeyType, class ValueType, class ColumnType>
void KVdbAccess::get(KeyType key, ColumnType column_name, ValueType& value) {
	Column* pColumn = get_column(column_name);
	pColumn->get_KV(key, value);
}
	
template<class KeyType, class ValueType, class ColumnType>
void KVdbAccess::set(KeyType key, ColumnType column_name, ValueType value, uint32_t exptime) {
    Column* pColumn = get_column(column_name);
    pColumn->insert_KV(key, value, exptime);
}

template<class KeyType, class ColumnType>
void KVdbAccess::get(KeyType key, ColumnType column_name, RawKVdbBuffer& value) {
	Column* pColumn = get_column(column_name);
	pColumn->get_KV(key, value);
}
	
template<class KeyType, class ColumnType>
void KVdbAccess::set(KeyType key, ColumnType column_name, RawKVdbBuffer& value, uint32_t exptime) {
    Column* pColumn = get_column(column_name);
    pColumn->insert_KV(key, value, exptime);
}
template<class KeyType, class ValueType, class ColumnType>
void KVdbAccess::get_all(ColumnType column_name, std::multimap<KeyType,ValueType>& value_map) {
	Column* pColumn = get_column(column_name);
	pColumn->get_all(value_map);
}

template<class KeyType, class ValueType, class ColumnType>
void KVdbAccess::get_all_count(ColumnType column_name, std::map<KeyType,uint32_t>& value_map) {
	Column* pColumn = get_column(column_name);
	pColumn->get_all_count<KeyType, ValueType>(value_map);
}

template<class KeyType, class ValueType, class ColumnType>
void KVdbAccess::append(KeyType key, ColumnType column_name, ValueType value, uint32_t exptime) {
	Column* pColumn = get_column(column_name);
	pColumn->append_KV(key, value, exptime);
}

template<class KeyType, class ColumnType>
void KVdbAccess::append(KeyType key, ColumnType column_name, RawKVdbBuffer& value, uint32_t exptime) {
	Column* pColumn = get_column(column_name);
	pColumn->append_KV(key, value, exptime);
}

template<class KeyType, class ValueType, class ColumnType>
void KVdbAccess::uniq_append(KeyType key, ColumnType column_name, ValueType value, uint32_t exptime) {
	Column* pColumn = get_column(column_name);
	pColumn->uniq_append_KV(key, value, exptime);
}	

template<class KeyType, class ValueType, class ColumnType>
void KVdbAccess::erase(KeyType key, ColumnType column_name, ValueType value) {
	Column* pColumn = get_column(column_name);
	pColumn->erase_KV(key, value);
}

template<class KeyType, class ColumnType>
void KVdbAccess::del(KeyType key, ColumnType column_name) {
	Column* pColumn = get_column(column_name);
	pColumn->delete_KV(key);
}

template<class ColumnType>
void KVdbAccess::get_random(ColumnType column_name, RawKVdbBuffer& value) {
	Column* pColumn = get_column(column_name);
	pColumn->random_get_KV(value);
}

template<class ColumnType, class ValueType>
void KVdbAccess::get_random(ColumnType column_name, ValueType& value) {
	Column* pColumn = get_column(column_name);
	pColumn->random_get_KV(value);
}

class KVdbRawAccess: public KVdbAccess {
public:
	KVdbRawAccess(ColumnManager* manager);

};

class KVdbNetAccess: protected KVdbAccess {
public:
	typedef sigslot::signal2<uint32_t , std::string> SignalType;
	typedef std::map<std::string, sigslot::signal2<uint32_t , std::string> > SignalMapType;
	KVdbNetAccess(ColumnManager* manager);
	void op_message(NetKVdbBuffer& buffer);
	SignalType& get_signal(std::string column, uint32_t op);
	bool exist_signal(std::string column, uint32_t op);
	/*signal*/
	SignalType sig_KV_set;
private:
	SignalMapType m_get_signal_map;
	SignalMapType m_set_signal_map;
	SignalMapType m_append_signal_map;
	SignalMapType m_count_signal_map;
	SignalMapType m_default_signal_map;
};


}

#endif /* KVDBACCESS_H__ */
