<?php
require_once("proto_base.php");

class main_login_add_game_in {
	/* 渠道id */
	#类型:uint16
	public $channel_id;

	/*  */
	#定长数组,长度:32, 类型:char 
	public $verify_code ;

	/*  */
	#类型:uint32
	public $idczone;

	/*  */
	#类型:uint32
	public $gameid;


	public function main_login_add_game_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint16($this->channel_id)) return false;
		if(!$ba->read_buf($this->verify_code,32)) return false;
		if (!$ba->read_uint32($this->idczone)) return false;
		if (!$ba->read_uint32($this->gameid)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint16($this->channel_id)) return false;
		$ba->write_buf($this->verify_code,32);
		if (!$ba->write_uint32($this->idczone)) return false;
		if (!$ba->write_uint32($this->gameid)) return false;
		return true;
	}

};

	
class main_login_add_game_with_chanel_in {
	/* 服务器验证渠道码 */
	#类型:uint16
	public $channel_code;

	/*  */
	#定长数组,长度:32, 类型:char 
	public $verify_code ;

	/*  */
	#类型:uint32
	public $idczone;

	/*  */
	#类型:uint32
	public $gameid;

	/* 渠道id */
	#类型:uint16
	public $channel_id;


	public function main_login_add_game_with_chanel_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint16($this->channel_code)) return false;
		if(!$ba->read_buf($this->verify_code,32)) return false;
		if (!$ba->read_uint32($this->idczone)) return false;
		if (!$ba->read_uint32($this->gameid)) return false;
		if (!$ba->read_uint16($this->channel_id)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint16($this->channel_code)) return false;
		$ba->write_buf($this->verify_code,32);
		if (!$ba->write_uint32($this->idczone)) return false;
		if (!$ba->write_uint32($this->gameid)) return false;
		if (!$ba->write_uint16($this->channel_id)) return false;
		return true;
	}

};

	
class main_login_add_session_in {
	/* 渠道id */
	#类型:uint16
	public $channel_id;

	/*  */
	#定长数组,长度:32, 类型:char 
	public $verify_code ;

	/*  */
	#类型:uint32
	public $gameid;

	/* 网络序 */
	#类型:uint32
	public $ip;


	public function main_login_add_session_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint16($this->channel_id)) return false;
		if(!$ba->read_buf($this->verify_code,32)) return false;
		if (!$ba->read_uint32($this->gameid)) return false;
		if (!$ba->read_uint32($this->ip)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint16($this->channel_id)) return false;
		$ba->write_buf($this->verify_code,32);
		if (!$ba->write_uint32($this->gameid)) return false;
		if (!$ba->write_uint32($this->ip)) return false;
		return true;
	}

};

	
class main_login_add_session_out {
	/*  */
	#定长数组,长度:16, 类型:char 
	public $session ;


	public function main_login_add_session_out(){

	}

	public function read_from_buf($ba ){
		if(!$ba->read_buf($this->session,16)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		$ba->write_buf($this->session,16);
		return true;
	}

};

	
class main_login_check_session_in {
	/*  */
	#类型:uint32
	public $gameid;

	/*  */
	#定长数组,长度:16, 类型:char 
	public $session ;

	/*  */
	#类型:uint32
	public $del_session_flag;


	public function main_login_check_session_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->gameid)) return false;
		if(!$ba->read_buf($this->session,16)) return false;
		if (!$ba->read_uint32($this->del_session_flag)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->gameid)) return false;
		$ba->write_buf($this->session,16);
		if (!$ba->write_uint32($this->del_session_flag)) return false;
		return true;
	}

};

	
class main_login_check_session_new_in {
	/* 代签的起始产品，从from_game代签到to_game */
	#类型:uint32
	public $from_game;

	/* session内容 */
	#定长数组,长度:16, 类型:char 
	public $session ;

	/* 删除此session标示,0验证通过后session仍然有效，1验证通过后session失效 */
	#类型:uint32
	public $del_session_flag;

	/* 代签的目标产品，从from_game代签到to_game，用于统计代签登录的人数 */
	#类型:uint32
	public $to_game;

	/* 客户端ip，网络字节序 */
	#类型:uint32
	public $ip;

	/* 游戏分区信息：0=电信，1=网通 */
	#类型:uint16
	public $region;

