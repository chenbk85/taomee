#ifndef  POP_DB_ENUM_H
#define  POP_DB_ENUM_H

/*命令定义*/	
enum  enum_pop_db_cmd{
	 get_server_version_cmd		=	0x1000, /*得到版本信息*/
	 pop_login_cmd		=	0x7401, /*登入*/
	 pop_reg_cmd		=	0x7402, /*注册*/
	 pop_set_nick_cmd		=	0x7403, /*设置昵称*/
	 pop_copy_user_cmd		=	0x7404, /*复制用户*/
	 pop_get_base_info_cmd		=	0x7405, /**/
	 pop_set_flag_cmd		=	0x7406, /*设置用户标识*/
	 pop_set_color_cmd		=	0x7407, /**/
	 pop_exchange_cmd		=	0x740A, /*交换协议，物品，任务，钱*/
	 pop_set_item_used_list_cmd		=	0x740C, /*设置使用列表，会将原有使用的设置为未使用*/
	 pop_task_complete_node_cmd		=	0x7410, /*完成子节点任务*/
	 pop_task_del_cmd		=	0x7411, /*删除任务*/
	 pop_find_map_add_cmd		=	0x741A, /*找到哪个地图*/
	 pop_logout_cmd		=	0x741B, /*退出*/
	 pop_user_log_add_cmd		=	0x741C, /*用户足迹*/
	 pop_get_all_info_cmd		=	0x74A0, /*得到用户的所有信息*/
	 pop_user_set_field_value_cmd		=	0x74A1, /*设置user表某一字段的值*/
	 pop_opt_cmd		=	0x74A2, /*设置相关的值*/
	 pop_add_game_info_cmd		=	0x74A4, /*增加小游戏统计*/
	 udp_post_msg_cmd		=	0xF130, /*udp 投稿*/

};

/*错误码定义*/	
enum  enum_pop_db_error{
	 NO_DEFINE_ERR		=	1000, /*没有定义*/
	 SYS_ERR		=	1001, /*系统出错 一般是内存出错*/
	 DB_ERR		=	1002, /*数据库出错*/
	 NET_ERR		=	1003, /*NET出错*/
	 CMDID_NODEFINE_ERR		=	1004, /*命令ID没有定义*/
	 PROTO_LEN_ERR		=	1005, /*协议长度不符合*/
	 VALUE_OUT_OF_RANGE_ERR		=	1006, /*数值越界*/
	 FLAY_ALREADY_SET_ERR		=	1007, /*要设置的flag和原有一致*/
	 VALUE_ISNOT_FULL_ERR		=	1008, /*数据不完整*/
	 ENUM_OUT_OF_RANGE_ERR		=	1009, /*枚举越界:不在类型的限定范围.(如某一字段只允许[0,1],却填了2)*/
	 PROTO_RETURN_LEN_ERR		=	1010, /*返回报文长度有问题*/
	 DB_DATA_ERR		=	1011, /*数据库中数据有误*/
	 NET_SEND_ERR		=	1012, /*无*/
	 NET_RECV_ERR		=	1013, /*无*/
	 KEY_EXISTED_ERR		=	1014, /*KEY 已经存在了*/
	 KEY_NOFIND_ERR		=	1015, /*KEY 不存在*/
	 DB_CANNOT_UPDATE_ERR		=	1016, /*DB不允许更新*/
	 NET_TIMEOUT_ERR		=	1017, /*网络超时*/
	 CMD_EXEC_MAX_PER_MINUTE_ERR		=	1018, /*当然命令号每分钟超过最大限制*/
	 CMD_MD5_TAG_ERR		=	1021, /*当然命令的md5_tags检查失败*/
	 CHECK_PASSWD_ERR		=	1103, /*登入时，检查用户名和密码出错*/
	 USER_ID_EXISTED_ERR		=	1104, /*在insert，米米号已存在*/
	 USER_ID_NOFIND_ERR		=	1105, /*在select update, delete 时，米米号不存在*/
	 ITEM_IS_EXISTED_ERR		=	5120, /*物品已经存在*/
	 ITEM_NOFIND_ERR		=	5121, /*物品不存在*/
	 ITEM_NOENOUGH_ERR		=	5122, /*物品不足*/
	 ITEM_MAX_ERR		=	5123, /*物品太多*/
	 TASK_IS_EXISTED_ERR		=	5131, /*任务已经存在*/

};



#endif // POP_DB_ENUM_H
