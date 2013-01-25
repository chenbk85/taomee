/*
 * =========================================================================
 *
 *        Filename: leveldb.h
 *
 *        Version:  1.0
 *        Created:  2012-04-20 01:32:50
 *        Description: leveldb实现 
 *
 *        Author:  francisco (francisco@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */

#ifndef  LEVELDB_H
#define  LEVELDB_H

#include "KVdb/KVdbAccess.hpp"
#include "KVdb/Column.hpp"
#include "KVdb/Protocal.hpp"

//#define  POLLEN_TYPE    uint16_t
    
#pragma pack(1)
typedef struct {
    uint32_t value;
}PollenType;
#pragma pack()
class CLevelDB {
	private:
		KVdb::ColumnManager* manager;
		KVdb::KVdbRawAccess* KVaccessor;
	public:
		CLevelDB() {
			manager = new KVdb::ColumnManager("./cache_data/");
		    KVaccessor = new KVdb::KVdbRawAccess(manager);
		}
		~CLevelDB(){
			delete manager;
			delete KVaccessor; 
		}
		uint32_t get_value_uint32(const char* col, uint32_t uid) {
			uint32_t v=0;
    		KVaccessor->get(uid,col,v);
			return v;
		}
		void set_value_uint32(const char* col, uint32_t uid,uint32_t value, uint32_t time){
    		KVaccessor->set(uid, col, value, time);
		}
};
#endif  /*LEVEDB_H*/
