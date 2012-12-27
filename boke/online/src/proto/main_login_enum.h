#ifndef  MAIN_LOGIN_ENUM_H
#define  MAIN_LOGIN_ENUM_H

/*命令定义*/	
enum  enum_main_login_cmd{
	 userinfo_check_pay_password_cmd		=	0x008B, /**/
	 userinfo_set_pay_password_cmd		=	0x008C, /**/
	 userinfo_isset_pay_password_cmd		=	0x008D, /*用户是否设置过支付密码*/
	 userinfo_get_gameflag_cmd		=	0x009B, /*检查用户的游戏激活标识*/
	 vip_add_days_cmd		=	0x2001, /**/
	 vip_sub_days_cmd		=	0x2002, /**/
	 vip_get_info_cmd		=	0x2004, /**/
	 pay_get_mb_left_cmd		=	0x7601, /*查询米币余额*/
	 pay_become_vip_cmd		=	0x7602, /*游戏内用米币直接开通VIP*/
	 pay_get_become_vip_price_cmd		=	0x7603, /*获取VIP开通价格*/
	 main_login_login_cmd		=	0xA021, /*主登录*/
	 main_login_check_session_cmd		=	0xA024, /*检查session*/
	 main_login_with_verif_img_cmd		=	0xA032, /**/
	 main_login_check_session_new_cmd		=	0xA03A, /*验证session*/
	 main_login_add_session_cmd		=	0xA122, /*加session*/
	 main_login_add_game_cmd		=	0xA123, /*增加项目*/
	 main_login_add_game_with_chanel_cmd		=	0xA134, /*增加游戏项目（带渠道号）*/

};

/*错误码定义*/	
enum  enum_main_login_error{
	 SESSION_CHECK_ERR		=	4331, /**/

};



#endif // MAIN_LOGIN_ENUM_H
