#ifndef  MAIN_LOGIN_H
#define  MAIN_LOGIN_H
#include "main.h"

class main_login_add_game_in: public Cmessage {
	public:
		/*渠道id*/
		uint16_t		channel_id;
		/**/
		char		verify_code[32]; 
		/**/
		uint32_t		idczone;
		/**/
		uint32_t		gameid;

		main_login_add_game_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class main_login_add_game_with_chanel_in: public Cmessage {
	public:
		/*服务器验证渠道码*/
		uint16_t		channel_code;
		/**/
		char		verify_code[32]; 
		/**/
		uint32_t		idczone;
		/**/
		uint32_t		gameid;
		/*渠道id*/
		uint16_t		channel_id;

		main_login_add_game_with_chanel_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class main_login_add_session_in: public Cmessage {
	public:
		/*渠道id*/
		uint16_t		channel_id;
		/**/
		char		verify_code[32]; 
		/**/
		uint32_t		gameid;
		/*网络序*/
		uint32_t		ip;

		main_login_add_session_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class main_login_add_session_out: public Cmessage {
	public:
		/**/
		char		session[16]; 

		main_login_add_session_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class main_login_check_session_in: public Cmessage {
	public:
		/**/
		uint32_t		gameid;
		/**/
		char		session[16]; 
		/**/
		uint32_t		del_session_flag;

		main_login_check_session_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class main_login_check_session_new_in: public Cmessage {
	public:
		/*代签的起始产品，从from_game代签到to_game*/
		uint32_t		from_game;
		/*session内容*/
		char		session[16]; 
		/*删除此session标示,0验证通过后session仍然有效，1验证通过后session失效*/
		uint32_t		del_session_flag;
		/*代签的目标产品，从from_game代签到to_game，用于统计代签登录的人数*/
		uint32_t		to_game;
		/*客户端ip，网络字节序*/
		uint32_t		ip;
		/*游戏分区信息：0=电信，1=网通*/
		uint16_t		region;
		/*是否进入游戏，1表示进入online服务器*/
		uint8_t		enter_game;
		/*登录渠道推广广告位，用于统计广告位带来的登录统计，如果没有推广全部填’\0’,如:media.4399.top.left等， 该字段目前为变长字符串，最长128个字节，以’\0’作为字符串结尾。*/
		uint8_t		tad[128]; 

		main_login_check_session_new_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class main_login_login_in: public Cmessage {
	public:
		/**/
		char		email[64]; 
		/**/
		char		passwd_md5_two[16]; 
		/*渠道id*/
		uint16_t		channel_id;
		/**/
		uint16_t		region;
		/**/
		uint16_t		gameid;
		/*网络序*/
		uint32_t		ip;

		main_login_login_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class main_login_login_out: public Cmessage {
	public:
		/**/
		char		session[16]; 
		/**/
		uint32_t		gameflag;

		main_login_login_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class main_login_with_verif_img_in: public Cmessage {
	public:
		/**/
		char		email[64]; 
		/**/
		char		passwd_md5_two[16]; 
		/*渠道id*/
		uint16_t		channel_id;
		/**/
		uint16_t		region;
		/**/
		uint16_t		gameid;
		/*网络序*/
		uint32_t		ip;
		/*验证码session*/
		char		verif_session[16]; 
		/*验证码*/
		char		verif_code[6]; 
		/*渠道码*/
		uint16_t		channel_code;

		main_login_with_verif_img_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pay_become_vip_in: public Cmessage {
	public:
		/*渠道id*/
		uint16_t		channel_id;
		/*安全码*/
		char		verify_code[32]; 
		/*0：非自动续费 1：自动续费*/
		uint8_t		is_auto_charge;
		/*0：不使用优惠券 1：使用优惠码 2：使用抵用券或折扣券*/
		uint8_t		use_coupon;
		/*9字节优惠码，当use_coupon=1时有效*/
		char		coupon[9]; 
		/*抵用券或折扣券类型，当use_coupon=2时有效 （11：摩尔2元 12：摩尔5元 13：摩尔20元 61：赛尔二7折 62：赛尔二8折 63：赛尔二9折）*/
		uint16_t		voucher;
		/*0：电信 1：网通*/
		uint8_t		which_line;
		/*扩展字段，暂时用不到，填NULL*/
		char		extend[19]; 

		pay_become_vip_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pay_become_vip_out: public Cmessage {
	public:
		/*扣取米币数（100倍；比如用户剩余1米币，该字段返回值为100）*/
		uint32_t		mb_consumed;
		/*米币账户余额（100倍；比如用户剩余1米币，该字段返回值为100）*/
		uint32_t		mb_balance;

		pay_become_vip_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pay_get_become_vip_price_out: public Cmessage {
	public:
		/*开通一个月VIP花费米币数（100倍）*/
		uint32_t		month1_price;
		/*开通三个月VIP花费米币数（100倍）*/
		uint32_t		month3_price;
		/*开通六个月VIP花费米币数（100倍）*/
		uint32_t		month6_price;
		/*开通十二个月VIP花费米币数（100倍）*/
		uint32_t		month12_price;

		pay_get_become_vip_price_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class pay_get_mb_left_out: public Cmessage {
	public:
		/*米币账户余额（100倍；比如用户剩余1米币，该字段返回值为100）*/
		uint32_t		mb_balance;

		pay_get_mb_left_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class userinfo_check_pay_password_in: public Cmessage {
	public:
		/*支付密码*/
		uint8_t		password[16]; 

		userinfo_check_pay_password_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class userinfo_get_gameflag_in: public Cmessage {
	public:
		/*忽略字段，添0*/
		uint32_t		region;

		userinfo_get_gameflag_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class userinfo_get_gameflag_out: public Cmessage {
	public:
		/*0非米饭，1米饭*/
		uint32_t		mee_fans;
		/*电信激活flag*/
		uint32_t		game_act_flag;
		/*电信gameid或运算结果*/
		uint32_t		dx_gameflag;
		/*网通gameid或运算结果*/
		uint32_t		wt_gameflag;

		userinfo_get_gameflag_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class userinfo_isset_pay_password_out: public Cmessage {
	public:
		/*0没有设置，非0设置过*/
		uint32_t		is_seted;

		userinfo_isset_pay_password_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class userinfo_set_pay_password_in: public Cmessage {
	public:
		/*支付密码*/
		uint8_t		password[16]; 

		userinfo_set_pay_password_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class vip_add_days_in: public Cmessage {
	public:
		/**/
		uint32_t		days;
		/**/
		uint32_t		add_flag;
		/**/
		uint32_t		channel_id;
		/**/
		char		trade_id[20]; 

		vip_add_days_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class vip_add_days_out: public Cmessage {
	public:
		/**/
		uint32_t		op_id;
		/**/
		uint32_t		vip_ret;

		vip_add_days_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class vip_get_info_out: public Cmessage {
	public:
		/**/
		uint32_t		vip_result;
		/*0：非自动续费 1：自动续费*/
		uint32_t		vip_is_auto_charge;
		/**/
		uint32_t		vip_begin_time;
		/**/
		uint32_t		vip_end_time;
		/*渠道id*/
		uint32_t		vip_channel_id;

		vip_get_info_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class vip_sub_days_in: public Cmessage {
	public:
		/**/
		uint32_t		days;
		/**/
		uint32_t		channel_id;
		/**/
		char		trade_id[20]; 

		vip_sub_days_in();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
class vip_sub_days_out: public Cmessage {
	public:
		/**/
		uint32_t		op_id;
		/**/
		uint32_t		vip_ret;
		/**/
		uint32_t		vip_endtime;

		vip_sub_days_out();
		void init();
		bool read_from_buf(byte_array_t & ba );
		bool write_to_buf(byte_array_t & ba );
};

	
#endif // MAIN_LOGIN_H