	/* 是否进入游戏，1表示进入online服务器 */
	#类型:uint8
	public $enter_game;

	/* 登录渠道推广广告位，用于统计广告位带来的登录统计，如果没有推广全部填’\0’,如:media.4399.top.left等， 该字段目前为变长字符串，最长128个字节，以’\0’作为字符串结尾。 */
	#定长数组,长度:128, 类型:uint8_t 
	public $tad =array();


	public function main_login_check_session_new_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->from_game)) return false;
		if(!$ba->read_buf($this->session,16)) return false;
		if (!$ba->read_uint32($this->del_session_flag)) return false;
		if (!$ba->read_uint32($this->to_game)) return false;
		if (!$ba->read_uint32($this->ip)) return false;
		if (!$ba->read_uint16($this->region)) return false;
		if (!$ba->read_uint8($this->enter_game)) return false;
		$this->tad=array();
		{for($i=0;$i<128;$i++){
			if (!$ba->read_uint8($this->tad[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->from_game)) return false;
		$ba->write_buf($this->session,16);
		if (!$ba->write_uint32($this->del_session_flag)) return false;
		if (!$ba->write_uint32($this->to_game)) return false;
		if (!$ba->write_uint32($this->ip)) return false;
		if (!$ba->write_uint16($this->region)) return false;
		if (!$ba->write_uint8($this->enter_game)) return false;
		if(count($this->tad)!=128) return false; 
		{for($i=0; $i<128;$i++){
			if (!$ba->write_uint8($this->tad[$i])) return false;
		}}
		return true;
	}

};

	
class main_login_login_in {
	/*  */
	#定长数组,长度:64, 类型:char 
	public $email ;

	/*  */
	#定长数组,长度:16, 类型:char 
	public $passwd_md5_two ;

	/* 渠道id */
	#类型:uint16
	public $channel_id;

	/*  */
	#类型:uint16
	public $region;

	/*  */
	#类型:uint16
	public $gameid;

	/* 网络序 */
	#类型:uint32
	public $ip;


	public function main_login_login_in(){

	}

	public function read_from_buf($ba ){
		if(!$ba->read_buf($this->email,64)) return false;
		if(!$ba->read_buf($this->passwd_md5_two,16)) return false;
		if (!$ba->read_uint16($this->channel_id)) return false;
		if (!$ba->read_uint16($this->region)) return false;
		if (!$ba->read_uint16($this->gameid)) return false;
		if (!$ba->read_uint32($this->ip)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		$ba->write_buf($this->email,64);
		$ba->write_buf($this->passwd_md5_two,16);
		if (!$ba->write_uint16($this->channel_id)) return false;
		if (!$ba->write_uint16($this->region)) return false;
		if (!$ba->write_uint16($this->gameid)) return false;
		if (!$ba->write_uint32($this->ip)) return false;
		return true;
	}

};

	
class main_login_login_out {
	/*  */
	#定长数组,长度:16, 类型:char 
	public $session ;

	/*  */
	#类型:uint32
	public $gameflag;


	public function main_login_login_out(){

	}

	public function read_from_buf($ba ){
		if(!$ba->read_buf($this->session,16)) return false;
		if (!$ba->read_uint32($this->gameflag)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		$ba->write_buf($this->session,16);
		if (!$ba->write_uint32($this->gameflag)) return false;
		return true;
	}

};

	
class main_login_with_verif_img_in {
	/*  */
	#定长数组,长度:64, 类型:char 
	public $email ;

	/*  */
	#定长数组,长度:16, 类型:char 
	public $passwd_md5_two ;

	/* 渠道id */
	#类型:uint16
	public $channel_id;

	/*  */
	#类型:uint16
	public $region;

	/*  */
	#类型:uint16
	public $gameid;

	/* 网络序 */
	#类型:uint32
	public $ip;

	/* 验证码session */
	#定长数组,长度:16, 类型:char 
	public $verif_session ;

	/* 验证码 */
	#定长数组,长度:6, 类型:char 
	public $verif_code ;

	/* 渠道码 */
	#类型:uint16
	public $channel_code;


	public function main_login_with_verif_img_in(){

	}

	public function read_from_buf($ba ){
		if(!$ba->read_buf($this->email,64)) return false;
		if(!$ba->read_buf($this->passwd_md5_two,16)) return false;
		if (!$ba->read_uint16($this->channel_id)) return false;
		if (!$ba->read_uint16($this->region)) return false;
		if (!$ba->read_uint16($this->gameid)) return false;
		if (!$ba->read_uint32($this->ip)) return false;
		if(!$ba->read_buf($this->verif_session,16)) return false;
		if(!$ba->read_buf($this->verif_code,6)) return false;
		if (!$ba->read_uint16($this->channel_code)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		$ba->write_buf($this->email,64);
		$ba->write_buf($this->passwd_md5_two,16);
		if (!$ba->write_uint16($this->channel_id)) return false;
		if (!$ba->write_uint16($this->region)) return false;
		if (!$ba->write_uint16($this->gameid)) return false;
		if (!$ba->write_uint32($this->ip)) return false;
		$ba->write_buf($this->verif_session,16);
		$ba->write_buf($this->verif_code,6);
		if (!$ba->write_uint16($this->channel_code)) return false;
		return true;
	}

};

	
class pay_become_vip_in {
	/* 渠道id */
	#类型:uint16
	public $channel_id;

	/* 安全码 */
	#定长数组,长度:32, 类型:char 
	public $verify_code ;

	/* 0：非自动续费 1：自动续费 */
	#类型:uint8
	public $is_auto_charge;

	/* 0：不使用优惠券 1：使用优惠码 2：使用抵用券或折扣券 */
	#类型:uint8
	public $use_coupon;

	/* 9字节优惠码，当use_coupon=1时有效 */
	#定长数组,长度:9, 类型:char 
	public $coupon ;

	/* 抵用券或折扣券类型，当use_coupon=2时有效 （11：摩尔2元 12：摩尔5元 13：摩尔20元 61：赛尔二7折 62：赛尔二8折 63：赛尔二9折） */
	#类型:uint16
	public $voucher;

	/* 0：电信 1：网通 */
	#类型:uint8
	public $which_line;

	/* 扩展字段，暂时用不到，填NULL */
	#定长数组,长度:19, 类型:char 
	public $extend ;


	public function pay_become_vip_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint16($this->channel_id)) return false;
		if(!$ba->read_buf($this->verify_code,32)) return false;
		if (!$ba->read_uint8($this->is_auto_charge)) return false;
		if (!$ba->read_uint8($this->use_coupon)) return false;
		if(!$ba->read_buf($this->coupon,9)) return false;
		if (!$ba->read_uint16($this->voucher)) return false;
		if (!$ba->read_uint8($this->which_line)) return false;
		if(!$ba->read_buf($this->extend,19)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint16($this->channel_id)) return false;
		$ba->write_buf($this->verify_code,32);
		if (!$ba->write_uint8($this->is_auto_charge)) return false;
		if (!$ba->write_uint8($this->use_coupon)) return false;
		$ba->write_buf($this->coupon,9);
		if (!$ba->write_uint16($this->voucher)) return false;
		if (!$ba->write_uint8($this->which_line)) return false;
		$ba->write_buf($this->extend,19);
		return true;
	}

};

	
class pay_become_vip_out {
	/* 扣取米币数（100倍；比如用户剩余1米币，该字段返回值为100） */
	#类型:uint32
	public $mb_consumed;

	/* 米币账户余额（100倍；比如用户剩余1米币，该字段返回值为100） */
	#类型:uint32
	public $mb_balance;


	public function pay_become_vip_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->mb_consumed)) return false;
		if (!$ba->read_uint32($this->mb_balance)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->mb_consumed)) return false;
		if (!$ba->write_uint32($this->mb_balance)) return false;
		return true;
	}

};

	
class pay_get_become_vip_price_out {
	/* 开通一个月VIP花费米币数（100倍） */
	#类型:uint32
	public $month1_price;

