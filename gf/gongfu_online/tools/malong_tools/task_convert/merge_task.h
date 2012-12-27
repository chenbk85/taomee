#ifndef _MERGE_TASK_H_
#define _MERGE_TASK_H_


#include "client_task.h"
#include "server_task.h"

bool merge_task(std::map<int, client_task>& client_tasks, std::map<int, task_data>& server_tasks);






#endif