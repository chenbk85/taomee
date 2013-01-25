#ifndef  POP_DB_ENUM_H
#define  POP_DB_ENUM_H

/*命令定义*/	
enum  enum_pop_db_cmd{
	 pop_login_cmd		=	0x7401, /*登入*/
	 pop_reg_cmd		=	0x7402, /*注册*/
	 pop_exchange_cmd		=	0x740A, /*交换物品*/
	 pop_task_complete_node_cmd		=	0x7410, /*完成子节点任务*/
	 pop_task_get_list_cmd		=	0x7411, /*得到完成任务的进度列表*/

};

/*错误码定义*/	
enum  enum_pop_db_error{
	 __SUCC__		=	0, /*成功*/

};



#endif // POP_DB_ENUM_H