	/* 开通三个月VIP花费米币数（100倍） */
	#类型:uint32
	public $month3_price;

	/* 开通六个月VIP花费米币数（100倍） */
	#类型:uint32
	public $month6_price;

	/* 开通十二个月VIP花费米币数（100倍） */
	#类型:uint32
	public $month12_price;


	public function pay_get_become_vip_price_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->month1_price)) return false;
		if (!$ba->read_uint32($this->month3_price)) return false;
		if (!$ba->read_uint32($this->month6_price)) return false;
		if (!$ba->read_uint32($this->month12_price)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->month1_price)) return false;
		if (!$ba->write_uint32($this->month3_price)) return false;
		if (!$ba->write_uint32($this->month6_price)) return false;
		if (!$ba->write_uint32($this->month12_price)) return false;
		return true;
	}

};

	
class pay_get_mb_left_out {
	/* 米币账户余额（100倍；比如用户剩余1米币，该字段返回值为100） */
	#类型:uint32
	public $mb_balance;


	public function pay_get_mb_left_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->mb_balance)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->mb_balance)) return false;
		return true;
	}

};

	
class userinfo_check_pay_password_in {
	/* 支付密码 */
	#定长数组,长度:16, 类型:uint8_t 
	public $password =array();


	public function userinfo_check_pay_password_in(){

	}

