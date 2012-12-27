
/*得到在线人数*/
void sw_get_user_count( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	sw_get_user_count_out * p_out = P_OUT;
	


}


/*online 上报启动信息*/
void sw_report_online_info( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*上报用户上下线*/
void sw_report_user_onoff( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*请求用户离线*/
void sw_set_user_offline( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/**/
void sw_noti_across_svr( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	sw_noti_across_svr_out * p_out = P_OUT;
	


}


/*得到推荐服务器列表*/
void sw_get_recommend_svr_list( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	sw_get_recommend_svr_list_out * p_out = P_OUT;
	


}


/*得到范围列表*/
void sw_get_ranged_svrlist( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	sw_get_ranged_svrlist_out * p_out = P_OUT;
	


}

