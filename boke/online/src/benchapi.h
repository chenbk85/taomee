#ifndef ONLINE_BENCHAPI_H_
#define ONLINE_BENCHAPI_H_

#include <time.h>
#include <stdint.h>
#include <glib.h>
#include <vector>
#include <map>
#include <set>
#include <list>

#ifdef __cplusplus
extern "C"
{
#endif
#include <libtaomee/timer.h>
#include <libtaomee/log.h>
#include <libtaomee/list.h>
#ifdef __cplusplus
}
#endif

#include  <libtaomee++/utils/strings.hpp>

#ifndef likely
#define likely(x)  __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x)  __builtin_expect(!!(x), 0)
#endif

typedef uint32_t userid_t;

typedef struct protocol {
	uint32_t	len;
	uint16_t	cmd;
	userid_t	timestamp;
	int32_t		seq;
	uint16_t	ret;
	uint16_t	useSever;
} __attribute__((packed)) protocol_t;

/**
  * @brief db protocol type
  */
typedef struct db_proto {
	/*! package length */
	uint32_t	len;
	/*! sequence number ((p->fd << 16) | p->waitcmd) */
	uint32_t	seq;
	uint16_t	cmd;
	uint32_t	ret;
	userid_t	id;
	uint8_t		body[];
}__attribute__((packed))db_proto_t;

class Citem{
	public:
		uint32_t itemid;
		uint32_t count;
};

inline void init_item_list(const char * init_buf, std::vector<Citem>& item_list )
{
	std::vector<std::string> row_list=split(init_buf,',');
	Citem item;
	for( uint32_t i=0 ; i<row_list.size() ; i++ ){
		item.itemid=atol(row_list[i].c_str());
		item.count=1;
		if (item.itemid >0 ){
			item_list.push_back(item);
		}
	}
}

inline void init_item_count_list(const char* init_buf, std::vector<Citem>& item_list)
{
	std::vector<std::string> row_list=split(init_buf,' ');
	Citem item;
	for( uint32_t i=0 ; i<row_list.size() ; i++ ){
		std::vector<std::string> item_str_list = split(row_list[i], ':');
		item.itemid=atol(item_str_list[0].c_str());
		item.count=1;
		if (item_str_list.size() > 1)
			item.count = atol(item_str_list[1].c_str());
		if (item.itemid >0 ){
			item_list.push_back(item);
			//DEBUG_LOG("== push item:id=%u ,count=%u",item.itemid,item.count );
		}
	}
}	

inline void init_int_list(const char * init_buf, std::vector<uint32_t>& item_list )
{
	//空的不处理
	if (init_buf==NULL) return;

	std::vector<std::string> row_list=split(init_buf,',');
	for( uint32_t i=0 ; i<row_list.size() ; i++ ){
		uint32_t value=atol(row_list[i].c_str());
		 if (value>0){
		 	//DEBUG_LOG("== push int:%u ",value );
		 	item_list.push_back(value) ;
		 }
	}
}

inline void init_int_list(const char * init_buf, std::set<uint32_t>& item_list )
{
	//空的不处理
	if (init_buf==NULL) return;

	std::vector<std::string> row_list=split(init_buf,',');
	for( uint32_t i=0 ; i<row_list.size() ; i++ ){
		uint32_t value=atol(row_list[i].c_str());
		 if (value>0){
		 	//DEBUG_LOG("== push int:%u ",value );
			item_list.insert(value);
		 }
	}
}

inline bool init_time_range(const char * str, uint32_t& start, uint32_t& end)
{
	std::vector<std::string> row_list = split(str, '-');
	if (row_list.size() != 2)
		return false;

	start = 0;
	end = 0;

	uint32_t val = 0;

	std::vector<std::string> time_val = split(row_list[0].c_str(), ':');
	switch (time_val.size()) {
	case 3:
		val = atol(time_val[2].c_str());
		if (val >= 60) return false;
		start += val;
	case 2:
		val = atol(time_val[1].c_str());
		if (val >= 60) return false;
		start += val * 60;
	case 1:
		val = atol(time_val[0].c_str());
		if (val >= 60) return false;
		start += val * 3600;
		break;
	default:
		return false;
	}

	time_val = split(row_list[1].c_str(), ':');
	switch (time_val.size()) {
	case 3:
		val = atol(time_val[2].c_str());
		if (val >= 60) return false;
		end += val;
	case 2:
		val = atol(time_val[1].c_str());
		if (val >= 60) return false;
		end += val * 60;
	case 1:
		val = atol(time_val[0].c_str());
		if (val >= 60) return false;
		end += val * 3600;
		break;
	default:
		return false;
	}

	return start < end;
}

inline bool init_time_list(const char * init_buf, std::map<uint32_t, uint32_t>* time_map_list)
{
	//空的不处理
	if (init_buf==NULL) return false;

	DEBUG_LOG("buf\t[%s]", init_buf);

	bool ret = false;

	std::vector<std::string> row_list=split(init_buf,',');
	for( uint32_t i=0 ; i<row_list.size() ; i++ ){
		std::vector<std::string> time_str = split(row_list[i].c_str(), '/');
		uint32_t start, end;
		if (time_str.size() == 1 && init_time_range(time_str[0].c_str(), start, end)) {
			for (int j = 0; j < 7; j ++) {
				time_map_list[j][start] = end;
			}
			ret = true;
		} else if (time_str.size() == 2 && init_time_range(time_str[1].c_str(), start, end)) {
			uint32_t day = atol(time_str[0].c_str());
			if (day > 0 && day <= 7) {
				time_map_list[day-1][start] = end;
				ret = true;
			}
		}
	}
	return ret;
}
#endif
