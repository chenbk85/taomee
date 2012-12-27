/**/
int userinfo_check_pay_password(Csprite *p, Cmessage* c_in)
{
	userinfo_check_pay_password_in * p_in = P_IN;



	return 0;
}

/**/
int userinfo_set_pay_password(Csprite *p, Cmessage* c_in)
{
	userinfo_set_pay_password_in * p_in = P_IN;



	return 0;
}

/*用户是否设置过支付密码*/
int userinfo_isset_pay_password(Csprite *p, Cmessage* c_in)
{
	userinfo_isset_pay_password_out	cli_out ;



	return 0;
}

/*检查用户的游戏激活标识*/
int userinfo_get_gameflag(Csprite *p, Cmessage* c_in)
{
	userinfo_get_gameflag_in * p_in = P_IN;
	userinfo_get_gameflag_out	cli_out ;



	return 0;
}

/**/
int vip_add_days(Csprite *p, Cmessage* c_in)
{
	vip_add_days_in * p_in = P_IN;
	vip_add_days_out	cli_out ;



	return 0;
}

/**/
int vip_sub_days(Csprite *p, Cmessage* c_in)
{
	vip_sub_days_in * p_in = P_IN;
	vip_sub_days_out	cli_out ;



	return 0;
}

/**/
int vip_get_info(Csprite *p, Cmessage* c_in)
{
	vip_get_info_out	cli_out ;



	return 0;
}

/*查询米币余额*/
int pay_get_mb_left(Csprite *p, Cmessage* c_in)
{
	pay_get_mb_left_out	cli_out ;



	return 0;
}

/*游戏内用米币直接开通VIP*/
int pay_become_vip(Csprite *p, Cmessage* c_in)
{
	pay_become_vip_in * p_in = P_IN;
	pay_become_vip_out	cli_out ;



	return 0;
}

/*获取VIP开通价格*/
int pay_get_become_vip_price(Csprite *p, Cmessage* c_in)
{
	pay_get_become_vip_price_out	cli_out ;



	return 0;
}

/*主登录*/
int main_login_login(Csprite *p, Cmessage* c_in)
{
	main_login_login_in * p_in = P_IN;
	main_login_login_out	cli_out ;



	return 0;
}

/*检查session*/
int main_login_check_session(Csprite *p, Cmessage* c_in)
{
	main_login_check_session_in * p_in = P_IN;



	return 0;
}

/**/
int main_login_with_verif_img(Csprite *p, Cmessage* c_in)
{
	main_login_with_verif_img_in * p_in = P_IN;



	return 0;
}

/*验证session*/
int main_login_check_session_new(Csprite *p, Cmessage* c_in)
{
	main_login_check_session_new_in * p_in = P_IN;



	return 0;
}

/*加session*/
int main_login_add_session(Csprite *p, Cmessage* c_in)
{
	main_login_add_session_in * p_in = P_IN;
	main_login_add_session_out	cli_out ;



	return 0;
}

/*增加项目*/
int main_login_add_game(Csprite *p, Cmessage* c_in)
{
	main_login_add_game_in * p_in = P_IN;



	return 0;
}

/*增加游戏项目（带渠道号）*/
int main_login_add_game_with_chanel(Csprite *p, Cmessage* c_in)
{
	main_login_add_game_with_chanel_in * p_in = P_IN;



	return 0;
}

