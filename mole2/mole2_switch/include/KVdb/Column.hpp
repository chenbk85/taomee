/**
 * =====================================================================================
 *      @file  Column.hpp
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

#ifndef COLUMN_H_
#define COLUMN_H_

#include <assert.h>
#include "leveldb/db.h"
#include "leveldb/comparator.h"
#include <string>
#include <map>
#include <stdint.h>
#include "StorageUnit.hpp"
#include "Protocal.hpp"
#include <iostream>
#include <string>
#include <sstream>

/*
 *KVbase::Column
 */
namespace KVdb {

/*every column have one db*/
class Column {
public:
	Column(std::string name);
	
	void create_db(std::string base_dir);
	
	bool create();
	
	std::string name();
	
	template<class KeyType, class ValueType>
	void insert_KV(KeyType& key, ValueType& value, uint32_t exptime);

	template<class KeyType>
	void insert_KV(KeyType& key, RawKVdbBuffer& value, uint32_t exptime);
		
	template<class KeyType, class ValueType>
	void append_KV(KeyType& key, ValueType& value, uint32_t exptime);

	template<class KeyType>
	void append_KV(KeyType& key, RawKVdbBuffer& value, uint32_t exptime);

	template<class KeyType, class ValueType>
	void uniq_append_KV(KeyType& key, ValueType& value, uint32_t exptime);

	template<class KeyType, class ValueType>
	void erase_KV(KeyType& key, ValueType& value);

	template<class KeyType, class ValueType>
	void get_KV(KeyType& key, ValueType& value);
	
	template<class KeyType>
	void get_KV(KeyType& key, RawKVdbBuffer& value);	

	void random_get_KV(RawKVdbBuffer& value);

	template<class ValueType>
	void random_get_KV(ValueType& value);

	template<class KeyType>
	void delete_KV(KeyType& key);

	template<typename KeyType, typename ValueType>
	void get_all(std::multimap<KeyType, ValueType>& value_map);

	template<typename KeyType, typename ValueType>
	void get_all_count(std::map<KeyType, uint32_t>& value_map);

	uint32_t count_KV();
	leveldb::DB* m_db;
private:
	std::string m_name;
	leveldb::Status m_status;
	bool m_create;
	leveldb::Iterator* m_random_itr;
	RawKVdbBuffer m_random_buffer;
};


class SortColumn {
public:
	SortColumn(std::string name);
	
	void create_db(std::string base_dir);
	
	bool create();
	
	std::string name();

	template<typename SortType, typename ValueType>
	void insert_sort(SortType sort, ValueType value);

	template<typename SortType, typename ValueType>
	void delete_sort(SortType sort, ValueType value);

	template<typename SortType, typename ValueType>
	void get_range(std::multimap<SortType, ValueType>& result);

private:
	leveldb::DB* m_db;
	std::string m_name;
	leveldb::Status m_status;
	bool m_create;
};

template<typename SortType, typename ValueType>
class SortComparator : public leveldb::Comparator {
public:
    // Three-way comparison function:
    //   if a < b: negative result
    //   if a > b: positive result
    //   else: zero result
	int Compare(const leveldb::Slice& a, const leveldb::Slice& b) const {
		RawKVdbBuffer buffer_a;
		buffer_a << a.ToString();
		SortType a_sort;
		buffer_a >> a_sort;
		RawKVdbBuffer buffer_b;
		buffer_b << b.ToString();
		SortType b_sort;
		buffer_b >> b_sort;
		if(a_sort<b_sort) return -1;
		if(a_sort>b_sort) return +1;
      	return 0;
    }

