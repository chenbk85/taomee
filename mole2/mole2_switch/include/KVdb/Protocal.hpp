/**
 * =====================================================================================
 *      @file  Protocal.hpp
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

#ifndef PROTOCAL_HPP_
#define PROTOCAL_HPP_

#include "config.h"
#include <string.h>
#include <vector>
#include <set>
#include <string>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define		NULL_OP 0
#define		GET_OP 	1
#define		SET_OP 	2
#define		APPEND_OP 3
#define		COUNT_OP 4

#define DEFAULT_DATA_SIZE (4*1024)

template<class HeadType>
class KVdbBuffer {
public:
	/*for new one*/
	KVdbBuffer() :
		buffer(NULL), capacity(sizeof(HeadType) + DEFAULT_DATA_SIZE), package_size(sizeof(HeadType)), read_pos(sizeof(HeadType)), m_bComplete(false) {
		buffer = (char*) malloc(sizeof(HeadType) + DEFAULT_DATA_SIZE);
		assert(buffer!=NULL);
		memset(buffer, 0, sizeof(HeadType) + DEFAULT_DATA_SIZE);
	}

	void clear(){
		if (buffer != NULL) {
            free(buffer);
            buffer = NULL;
        }
		buffer = (char*) malloc(sizeof(HeadType) + DEFAULT_DATA_SIZE);
		memset(buffer, 0, sizeof(HeadType) + DEFAULT_DATA_SIZE);
		capacity = (sizeof(HeadType) + DEFAULT_DATA_SIZE);
		package_size = (sizeof(HeadType));
		read_pos = (sizeof(HeadType));
	 	m_bComplete = (false);
	}
	/*for receive and call bakc*/
	KVdbBuffer(char* msg, size_t length) :
		buffer(NULL), capacity(sizeof(HeadType) + DEFAULT_DATA_SIZE), package_size(sizeof(HeadType)), read_pos(sizeof(HeadType)) {
		if (length < sizeof(HeadType)) {
			m_bComplete = false;
		} else {
			buffer = (char*) malloc(length);
			assert(buffer!=NULL);
			package_size = length;
			memcpy(&buffer[0], msg, length);
			m_bComplete = true;
		}
	}

	KVdbBuffer(const KVdbBuffer& KVdb_buffer) {
		capacity = KVdb_buffer.capacity;
		package_size = KVdb_buffer.package_size - KVdb_buffer.read_pos;
		read_pos = 0;
		m_bComplete = KVdb_buffer.m_bComplete;
		buffer = (char*) malloc(package_size);
		memcpy(&buffer[0], &KVdb_buffer.buffer[0], package_size);
		assert(buffer!=NULL);
	}

	KVdbBuffer& operator=(const KVdbBuffer& KVdb_buffer) {
		if (this == &KVdb_buffer) {
			return *this;
		};
		capacity = KVdb_buffer.capacity;
		package_size = KVdb_buffer.package_size;
		read_pos = KVdb_buffer.read_pos;
		m_bComplete = KVdb_buffer.m_bComplete;
		if (buffer != NULL) {
			free(buffer);
		}
		buffer = (char*) malloc(package_size);
		memcpy(&buffer[0], &KVdb_buffer.buffer[0], package_size);
		assert(buffer!=NULL);
		return *this;
	}

	~KVdbBuffer() {
		if (buffer != NULL) {
			free(buffer);
			buffer = NULL;
		}
	}
	
	char* Value() {
		return &buffer[0];
	}

	char* Data() {
		return &buffer[read_pos];
	}

	size_t size() {
		return package_size;
	}

	HeadType* get_head() {
		return (HeadType*) &buffer[0];
	}
	;

	char* get_value() {
		if (m_bComplete) {
			return &buffer[sizeof(HeadType)];
		} else {
			return NULL;
		}
	}

	size_t get_value_size() {
		return package_size - sizeof(HeadType);
	}

	size_t get_remain_size() {
		return package_size - read_pos;
	}

	void pass_read_data(size_t size) {
		read_pos += size;
	}

	//	void assign_kV_op(uint32_t op, uint32_t key, std::string column, uint32_t exptime = 0) {
	//		struct KV_op_t* proto_head = (struct KV_op_t*) (&buffer[PROTO_HEAD_SIZE]);
	//		proto_head->command = op;
	//		proto_head->key = key;
	//		proto_head->exptime = exptime;
	//		memcpy(proto_head->column, column.c_str(), column.size() <= COLUMN_NAME_MAX ? column.size() : COLUMN_NAME_MAX);
	//		m_bComplete = true;
	//	}

	KVdbBuffer& operator<<(KVdbBuffer<HeadType>& KVdb_buffer) {
		if (this == &KVdb_buffer) {
            return *this;
        };
        capacity = KVdb_buffer.capacity;
        package_size = KVdb_buffer.package_size;
        read_pos = KVdb_buffer.read_pos;
        m_bComplete = KVdb_buffer.m_bComplete;
        if (buffer != NULL) {
            free(buffer);
        }
        buffer = (char*) malloc(package_size);
        memcpy(&buffer[0], &KVdb_buffer.buffer[0], package_size);
        assert(buffer!=NULL);
        return *this;
	}

	KVdbBuffer& operator>>(KVdbBuffer<HeadType>& KVdb_buffer) {
		if (this == &KVdb_buffer) {
            return *this;
        };
        KVdb_buffer.capacity = capacity;
       	KVdb_buffer.package_size = package_size;
        KVdb_buffer.read_pos = read_pos;
        KVdb_buffer.m_bComplete = m_bComplete;
        if (KVdb_buffer.buffer != NULL) {
            free(KVdb_buffer.buffer);
        }
        KVdb_buffer.buffer = (char*) malloc(package_size);
        memcpy(&KVdb_buffer.buffer[0], &buffer[0], package_size);
        assert(KVdb_buffer.buffer!=NULL);
        return *this;
	}

	template<typename T>
	KVdbBuffer& operator<<(T value) {
		must_be_pod<T> test_be_pod;
		if (!sizeof(test_be_pod)) {
			return *this;
		};
		//		if (capacity < package_size + sizeof(T)) {
		//			buffer = (char*)realloc(buffer, package_size + sizeof(T));
		//			capacity = package_size + sizeof(T);
		//		}
		size_t now_size = package_size;
		expand_size(sizeof(T));
		memcpy(&buffer[now_size], (void *) &value, sizeof(T));
		//		package_size += sizeof(T);
		return *this;
	}

	KVdbBuffer& operator<<(std::string value) {
		//		if (capacity < package_size + value.length()) {
		//			buffer = (char*)realloc(buffer, package_size + value.length());
		//			capacity = package_size + value.length();
		//		}
		size_t now_size = package_size;
		expand_size(value.length());
		memcpy(&buffer[now_size], (void *) value.c_str(), value.length());
		//		package_size += value.length();
		return *this;
	}

	KVdbBuffer& operator<<(const char* value) {
		//		if (capacity < package_size + strlen(value)) {
		//			buffer = (char*)realloc(buffer, package_size + strlen(value));
		//			capacity = package_size + strlen(value);
		//		}
		size_t now_size = package_size;
		expand_size(strlen(value));
		memcpy(&buffer[now_size], value, strlen(value));
		//		package_size += strlen(value);
		return *this;
	}

	template<typename T>
	KVdbBuffer& operator<<(std::vector<T> value) {
		*this << value.size();
		typename std::vector<T>::iterator itr = value.begin();
		for (; itr != value.end(); itr++) {
			*this << *itr;
		}
		return *this;
	}

	template<typename T>
	KVdbBuffer& operator<<(std::set<T> value) {
		*this << value.size();
		typename std::set<T>::iterator itr = value.begin();
		for (; itr != value.end(); itr++) {
			*this << *itr;
		}
		return *this;
	}

	KVdbBuffer& append(KVdbBuffer<HeadType> value) {
		//			if (capacity < package_size + value.get_remain_size()) {
		//				buffer = (char*)realloc(buffer, package_size + value.get_remain_size());
		//				capacity = package_size + value.get_remain_size();
		//			}
		size_t now_size = package_size;
		expand_size(value.get_remain_size());
		memcpy(&buffer[now_size], value.Data(), value.get_remain_size());
		return *this;
	}

	KVdbBuffer& operator<<(char* value) {
		//		if (capacity < package_size + strlen(value)) {
		//			buffer = (char*)realloc(buffer, package_size + strlen(value));
		//			capacity = package_size + strlen(value);
		//		}
		size_t now_size = package_size;
		expand_size(strlen(value));
		memcpy(&buffer[now_size], value, strlen(value));
		//		package_size += strlen(value);
		return *this;
	}

	template<typename T>
	KVdbBuffer& operator>>(T &value) {
		must_be_pod<T> test_be_pod;
		if (!sizeof(test_be_pod)) {
			return *this;
		};
		if (package_size - read_pos >= sizeof(T)) {
			memcpy((char*) &value, &buffer[read_pos], sizeof(T));
			read_pos += sizeof(T);
		} else {
			value = T();
		}
		return *this;
	}

	KVdbBuffer& operator>>(std::string &value) {
		if (package_size > read_pos) {
			value = std::string(&buffer[read_pos], package_size - read_pos);
		} else {
			value = "";
		}
		read_pos = package_size;
		return *this;
	}

	template<typename T>
	KVdbBuffer& operator>>(std::vector<T> value) {
		size_t size = 0;
		*this >> size;
		for (size_t i = 0; i < size; i++) {
			T Temp;
			*this >> Temp;
			value.push_back(Temp);
		}
		return *this;
	}

	template<typename T>
	KVdbBuffer& operator>>(std::set<T> value) {
		size_t size = 0;
		*this >> size;
		for (size_t i = 0; i < size; i++) {
			T Temp;
			*this >> Temp;
			value.insert(Temp);
		}
		return *this;
	}

	virtual void expand_size(size_t size) {
		if (capacity < package_size + size) {
			buffer = (char*) realloc(buffer, package_size + size);
			capacity = package_size + size;
		}
		package_size += size;
	}

	void clear_data() {
		if (package_size > sizeof(HeadType)) {
			package_size = sizeof(HeadType);
		}
	}

	bool available() {
		return m_bComplete;
	}