	public function read_from_buf($ba ){
		$this->password=array();
		{for($i=0;$i<16;$i++){
			if (!$ba->read_uint8($this->password[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if(count($this->password)!=16) return false; 
		{for($i=0; $i<16;$i++){
			if (!$ba->write_uint8($this->password[$i])) return false;
		}}
		return true;
	}

};

	
class userinfo_get_gameflag_in {
	/* 忽略字段，添0 */
	#类型:uint32
	public $region;


	public function userinfo_get_gameflag_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->region)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->region)) return false;
		return true;
	}

};

	
class userinfo_get_gameflag_out {
	/* 0非米饭，1米饭 */
	#类型:uint32
	public $mee_fans;

	/* 电信激活flag */
	#类型:uint32
	public $game_act_flag;

	/* 电信gameid或运算结果 */
	#类型:uint32
	public $dx_gameflag;

	/* 网通gameid或运算结果 */
	#类型:uint32
	public $wt_gameflag;


	public function userinfo_get_gameflag_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->mee_fans)) return false;
		if (!$ba->read_uint32($this->game_act_flag)) return false;
		if (!$ba->read_uint32($this->dx_gameflag)) return false;
		if (!$ba->read_uint32($this->wt_gameflag)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->mee_fans)) return false;
		if (!$ba->write_uint32($this->game_act_flag)) return false;
		if (!$ba->write_uint32($this->dx_gameflag)) return false;
		if (!$ba->write_uint32($this->wt_gameflag)) return false;
		return true;
	}

};

	
class userinfo_isset_pay_password_out {
	/* 0没有设置，非0设置过 */
	#类型:uint32
	public $is_seted;


	public function userinfo_isset_pay_password_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->is_seted)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->is_seted)) return false;
		return true;
	}

};

	
class userinfo_set_pay_password_in {
	/* 支付密码 */
	#定长数组,长度:16, 类型:uint8_t 
	public $password =array();


	public function userinfo_set_pay_password_in(){

	}

	public function read_from_buf($ba ){
		$this->password=array();
		{for($i=0;$i<16;$i++){
			if (!$ba->read_uint8($this->password[$i])) return false;
		}}
		return true;
	}

	public function write_to_buf($ba ){
		if(count($this->password)!=16) return false; 
		{for($i=0; $i<16;$i++){
			if (!$ba->write_uint8($this->password[$i])) return false;
		}}
		return true;
	}

};

	
class vip_add_days_in {
	/*  */
	#类型:uint32
	public $days;

	/*  */
	#类型:uint32
	public $add_flag;

	/*  */
	#类型:uint32
	public $channel_id;

	/*  */
	#定长数组,长度:20, 类型:char 
	public $trade_id ;


	public function vip_add_days_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->days)) return false;
		if (!$ba->read_uint32($this->add_flag)) return false;
		if (!$ba->read_uint32($this->channel_id)) return false;
		if(!$ba->read_buf($this->trade_id,20)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->days)) return false;
		if (!$ba->write_uint32($this->add_flag)) return false;
		if (!$ba->write_uint32($this->channel_id)) return false;
		$ba->write_buf($this->trade_id,20);
		return true;
	}

};

	
class vip_add_days_out {
	/*  */
	#类型:uint32
	public $op_id;

	/*  */
	#类型:uint32
	public $vip_ret;