    // Ignore the following methods for now:
    const char* Name() const { return "SortComparator"; }
    void FindShortestSeparator(std::string*, const leveldb::Slice&) const { }
    void FindShortSuccessor(std::string*) const { }
};

typedef std::map<std::string, Column*> ColumnMapType;
/*Column Manager hold all column for search */
class ColumnManager {
public:
	ColumnManager(std::string basedir);
	~ColumnManager();
	bool exist(std::string name);
	Column* get_column(std::string name);
	Column* get_column(const char* char_name);
	void check_column_create();
private:
	ColumnMapType m_map_column;
	std::string m_basedir;
};

template<class KeyType, class ValueType>
void Column::insert_KV(KeyType& key, ValueType& value, uint32_t exptime) {
	if (m_create == true) {
		RawKVdbBuffer  buffer_key;
		std::string key_str;
		RawKVdbBuffer  buffer_value;
		std::string value_str;
		buffer_key << key;
		buffer_key >> key_str;
		buffer_value << value;
		buffer_value >> value_str;
		//KDEBUG_LOG(0,"INSERT KV:%s %s, %u",K.ToString().c_str(),value.c_str(), exptime);
        StorageUnit uint = StorageUnit(STORAGE_VERSION, exptime, value_str);
        m_status = m_db->Put(leveldb::WriteOptions(), key_str , uint.pack());
        if(!m_status.ok()){
//            KDEBUG_LOG(0,"INSERT KV:%s %s ERROR",key_str.c_str(),value_str.c_str());
        }
    }
};

template <class KeyType>
void Column::insert_KV(KeyType& key, RawKVdbBuffer& value, uint32_t exptime) {
	if (m_create == true) {
		RawKVdbBuffer  buffer_key;
		std::string key_str;
		std::string value_str;
		buffer_key << key;
		buffer_key >> key_str;
		value >> value_str;
//      KDEBUG_LOG(0,"INSERT KV:%s %s, %u",K.ToString().c_str(),value.c_str(), exptime);
        StorageUnit uint = StorageUnit(STORAGE_VERSION, exptime, value_str);
        m_status = m_db->Put(leveldb::WriteOptions(), key_str , uint.pack());
        if(!m_status.ok()){
//            KDEBUG_LOG(0,"INSERT KV:%s %s ERROR",key_str.c_str(),value_str.c_str());
        }
    }
};

template<class KeyType, class ValueType>
void Column::get_KV(KeyType& key, ValueType& value) {
	if (m_create == true) {
		RawKVdbBuffer  buffer_key;
		std::string key_str;
		RawKVdbBuffer  buffer_value;
		std::string value_str;
		buffer_key << key;
		buffer_key >> key_str;
        std::string uint_str;
//      KDEBUG_LOG(0,"GET K:%s",K.ToString().c_str());
        m_status = m_db->Get(leveldb::ReadOptions(), key_str, &uint_str);
//      KDEBUG_LOG(0,"GET V:%s",uint_str.c_str()+5);
        if (m_status.IsNotFound()) {
//          KDEBUG_LOG(0,"CAN'T FIND KV:%u ",id);
        }
        StorageUnit uint(uint_str);
        /*for temp userage*/
        if(uint.time_available()){
            value_str = uint.data();
			buffer_value << value_str;
			buffer_value >> value;
        }
    }
}

template<class KeyType>
void Column::get_KV(KeyType& key, RawKVdbBuffer& value) {
    if (m_create == true) {
        RawKVdbBuffer  buffer_key;
        std::string key_str;
        std::string value_str;
        buffer_key << key;
        buffer_key >> key_str;
        std::string uint_str;
//      KDEBUG_LOG(0,"GET K:%s",K.ToString().c_str());
        m_status = m_db->Get(leveldb::ReadOptions(), key_str, &uint_str);
//      KDEBUG_LOG(0,"GET V:%s",uint_str.c_str()+5);
        if (m_status.IsNotFound()) {
//          KDEBUG_LOG(0,"CAN'T FIND KV:%u ",id);
        }
        StorageUnit uint(uint_str);
        /*for temp userage*/
        if(uint.time_available()){
            value_str = uint.data();
            value << value_str;
        }
    }
}

template<class KeyType, class ValueType>
void Column::append_KV(KeyType& key, ValueType& value, uint32_t exptime) {
    if(m_create == true) {
        RawKVdbBuffer old_value;
        get_KV(key,old_value);
		old_value << value;
        insert_KV(key, old_value, exptime);
    }
}

template<class KeyType>
void Column::append_KV(KeyType& key, RawKVdbBuffer& value, uint32_t exptime) {
	if(m_create == true) {
        RawKVdbBuffer old_value;
        get_KV(key,old_value);
		std::string old_str;
		value >> old_str;
        old_value << old_str;
        insert_KV(key, old_value, exptime);
    }
}

template<class KeyType, class ValueType>
void Column::uniq_append_KV(KeyType& key, ValueType& value, uint32_t exptime) {
	if(m_create == true) {
        RawKVdbBuffer  buffer_key;
        std::string key_str;
        std::string value_str;
        buffer_key << key;
        buffer_key >> key_str;
        std::string uint_str;
//      KDEBUG_LOG(0,"GET K:%s",K.ToString().c_str());
        m_status = m_db->Get(leveldb::ReadOptions(), key_str, &uint_str);
//      KDEBUG_LOG(0,"GET V:%s",uint_str.c_str()+5);
        if (m_status.IsNotFound()) {
//          KDEBUG_LOG(0,"CAN'T FIND KV:%u ",id);
        }
        StorageUnit uint(uint_str);
        /*for temp userage*/
        if(uint.time_available()){
            value_str = uint.data();
            RawKVdbBuffer buffer_value;
            RawKVdbBuffer buffer_set;
            buffer_value << value_str;
			uint32_t uniq = true;
            while (buffer_value.get_remain_size() >= sizeof(ValueType) ) {
                ValueType value_tmp;
                buffer_value >> value_tmp;
                if( (value == value_tmp)) {
                	uniq  = false;
				}
				buffer_set << value_tmp;
            }
			if (uniq) {
				buffer_set << value;
			}
            insert_KV(key,buffer_set, uint.get_exptime());
        }
    }   
}

template<class KeyType, class ValueType>
void Column::erase_KV(KeyType& key, ValueType& value) {
	if(m_create == true) {
		RawKVdbBuffer  buffer_key;
        std::string key_str;
        std::string value_str;
        buffer_key << key;
        buffer_key >> key_str;
        std::string uint_str;
//      KDEBUG_LOG(0,"GET K:%s",K.ToString().c_str());
        m_status = m_db->Get(leveldb::ReadOptions(), key_str, &uint_str);
//      KDEBUG_LOG(0,"GET V:%s",uint_str.c_str()+5);
        if (m_status.IsNotFound()) {
//          KDEBUG_LOG(0,"CAN'T FIND KV:%u ",id);
        }
        StorageUnit uint(uint_str);
        /*for temp userage*/
        if(uint.time_available()){
            value_str = uint.data();
			RawKVdbBuffer buffer_value;
			RawKVdbBuffer buffer_set;
            buffer_value << value_str;
			while (buffer_value.get_remain_size() >= sizeof(ValueType) ) {
				ValueType value_tmp;
				buffer_value >> value_tmp;
				if(! (value == value_tmp)) {
					buffer_set << value_tmp;
				}	
			}
			insert_KV(key,buffer_set, uint.get_exptime());
        }
	}
}

template<class KeyType>
void Column::delete_KV(KeyType& key) {
	if(m_create == true) {
        RawKVdbBuffer buffer_key;
		buffer_key << key;
		std::string key_str;
		buffer_key >> key_str;
        m_status = m_db->Delete(leveldb::WriteOptions(), key_str);
    }
}


template<typename KeyType, typename ValueType>
void Column::get_all(std::multimap<KeyType, ValueType>& value_map) {
if (m_create == true) {
        leveldb::Iterator* it = m_db->NewIterator(leveldb::ReadOptions());
        for (it->SeekToFirst(); it->Valid(); it->Next()) {
            std::string K = it->key().ToString();
			RawKVdbBuffer buffer_key;
			buffer_key << K;
			KeyType key;
			buffer_key >> key;
            std::string V = it->value().ToString();
//          KDEBUG_LOG(0,"GET ALL KV :%s %s",K.c_str(),V.c_str());
            StorageUnit uint(V);
            if (uint.time_available()) {
                KVdbBuffer<struct empty_head> buffer;
                buffer << (uint.data());
                while(buffer.get_remain_size() >= sizeof(ValueType)){
                    ValueType value;
                    buffer >> value;
                    value_map.insert(std::pair<KeyType,ValueType>(key, value));
                }
            }
        }
        if (!it->status().ok()) {
  //          KERROR_LOG(0,"GET ALL NUM ERROR :%s",m_name.c_str());
        }
        delete it;
    }
}

template<typename KeyType, typename ValueType>
void Column::get_all_count(std::map<KeyType, uint32_t>& value_map) {
if (m_create == true) {
        leveldb::Iterator* it = m_db->NewIterator(leveldb::ReadOptions());
        for (it->SeekToFirst(); it->Valid(); it->Next()) {
            std::string K = it->key().ToString();
			RawKVdbBuffer buffer_key;
			buffer_key << K;
			KeyType key;
			buffer_key >> key;
            std::string V = it->value().ToString();
//          KDEBUG_LOG(0,"GET ALL KV :%s %s",K.c_str(),V.c_str());
            StorageUnit uint(V);
            if (uint.time_available()) {
                KVdbBuffer<struct empty_head> buffer;
                buffer << (uint.data());
				uint32_t count = 0;
                while(buffer.get_remain_size() >= sizeof(ValueType)){
                    ValueType value;
                    buffer >> value;
					count ++;
                }
                value_map.insert(std::pair<KeyType,uint32_t>(key, count));
            }
        }
        if (!it->status().ok()) {
    //        KERROR_LOG(0,"GET ALL NUM ERROR :%s",m_name.c_str());
        }
        delete it;
    }
}

template<class ValueType>
void Column::random_get_KV(ValueType& value) {
	if( m_random_buffer.get_remain_size() >= sizeof(ValueType) ){
		m_random_buffer >> value;
		return;
	}else {
		m_random_buffer .clear();
	}
    if (m_create == true) {
        if (m_random_itr == NULL || !m_random_itr->Valid()) {
            delete m_random_itr;
            m_random_itr = m_db->NewIterator(leveldb::ReadOptions());
            m_random_itr->SeekToFirst();
        };
        if( !m_random_itr->Valid()){
            return;
        }
        leveldb::Slice K  = m_random_itr->value();
        do {
         /*   std::string K = m_random_itr->key().ToString();
            std::stringstream tmp_stream;
            tmp_stream << K.c_str();
            uint32_t id = 0;
            tmp_stream >> id;*/
            std::string V = m_random_itr->value().ToString();
//          KDEBUG_LOG(0,"GET RANDOM KV :%s",K.c_str());
            StorageUnit uint(V);
            if (uint.time_available()) {
                m_random_buffer << uint.data();
				m_random_buffer >> value;
                m_random_itr->Next();
                return;
            }else{
                m_random_itr->Next();
                if(!m_random_itr->Valid()){
                    delete m_random_itr;
                    m_random_itr = m_db->NewIterator(leveldb::ReadOptions());
                    m_random_itr->SeekToFirst();
                    if(!m_random_itr->Valid()) {
                        return;
                    }
                }
            }
        }while (m_random_itr->value() != K);
    }
    return;
}

}

#endif /* COLUMN_H_ */
