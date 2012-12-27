/*得到在线人数*/
int sw_get_user_count(Csprite *p, Cmessage* c_in)
{
	sw_get_user_count_out	cli_out ;



	return 0;
}

/*online 上报启动信息*/
int sw_report_online_info(Csprite *p, Cmessage* c_in)
{
	sw_report_online_info_in * p_in = P_IN;



	return 0;
}

/*上报用户上下线*/
int sw_report_user_onoff(Csprite *p, Cmessage* c_in)
{
	sw_report_user_onoff_in * p_in = P_IN;



	return 0;
}

/*请求用户离线*/
int sw_set_user_offline(Csprite *p, Cmessage* c_in)
{



	return 0;
}

/**/
int sw_noti_across_svr(Csprite *p, Cmessage* c_in)
{
	sw_noti_across_svr_in * p_in = P_IN;
	sw_noti_across_svr_out	cli_out ;



	return 0;
}

/*得到推荐服务器列表*/
int sw_get_recommend_svr_list(Csprite *p, Cmessage* c_in)
{
	sw_get_recommend_svr_list_out	cli_out ;



	return 0;
}

/*得到范围列表*/
int sw_get_ranged_svrlist(Csprite *p, Cmessage* c_in)
{
	sw_get_ranged_svrlist_in * p_in = P_IN;
	sw_get_ranged_svrlist_out	cli_out ;



	return 0;
}