	public function vip_add_days_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->op_id)) return false;
		if (!$ba->read_uint32($this->vip_ret)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->op_id)) return false;
		if (!$ba->write_uint32($this->vip_ret)) return false;
		return true;
	}

};

	
class vip_get_info_out {
	/*  */
	#类型:uint32
	public $vip_result;

	/* 0：非自动续费 1：自动续费 */
	#类型:uint32
	public $vip_is_auto_charge;

	/*  */
	#类型:uint32
	public $vip_begin_time;

	/*  */
	#类型:uint32
	public $vip_end_time;

	/* 渠道id */
	#类型:uint32
	public $vip_channel_id;


	public function vip_get_info_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->vip_result)) return false;
		if (!$ba->read_uint32($this->vip_is_auto_charge)) return false;
		if (!$ba->read_uint32($this->vip_begin_time)) return false;
		if (!$ba->read_uint32($this->vip_end_time)) return false;
		if (!$ba->read_uint32($this->vip_channel_id)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->vip_result)) return false;
		if (!$ba->write_uint32($this->vip_is_auto_charge)) return false;
		if (!$ba->write_uint32($this->vip_begin_time)) return false;
		if (!$ba->write_uint32($this->vip_end_time)) return false;
		if (!$ba->write_uint32($this->vip_channel_id)) return false;
		return true;
	}

};

	
class vip_sub_days_in {
	/*  */
	#类型:uint32
	public $days;

	/*  */
	#类型:uint32
	public $channel_id;

	/*  */
	#定长数组,长度:20, 类型:char 
	public $trade_id ;


	public function vip_sub_days_in(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->days)) return false;
		if (!$ba->read_uint32($this->channel_id)) return false;
		if(!$ba->read_buf($this->trade_id,20)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->days)) return false;
		if (!$ba->write_uint32($this->channel_id)) return false;
		$ba->write_buf($this->trade_id,20);
		return true;
	}

};

	
class vip_sub_days_out {
	/*  */
	#类型:uint32
	public $op_id;

	/*  */
	#类型:uint32
	public $vip_ret;

	/*  */
	#类型:uint32
	public $vip_endtime;


	public function vip_sub_days_out(){

	}

	public function read_from_buf($ba ){
		if (!$ba->read_uint32($this->op_id)) return false;
		if (!$ba->read_uint32($this->vip_ret)) return false;
		if (!$ba->read_uint32($this->vip_endtime)) return false;
		return true;
	}

	public function write_to_buf($ba ){
		if (!$ba->write_uint32($this->op_id)) return false;
		if (!$ba->write_uint32($this->vip_ret)) return false;
		if (!$ba->write_uint32($this->vip_endtime)) return false;
		return true;
	}

};

	
class Cmain_login_proto  extends Cproto_base {
    function __construct( $proxyip,$proxyport){
        parent::__construct($proxyip,$proxyport) ;
    }

	/*  */
	/* 调用方式还可以是： 
		$in=new userinfo_check_pay_password_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->userinfo_check_pay_password($userid,$in );
	*/

