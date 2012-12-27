#ifndef  POP_SWITCH_ENUM_H
#define  POP_SWITCH_ENUM_H

/*命令定义*/	
enum  enum_pop_switch_cmd{
	 sw_get_user_count_cmd		=	61001, /*得到在线人数*/
	 sw_report_online_info_cmd		=	62001, /*online 上报启动信息*/
	 sw_report_user_onoff_cmd		=	62002, /*上报用户上下线*/
	 sw_set_user_offline_cmd		=	62003, /*请求用户离线*/
	 sw_noti_across_svr_cmd		=	62004, /**/
	 sw_get_recommend_svr_list_cmd		=	62006, /*得到推荐服务器列表*/
	 sw_get_ranged_svrlist_cmd		=	64002, /*得到范围列表*/

};

/*错误码定义*/	
enum  enum_pop_switch_error{
	 __SUCC__		=	0, /*成功*/

};



#endif // POP_SWITCH_ENUM_H