protected:

	template<class T> struct must_be_pod {
		union {
			T noname;
		};
	};

	char* buffer;
	size_t capacity;
	size_t package_size;
	size_t read_pos;
	bool m_bComplete;
};

/* there are one suggest buffer_head */
#pragma pack(1)
struct KV_op_t {
	/* */
	uint32_t len;
	uint32_t command;
	uint32_t key;
	char column[COLUMN_NAME_MAX];
	uint32_t exptime;
};
#pragma pack()

class NetKVdbBuffer: public KVdbBuffer<struct KV_op_t> {
public:
	NetKVdbBuffer() :
		KVdbBuffer<struct KV_op_t> () {
		this->get_head()->len = sizeof(struct KV_op_t);
	}
	;
	NetKVdbBuffer(char* msg, size_t length) :
		KVdbBuffer<struct KV_op_t> (msg, length) {
		this->get_head()->len = length;
	}
	;
	struct KV_op_t* get_operator_head() {
		return this->get_head();
	}
	;
	void expand_size(size_t size) {
		KVdbBuffer<struct KV_op_t>::expand_size(size);
		get_head()->len = get_value_size() + sizeof(struct KV_op_t);
	}
	;
	void prealloc_size() {
		m_bComplete = true;	
		package_size = get_head()->len;
		if (get_head()->len <= capacity) {
			return;
		} else if (get_head()->len >= capacity) {
			KVdbBuffer<struct KV_op_t>::expand_size(get_head()->len-capacity);
		}
	}
	bool complete() {
		return get_head()->len == (get_value_size() + sizeof(struct KV_op_t));
	}
};

/*empty head */
struct empty_head {
	char pad[];
};

class RawKVdbBuffer: public KVdbBuffer<struct empty_head> {
public:
	RawKVdbBuffer() :
		KVdbBuffer<struct empty_head> () {
	}
	;
	RawKVdbBuffer(char* msg, size_t length) :
		KVdbBuffer<struct empty_head> (msg, length) {
	}
	;
	template<class AssignType>
	RawKVdbBuffer(AssignType value) :
		KVdbBuffer<struct empty_head> () {
		*this << value;
	}
	;
};

class AssicHead {
public:
	//	AssicHead(char* msg, size_t length) {
	//		if (length < 3) {
	//			m_available = false;
	//		}
	//
	//		char *s = *e = msg;
	//		for (int i = 0; i < length; i++) {
	//			if (*e == ' ') {
	//				if (s != e) {
	//					m_cmds.push_back(std::string(msg, e - s));
	//				}
	//				s = e + 1;
	//			}
	//			e++;
	//		}
	//		m_available = true;
	//	}
	//
	//	size_t request_len() {
	//	}
private:
	std::vector<std::string> m_cmds;
	bool m_available;
};

#endif /* PROTOCAL_HPP_ */
