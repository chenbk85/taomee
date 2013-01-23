#ifndef TASK_H
#define TASK_H

#include "CtableRoute100x10.h"
#include "proto.hpp"
#include "benchapi.h"

#define TASK_BUF_LEN 32
class task: public CtableRoute100x10
{
public:
	task(mysql_interface* db);	
	int add( db_user_id_t db_user_id,  uint32_t taskid, uint32_t buf_len, char *buff);
	int del( db_user_id_t db_user_id,  uint32_t taskid );
	int update( db_user_id_t db_user_id,  uint32_t taskid, uint32_t buf_len, char *buff, uint32_t flag );
	int update_buf( db_user_id_t db_user_id,  uint32_t taskid , uint32_t buf_len ,char * buff );
	int get_flag_list( db_user_id_t db_user_id,uint32_t begin,uint32_t end, std::vector<stru_task_flag > &task_list );
	int get_full_list( db_user_id_t db_user_id, uint32_t begin,uint32_t end, std::vector<stru_task_info > &task_list );
	
};






#endif