	function userinfo_check_pay_password($userid , $password=null){

		if ( $password instanceof userinfo_check_pay_password_in ){
			$in=$password;
		}else{
			$in=new userinfo_check_pay_password_in();
			$in->password=$password;

		}
		
		return $this->send_cmd_new(0x008B,$userid, $in, null, 0xa96a11a6);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new userinfo_set_pay_password_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->userinfo_set_pay_password($userid,$in );
	*/

	function userinfo_set_pay_password($userid , $password=null){

		if ( $password instanceof userinfo_set_pay_password_in ){
			$in=$password;
		}else{
			$in=new userinfo_set_pay_password_in();
			$in->password=$password;

		}
		
		return $this->send_cmd_new(0x008C,$userid, $in, null, 0x3a111adc);
	}
	
	/* 用户是否设置过支付密码 */

	function userinfo_isset_pay_password($userid ){

		return $this->send_cmd_new(0x008D,$userid, null, new userinfo_isset_pay_password_out(), 0xcc3fa251);
	}
	
	/* 检查用户的游戏激活标识 */
	/* 调用方式还可以是： 
		$in=new userinfo_get_gameflag_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->userinfo_get_gameflag($userid,$in );
	*/

	function userinfo_get_gameflag($userid , $region=null){

		if ( $region instanceof userinfo_get_gameflag_in ){
			$in=$region;
		}else{
			$in=new userinfo_get_gameflag_in();
			$in->region=$region;

		}
		
		return $this->send_cmd_new(0x009B,$userid, $in, new userinfo_get_gameflag_out(), 0xaa54b631);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new vip_add_days_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->vip_add_days($userid,$in );
	*/

	function vip_add_days($userid , $days=null, $add_flag=null, $channel_id=null, $trade_id=null){

		if ( $days instanceof vip_add_days_in ){
			$in=$days;
		}else{
			$in=new vip_add_days_in();
			$in->days=$days;
			$in->add_flag=$add_flag;
			$in->channel_id=$channel_id;
			$in->trade_id=$trade_id;

		}
		
		return $this->send_cmd_new(0x2001,$userid, $in, new vip_add_days_out(), 0x78b82792);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new vip_sub_days_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->vip_sub_days($userid,$in );
	*/

	function vip_sub_days($userid , $days=null, $channel_id=null, $trade_id=null){

		if ( $days instanceof vip_sub_days_in ){
			$in=$days;
		}else{
			$in=new vip_sub_days_in();
			$in->days=$days;
			$in->channel_id=$channel_id;
			$in->trade_id=$trade_id;

		}
		
		return $this->send_cmd_new(0x2002,$userid, $in, new vip_sub_days_out(), 0xbf0c5334);
	}
	
	/*  */

	function vip_get_info($userid ){

		return $this->send_cmd_new(0x2004,$userid, null, new vip_get_info_out(), 0x71ac0042);
	}
	
	/* 查询米币余额 */

	function pay_get_mb_left($userid ){

		return $this->send_cmd_new(0x7601,$userid, null, new pay_get_mb_left_out(), 0xf2c4efe9);
	}
	
	/* 游戏内用米币直接开通VIP */
	/* 调用方式还可以是： 
		$in=new pay_become_vip_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->pay_become_vip($userid,$in );
	*/

	function pay_become_vip($userid , $channel_id=null, $verify_code=null, $is_auto_charge=null, $use_coupon=null, $coupon=null, $voucher=null, $which_line=null, $extend=null){

		if ( $channel_id instanceof pay_become_vip_in ){
			$in=$channel_id;
		}else{
			$in=new pay_become_vip_in();
			$in->channel_id=$channel_id;
			$in->verify_code=$verify_code;
			$in->is_auto_charge=$is_auto_charge;
			$in->use_coupon=$use_coupon;
			$in->coupon=$coupon;
			$in->voucher=$voucher;
			$in->which_line=$which_line;
			$in->extend=$extend;

		}
		
		return $this->send_cmd_new(0x7602,$userid, $in, new pay_become_vip_out(), 0xd8165379);
	}
	
	/* 获取VIP开通价格 */

	function pay_get_become_vip_price($userid ){

		return $this->send_cmd_new(0x7603,$userid, null, new pay_get_become_vip_price_out(), 0x02701dc7);
	}
	
	/* 主登录 */
	/* 调用方式还可以是： 
		$in=new main_login_login_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->main_login_login($userid,$in );
	*/

	function main_login_login($userid , $email=null, $passwd_md5_two=null, $channel_id=null, $region=null, $gameid=null, $ip=null){

		if ( $email instanceof main_login_login_in ){
			$in=$email;
		}else{
			$in=new main_login_login_in();
			$in->email=$email;
			$in->passwd_md5_two=$passwd_md5_two;
			$in->channel_id=$channel_id;
			$in->region=$region;
			$in->gameid=$gameid;
			$in->ip=$ip;

		}
		
		return $this->send_cmd_new(0xA021,$userid, $in, new main_login_login_out(), 0xa59348f6);
	}
	
	/* 检查session */
	/* 调用方式还可以是： 
		$in=new main_login_check_session_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->main_login_check_session($userid,$in );
	*/

	function main_login_check_session($userid , $gameid=null, $session=null, $del_session_flag=null){

		if ( $gameid instanceof main_login_check_session_in ){
			$in=$gameid;
		}else{
			$in=new main_login_check_session_in();
			$in->gameid=$gameid;
			$in->session=$session;
			$in->del_session_flag=$del_session_flag;

		}
		
		return $this->send_cmd_new(0xA024,$userid, $in, null, 0x372301cc);
	}
	
	/*  */
	/* 调用方式还可以是： 
		$in=new main_login_with_verif_img_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->main_login_with_verif_img($userid,$in );
	*/

	function main_login_with_verif_img($userid , $email=null, $passwd_md5_two=null, $channel_id=null, $region=null, $gameid=null, $ip=null, $verif_session=null, $verif_code=null, $channel_code=null){

		if ( $email instanceof main_login_with_verif_img_in ){
			$in=$email;
		}else{
			$in=new main_login_with_verif_img_in();
			$in->email=$email;
			$in->passwd_md5_two=$passwd_md5_two;
			$in->channel_id=$channel_id;
			$in->region=$region;
			$in->gameid=$gameid;
			$in->ip=$ip;
			$in->verif_session=$verif_session;
			$in->verif_code=$verif_code;
			$in->channel_code=$channel_code;

		}
		
		return $this->send_cmd_new(0xA032,$userid, $in, null, 0x72adf6bd);
	}
	
	/* 验证session */
	/* 调用方式还可以是： 
		$in=new main_login_check_session_new_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->main_login_check_session_new($userid,$in );
	*/

	function main_login_check_session_new($userid , $from_game=null, $session=null, $del_session_flag=null, $to_game=null, $ip=null, $region=null, $enter_game=null, $tad=null){

		if ( $from_game instanceof main_login_check_session_new_in ){
			$in=$from_game;
		}else{
			$in=new main_login_check_session_new_in();
			$in->from_game=$from_game;
			$in->session=$session;
			$in->del_session_flag=$del_session_flag;
			$in->to_game=$to_game;
			$in->ip=$ip;
			$in->region=$region;
			$in->enter_game=$enter_game;
			$in->tad=$tad;

		}
		
		return $this->send_cmd_new(0xA03A,$userid, $in, null, 0xab75005c);
	}
	
	/* 加session */
	/* 调用方式还可以是： 
		$in=new main_login_add_session_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->main_login_add_session($userid,$in );
	*/

	function main_login_add_session($userid , $channel_id=null, $verify_code=null, $gameid=null, $ip=null){

		if ( $channel_id instanceof main_login_add_session_in ){
			$in=$channel_id;
		}else{
			$in=new main_login_add_session_in();
			$in->channel_id=$channel_id;
			$in->verify_code=$verify_code;
			$in->gameid=$gameid;
			$in->ip=$ip;

		}
		
		return $this->send_cmd_new(0xA122,$userid, $in, new main_login_add_session_out(), 0xc0d5347b);
	}
	
	/* 增加项目 */
	/* 调用方式还可以是： 
		$in=new main_login_add_game_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->main_login_add_game($userid,$in );
	*/

	function main_login_add_game($userid , $channel_id=null, $verify_code=null, $idczone=null, $gameid=null){

		if ( $channel_id instanceof main_login_add_game_in ){
			$in=$channel_id;
		}else{
			$in=new main_login_add_game_in();
			$in->channel_id=$channel_id;
			$in->verify_code=$verify_code;
			$in->idczone=$idczone;
			$in->gameid=$gameid;

		}
		
		return $this->send_cmd_new(0xA123,$userid, $in, null, 0xfa9feb85);
	}
	
	/* 增加游戏项目（带渠道号） */
	/* 调用方式还可以是： 
		$in=new main_login_add_game_with_chanel_in();
		$in.xx1="xxxxx1";
		$in.xx2="xxxxx2";
		....
		$in.xx3="xxxxx3";
		$proto->main_login_add_game_with_chanel($userid,$in );
	*/

	function main_login_add_game_with_chanel($userid , $channel_code=null, $verify_code=null, $idczone=null, $gameid=null, $channel_id=null){

		if ( $channel_code instanceof main_login_add_game_with_chanel_in ){
			$in=$channel_code;
		}else{
			$in=new main_login_add_game_with_chanel_in();
			$in->channel_code=$channel_code;
			$in->verify_code=$verify_code;
			$in->idczone=$idczone;
			$in->gameid=$gameid;
			$in->channel_id=$channel_id;

		}
		
		return $this->send_cmd_new(0xA134,$userid, $in, null, 0x581caa2d);
	}
	
};
?>
