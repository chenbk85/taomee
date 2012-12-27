# -*- coding: utf-8 -*-

hexchars="0123456789ABCDEF"
def dit2hex(ch):
	i=ord(ch) # get its integral value.
	leftnib=i>>4
	rightnib=i & 0xf
	leftchar=hexchars[leftnib]
	rightchar=hexchars[rightnib]
	return leftchar+rightchar+" "
	
def bin2hex(str):
	retvalue=""  # return value is empty until we add to it...
	for s in str:
		hexchars=dit2hex(s)
		retvalue=retvalue+hexchars
		pass
	return retvalue

		
class main_login_add_game_in :
	"""
参数列表:
	/* 渠道id */
	#类型:uint16
	public channel_id;

	/*  */
	#定长数组,长度:32, 类型:char 
	public verify_code ;

	/*  */
	#类型:uint32
	public idczone;

	/*  */
	#类型:uint32
	public gameid;


	"""

	def read_from_buf(self, ba ):
		self.channel_id=ba.read_uint16();
		if ( self.channel_id== None): return False;
		self.verify_code=ba.read_buf(32);
		if( self.verify_code==None ): return False;
		self.idczone=ba.read_uint32();
		if ( self.idczone== None): return False;
		self.gameid=ba.read_uint32();
		if ( self.gameid== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint16(self.channel_id)):return False;
		ba.write_buf(self.verify_code,32);
		if ( not ba.write_uint32(self.idczone)):return False;
		if ( not ba.write_uint32(self.gameid)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 渠道id */"""
		print tabs+"[channel_id]=%s"%(str(self.channel_id))
		print tabs+"""/*  */"""
		tmp_str=str(self.verify_code);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[verify_code]=%s"%(tmp_str)
		print tabs+"""/*  */"""
		print tabs+"[idczone]=%s"%(str(self.idczone))
		print tabs+"""/*  */"""
		print tabs+"[gameid]=%s"%(str(self.gameid))
		
		pass;
	




class main_login_add_game_with_chanel_in :
	"""
参数列表:
	/* 服务器验证渠道码 */
	#类型:uint16
	public channel_code;

	/*  */
	#定长数组,长度:32, 类型:char 
	public verify_code ;

	/*  */
	#类型:uint32
	public idczone;

	/*  */
	#类型:uint32
	public gameid;

	/* 渠道id */
	#类型:uint16
	public channel_id;


	"""

	def read_from_buf(self, ba ):
		self.channel_code=ba.read_uint16();
		if ( self.channel_code== None): return False;
		self.verify_code=ba.read_buf(32);
		if( self.verify_code==None ): return False;
		self.idczone=ba.read_uint32();
		if ( self.idczone== None): return False;
		self.gameid=ba.read_uint32();
		if ( self.gameid== None): return False;
		self.channel_id=ba.read_uint16();
		if ( self.channel_id== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint16(self.channel_code)):return False;
		ba.write_buf(self.verify_code,32);
		if ( not ba.write_uint32(self.idczone)):return False;
		if ( not ba.write_uint32(self.gameid)):return False;
		if ( not ba.write_uint16(self.channel_id)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 服务器验证渠道码 */"""
		print tabs+"[channel_code]=%s"%(str(self.channel_code))
		print tabs+"""/*  */"""
		tmp_str=str(self.verify_code);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[verify_code]=%s"%(tmp_str)
		print tabs+"""/*  */"""
		print tabs+"[idczone]=%s"%(str(self.idczone))
		print tabs+"""/*  */"""
		print tabs+"[gameid]=%s"%(str(self.gameid))
		print tabs+"""/* 渠道id */"""
		print tabs+"[channel_id]=%s"%(str(self.channel_id))
		
		pass;
	




class main_login_add_session_in :
	"""
参数列表:
	/* 渠道id */
	#类型:uint16
	public channel_id;

	/*  */
	#定长数组,长度:32, 类型:char 
	public verify_code ;

	/*  */
	#类型:uint32
	public gameid;

	/* 网络序 */
	#类型:uint32
	public ip;


	"""

	def read_from_buf(self, ba ):
		self.channel_id=ba.read_uint16();
		if ( self.channel_id== None): return False;
		self.verify_code=ba.read_buf(32);
		if( self.verify_code==None ): return False;
		self.gameid=ba.read_uint32();
		if ( self.gameid== None): return False;
		self.ip=ba.read_uint32();
		if ( self.ip== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint16(self.channel_id)):return False;
		ba.write_buf(self.verify_code,32);
		if ( not ba.write_uint32(self.gameid)):return False;
		if ( not ba.write_uint32(self.ip)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 渠道id */"""
		print tabs+"[channel_id]=%s"%(str(self.channel_id))
		print tabs+"""/*  */"""
		tmp_str=str(self.verify_code);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[verify_code]=%s"%(tmp_str)
		print tabs+"""/*  */"""
		print tabs+"[gameid]=%s"%(str(self.gameid))
		print tabs+"""/* 网络序 */"""
		print tabs+"[ip]=%s"%(str(self.ip))
		
		pass;
	




class main_login_add_session_out :
	"""
参数列表:
	/*  */
	#定长数组,长度:16, 类型:char 
	public session ;


	"""

	def read_from_buf(self, ba ):
		self.session=ba.read_buf(16);
		if( self.session==None ): return False;

		return True;


	def write_to_buf(self,ba):
		ba.write_buf(self.session,16);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		tmp_str=str(self.session);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[session]=%s"%(tmp_str)
		
		pass;
	




class main_login_check_session_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public gameid;

	/*  */
	#定长数组,长度:16, 类型:char 
	public session ;

	/*  */
	#类型:uint32
	public del_session_flag;


	"""

	def read_from_buf(self, ba ):
		self.gameid=ba.read_uint32();
		if ( self.gameid== None): return False;
		self.session=ba.read_buf(16);
		if( self.session==None ): return False;
		self.del_session_flag=ba.read_uint32();
		if ( self.del_session_flag== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.gameid)):return False;
		ba.write_buf(self.session,16);
		if ( not ba.write_uint32(self.del_session_flag)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[gameid]=%s"%(str(self.gameid))
		print tabs+"""/*  */"""
		tmp_str=str(self.session);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[session]=%s"%(tmp_str)
		print tabs+"""/*  */"""
		print tabs+"[del_session_flag]=%s"%(str(self.del_session_flag))
		
		pass;
	




class main_login_check_session_new_in :
	"""
参数列表:
	/* 代签的起始产品，从from_game代签到to_game */
	#类型:uint32
	public from_game;

	/* session内容 */
	#定长数组,长度:16, 类型:char 
	public session ;

	/* 删除此session标示,0验证通过后session仍然有效，1验证通过后session失效 */
	#类型:uint32
	public del_session_flag;

	/* 代签的目标产品，从from_game代签到to_game，用于统计代签登录的人数 */
	#类型:uint32
	public to_game;

	/* 客户端ip，网络字节序 */
	#类型:uint32
	public ip;

	/* 游戏分区信息：0=电信，1=网通 */
	#类型:uint16
	public region;

	/* 是否进入游戏，1表示进入online服务器 */
	#类型:uint8
	public enter_game;

	/* 登录渠道推广广告位，用于统计广告位带来的登录统计，如果没有推广全部填’\0’,如:media.4399.top.left等， 该字段目前为变长字符串，最长128个字节，以’\0’作为字符串结尾。 */
	#定长数组,长度:128, 类型:uint8_t 
	public tad =array();


	"""

	def read_from_buf(self, ba ):
		self.from_game=ba.read_uint32();
		if ( self.from_game== None): return False;
		self.session=ba.read_buf(16);
		if( self.session==None ): return False;
		self.del_session_flag=ba.read_uint32();
		if ( self.del_session_flag== None): return False;
		self.to_game=ba.read_uint32();
		if ( self.to_game== None): return False;
		self.ip=ba.read_uint32();
		if ( self.ip== None): return False;
		self.region=ba.read_uint16();
		if ( self.region== None): return False;
		self.enter_game=ba.read_uint8();
		if ( self.enter_game== None): return False;
		self.tad=[];
		for i in range(128):
			self.tad.append(ba.read_uint8());
			if ( self.tad[i]== None): return False;


		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.from_game)):return False;
		ba.write_buf(self.session,16);
		if ( not ba.write_uint32(self.del_session_flag)):return False;
		if ( not ba.write_uint32(self.to_game)):return False;
		if ( not ba.write_uint32(self.ip)):return False;
		if ( not ba.write_uint16(self.region)):return False;
		if ( not ba.write_uint8(self.enter_game)):return False;
		if(len(self.tad)<>128):return False; 
		for i in range(128):
			if ( not ba.write_uint8(self.tad[i])):return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 代签的起始产品，从from_game代签到to_game */"""
		print tabs+"[from_game]=%s"%(str(self.from_game))
		print tabs+"""/* session内容 */"""
		tmp_str=str(self.session);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[session]=%s"%(tmp_str)
		print tabs+"""/* 删除此session标示,0验证通过后session仍然有效，1验证通过后session失效 */"""
		print tabs+"[del_session_flag]=%s"%(str(self.del_session_flag))
		print tabs+"""/* 代签的目标产品，从from_game代签到to_game，用于统计代签登录的人数 */"""
		print tabs+"[to_game]=%s"%(str(self.to_game))
		print tabs+"""/* 客户端ip，网络字节序 */"""
		print tabs+"[ip]=%s"%(str(self.ip))
		print tabs+"""/* 游戏分区信息：0=电信，1=网通 */"""
		print tabs+"[region]=%s"%(str(self.region))
		print tabs+"""/* 是否进入游戏，1表示进入online服务器 */"""
		print tabs+"[enter_game]=%s"%(str(self.enter_game))
		print tabs+"""#登录渠道推广广告位，用于统计广告位带来的登录统计，如果没有推广全部填’\0’,如:media.4399.top.left等， 该字段目前为变长字符串，最长128个字节，以’\0’作为字符串结尾。"""
		print tabs+"[tad]=>{";
		for i in range(128):
			print tabs+"    [%d]="%(i)+str(self.tad[i]); 

		print tabs+"}"
		
		pass;
	




class main_login_login_in :
	"""
参数列表:
	/*  */
	#定长数组,长度:64, 类型:char 
	public email ;

	/*  */
	#定长数组,长度:16, 类型:char 
	public passwd_md5_two ;

	/* 渠道id */
	#类型:uint16
	public channel_id;

	/*  */
	#类型:uint16
	public region;

	/*  */
	#类型:uint16
	public gameid;

	/* 网络序 */
	#类型:uint32
	public ip;


	"""

	def read_from_buf(self, ba ):
		self.email=ba.read_buf(64);
		if( self.email==None ): return False;
		self.passwd_md5_two=ba.read_buf(16);
		if( self.passwd_md5_two==None ): return False;
		self.channel_id=ba.read_uint16();
		if ( self.channel_id== None): return False;
		self.region=ba.read_uint16();
		if ( self.region== None): return False;
		self.gameid=ba.read_uint16();
		if ( self.gameid== None): return False;
		self.ip=ba.read_uint32();
		if ( self.ip== None): return False;

		return True;


	def write_to_buf(self,ba):
		ba.write_buf(self.email,64);
		ba.write_buf(self.passwd_md5_two,16);
		if ( not ba.write_uint16(self.channel_id)):return False;
		if ( not ba.write_uint16(self.region)):return False;
		if ( not ba.write_uint16(self.gameid)):return False;
		if ( not ba.write_uint32(self.ip)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		tmp_str=str(self.email);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[email]=%s"%(tmp_str)
		print tabs+"""/*  */"""
		tmp_str=str(self.passwd_md5_two);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[passwd_md5_two]=%s"%(tmp_str)
		print tabs+"""/* 渠道id */"""
		print tabs+"[channel_id]=%s"%(str(self.channel_id))
		print tabs+"""/*  */"""
		print tabs+"[region]=%s"%(str(self.region))
		print tabs+"""/*  */"""
		print tabs+"[gameid]=%s"%(str(self.gameid))
		print tabs+"""/* 网络序 */"""
		print tabs+"[ip]=%s"%(str(self.ip))
		
		pass;
	




class main_login_login_out :
	"""
参数列表:
	/*  */
	#定长数组,长度:16, 类型:char 
	public session ;

	/*  */
	#类型:uint32
	public gameflag;


	"""

	def read_from_buf(self, ba ):
		self.session=ba.read_buf(16);
		if( self.session==None ): return False;
		self.gameflag=ba.read_uint32();
		if ( self.gameflag== None): return False;

		return True;


	def write_to_buf(self,ba):
		ba.write_buf(self.session,16);
		if ( not ba.write_uint32(self.gameflag)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		tmp_str=str(self.session);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[session]=%s"%(tmp_str)
		print tabs+"""/*  */"""
		print tabs+"[gameflag]=%s"%(str(self.gameflag))
		
		pass;
	




class main_login_with_verif_img_in :
	"""
参数列表:
	/*  */
	#定长数组,长度:64, 类型:char 
	public email ;

	/*  */
	#定长数组,长度:16, 类型:char 
	public passwd_md5_two ;

	/* 渠道id */
	#类型:uint16
	public channel_id;

	/*  */
	#类型:uint16
	public region;

	/*  */
	#类型:uint16
	public gameid;

	/* 网络序 */
	#类型:uint32
	public ip;

	/* 验证码session */
	#定长数组,长度:16, 类型:char 
	public verif_session ;

	/* 验证码 */
	#定长数组,长度:6, 类型:char 
	public verif_code ;

	/* 渠道码 */
	#类型:uint16
	public channel_code;


	"""

	def read_from_buf(self, ba ):
		self.email=ba.read_buf(64);
		if( self.email==None ): return False;
		self.passwd_md5_two=ba.read_buf(16);
		if( self.passwd_md5_two==None ): return False;
		self.channel_id=ba.read_uint16();
		if ( self.channel_id== None): return False;
		self.region=ba.read_uint16();
		if ( self.region== None): return False;
		self.gameid=ba.read_uint16();
		if ( self.gameid== None): return False;
		self.ip=ba.read_uint32();
		if ( self.ip== None): return False;
		self.verif_session=ba.read_buf(16);
		if( self.verif_session==None ): return False;
		self.verif_code=ba.read_buf(6);
		if( self.verif_code==None ): return False;
		self.channel_code=ba.read_uint16();
		if ( self.channel_code== None): return False;

		return True;


	def write_to_buf(self,ba):
		ba.write_buf(self.email,64);
		ba.write_buf(self.passwd_md5_two,16);
		if ( not ba.write_uint16(self.channel_id)):return False;
		if ( not ba.write_uint16(self.region)):return False;
		if ( not ba.write_uint16(self.gameid)):return False;
		if ( not ba.write_uint32(self.ip)):return False;
		ba.write_buf(self.verif_session,16);
		ba.write_buf(self.verif_code,6);
		if ( not ba.write_uint16(self.channel_code)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		tmp_str=str(self.email);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[email]=%s"%(tmp_str)
		print tabs+"""/*  */"""
		tmp_str=str(self.passwd_md5_two);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[passwd_md5_two]=%s"%(tmp_str)
		print tabs+"""/* 渠道id */"""
		print tabs+"[channel_id]=%s"%(str(self.channel_id))
		print tabs+"""/*  */"""
		print tabs+"[region]=%s"%(str(self.region))
		print tabs+"""/*  */"""
		print tabs+"[gameid]=%s"%(str(self.gameid))
		print tabs+"""/* 网络序 */"""
		print tabs+"[ip]=%s"%(str(self.ip))
		print tabs+"""/* 验证码session */"""
		tmp_str=str(self.verif_session);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[verif_session]=%s"%(tmp_str)
		print tabs+"""/* 验证码 */"""
		tmp_str=str(self.verif_code);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[verif_code]=%s"%(tmp_str)
		print tabs+"""/* 渠道码 */"""
		print tabs+"[channel_code]=%s"%(str(self.channel_code))
		
		pass;
	




class pay_become_vip_in :
	"""
参数列表:
	/* 渠道id */
	#类型:uint16
	public channel_id;

	/* 安全码 */
	#定长数组,长度:32, 类型:char 
	public verify_code ;

	/* 0：非自动续费 1：自动续费 */
	#类型:uint8
	public is_auto_charge;

	/* 0：不使用优惠券 1：使用优惠码 2：使用抵用券或折扣券 */
	#类型:uint8
	public use_coupon;

	/* 9字节优惠码，当use_coupon=1时有效 */
	#定长数组,长度:9, 类型:char 
	public coupon ;

	/* 抵用券或折扣券类型，当use_coupon=2时有效 （11：摩尔2元 12：摩尔5元 13：摩尔20元 61：赛尔二7折 62：赛尔二8折 63：赛尔二9折） */
	#类型:uint16
	public voucher;

	/* 0：电信 1：网通 */
	#类型:uint8
	public which_line;

	/* 扩展字段，暂时用不到，填NULL */
	#定长数组,长度:19, 类型:char 
	public extend ;


	"""

	def read_from_buf(self, ba ):
		self.channel_id=ba.read_uint16();
		if ( self.channel_id== None): return False;
		self.verify_code=ba.read_buf(32);
		if( self.verify_code==None ): return False;
		self.is_auto_charge=ba.read_uint8();
		if ( self.is_auto_charge== None): return False;
		self.use_coupon=ba.read_uint8();
		if ( self.use_coupon== None): return False;
		self.coupon=ba.read_buf(9);
		if( self.coupon==None ): return False;
		self.voucher=ba.read_uint16();
		if ( self.voucher== None): return False;
		self.which_line=ba.read_uint8();
		if ( self.which_line== None): return False;
		self.extend=ba.read_buf(19);
		if( self.extend==None ): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint16(self.channel_id)):return False;
		ba.write_buf(self.verify_code,32);
		if ( not ba.write_uint8(self.is_auto_charge)):return False;
		if ( not ba.write_uint8(self.use_coupon)):return False;
		ba.write_buf(self.coupon,9);
		if ( not ba.write_uint16(self.voucher)):return False;
		if ( not ba.write_uint8(self.which_line)):return False;
		ba.write_buf(self.extend,19);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 渠道id */"""
		print tabs+"[channel_id]=%s"%(str(self.channel_id))
		print tabs+"""/* 安全码 */"""
		tmp_str=str(self.verify_code);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[verify_code]=%s"%(tmp_str)
		print tabs+"""/* 0：非自动续费 1：自动续费 */"""
		print tabs+"[is_auto_charge]=%s"%(str(self.is_auto_charge))
		print tabs+"""/* 0：不使用优惠券 1：使用优惠码 2：使用抵用券或折扣券 */"""
		print tabs+"[use_coupon]=%s"%(str(self.use_coupon))
		print tabs+"""/* 9字节优惠码，当use_coupon=1时有效 */"""
		tmp_str=str(self.coupon);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[coupon]=%s"%(tmp_str)
		print tabs+"""/* 抵用券或折扣券类型，当use_coupon=2时有效 （11：摩尔2元 12：摩尔5元 13：摩尔20元 61：赛尔二7折 62：赛尔二8折 63：赛尔二9折） */"""
		print tabs+"[voucher]=%s"%(str(self.voucher))
		print tabs+"""/* 0：电信 1：网通 */"""
		print tabs+"[which_line]=%s"%(str(self.which_line))
		print tabs+"""/* 扩展字段，暂时用不到，填NULL */"""
		tmp_str=str(self.extend);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[extend]=%s"%(tmp_str)
		
		pass;
	




class pay_become_vip_out :
	"""
参数列表:
	/* 扣取米币数（100倍；比如用户剩余1米币，该字段返回值为100） */
	#类型:uint32
	public mb_consumed;

	/* 米币账户余额（100倍；比如用户剩余1米币，该字段返回值为100） */
	#类型:uint32
	public mb_balance;


	"""

	def read_from_buf(self, ba ):
		self.mb_consumed=ba.read_uint32();
		if ( self.mb_consumed== None): return False;
		self.mb_balance=ba.read_uint32();
		if ( self.mb_balance== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.mb_consumed)):return False;
		if ( not ba.write_uint32(self.mb_balance)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 扣取米币数（100倍；比如用户剩余1米币，该字段返回值为100） */"""
		print tabs+"[mb_consumed]=%s"%(str(self.mb_consumed))
		print tabs+"""/* 米币账户余额（100倍；比如用户剩余1米币，该字段返回值为100） */"""
		print tabs+"[mb_balance]=%s"%(str(self.mb_balance))
		
		pass;
	




class pay_get_become_vip_price_out :
	"""
参数列表:
	/* 开通一个月VIP花费米币数（100倍） */
	#类型:uint32
	public month1_price;

	/* 开通三个月VIP花费米币数（100倍） */
	#类型:uint32
	public month3_price;

	/* 开通六个月VIP花费米币数（100倍） */
	#类型:uint32
	public month6_price;

	/* 开通十二个月VIP花费米币数（100倍） */
	#类型:uint32
	public month12_price;


	"""

	def read_from_buf(self, ba ):
		self.month1_price=ba.read_uint32();
		if ( self.month1_price== None): return False;
		self.month3_price=ba.read_uint32();
		if ( self.month3_price== None): return False;
		self.month6_price=ba.read_uint32();
		if ( self.month6_price== None): return False;
		self.month12_price=ba.read_uint32();
		if ( self.month12_price== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.month1_price)):return False;
		if ( not ba.write_uint32(self.month3_price)):return False;
		if ( not ba.write_uint32(self.month6_price)):return False;
		if ( not ba.write_uint32(self.month12_price)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 开通一个月VIP花费米币数（100倍） */"""
		print tabs+"[month1_price]=%s"%(str(self.month1_price))
		print tabs+"""/* 开通三个月VIP花费米币数（100倍） */"""
		print tabs+"[month3_price]=%s"%(str(self.month3_price))
		print tabs+"""/* 开通六个月VIP花费米币数（100倍） */"""
		print tabs+"[month6_price]=%s"%(str(self.month6_price))
		print tabs+"""/* 开通十二个月VIP花费米币数（100倍） */"""
		print tabs+"[month12_price]=%s"%(str(self.month12_price))
		
		pass;
	




class pay_get_mb_left_out :
	"""
参数列表:
	/* 米币账户余额（100倍；比如用户剩余1米币，该字段返回值为100） */
	#类型:uint32
	public mb_balance;


	"""

	def read_from_buf(self, ba ):
		self.mb_balance=ba.read_uint32();
		if ( self.mb_balance== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.mb_balance)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 米币账户余额（100倍；比如用户剩余1米币，该字段返回值为100） */"""
		print tabs+"[mb_balance]=%s"%(str(self.mb_balance))
		
		pass;
	




class userinfo_check_pay_password_in :
	"""
参数列表:
	/* 支付密码 */
	#定长数组,长度:16, 类型:uint8_t 
	public password =array();


	"""

	def read_from_buf(self, ba ):
		self.password=[];
		for i in range(16):
			self.password.append(ba.read_uint8());
			if ( self.password[i]== None): return False;


		return True;


	def write_to_buf(self,ba):
		if(len(self.password)<>16):return False; 
		for i in range(16):
			if ( not ba.write_uint8(self.password[i])):return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""#支付密码"""
		print tabs+"[password]=>{";
		for i in range(16):
			print tabs+"    [%d]="%(i)+str(self.password[i]); 

		print tabs+"}"
		
		pass;
	




class userinfo_get_gameflag_in :
	"""
参数列表:
	/* 忽略字段，添0 */
	#类型:uint32
	public region;


	"""

	def read_from_buf(self, ba ):
		self.region=ba.read_uint32();
		if ( self.region== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.region)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 忽略字段，添0 */"""
		print tabs+"[region]=%s"%(str(self.region))
		
		pass;
	




class userinfo_get_gameflag_out :
	"""
参数列表:
	/* 0非米饭，1米饭 */
	#类型:uint32
	public mee_fans;

	/* 电信激活flag */
	#类型:uint32
	public game_act_flag;

	/* 电信gameid或运算结果 */
	#类型:uint32
	public dx_gameflag;

	/* 网通gameid或运算结果 */
	#类型:uint32
	public wt_gameflag;


	"""

	def read_from_buf(self, ba ):
		self.mee_fans=ba.read_uint32();
		if ( self.mee_fans== None): return False;
		self.game_act_flag=ba.read_uint32();
		if ( self.game_act_flag== None): return False;
		self.dx_gameflag=ba.read_uint32();
		if ( self.dx_gameflag== None): return False;
		self.wt_gameflag=ba.read_uint32();
		if ( self.wt_gameflag== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.mee_fans)):return False;
		if ( not ba.write_uint32(self.game_act_flag)):return False;
		if ( not ba.write_uint32(self.dx_gameflag)):return False;
		if ( not ba.write_uint32(self.wt_gameflag)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 0非米饭，1米饭 */"""
		print tabs+"[mee_fans]=%s"%(str(self.mee_fans))
		print tabs+"""/* 电信激活flag */"""
		print tabs+"[game_act_flag]=%s"%(str(self.game_act_flag))
		print tabs+"""/* 电信gameid或运算结果 */"""
		print tabs+"[dx_gameflag]=%s"%(str(self.dx_gameflag))
		print tabs+"""/* 网通gameid或运算结果 */"""
		print tabs+"[wt_gameflag]=%s"%(str(self.wt_gameflag))
		
		pass;
	




class userinfo_isset_pay_password_out :
	"""
参数列表:
	/* 0没有设置，非0设置过 */
	#类型:uint32
	public is_seted;


	"""

	def read_from_buf(self, ba ):
		self.is_seted=ba.read_uint32();
		if ( self.is_seted== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.is_seted)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/* 0没有设置，非0设置过 */"""
		print tabs+"[is_seted]=%s"%(str(self.is_seted))
		
		pass;
	




class userinfo_set_pay_password_in :
	"""
参数列表:
	/* 支付密码 */
	#定长数组,长度:16, 类型:uint8_t 
	public password =array();


	"""

	def read_from_buf(self, ba ):
		self.password=[];
		for i in range(16):
			self.password.append(ba.read_uint8());
			if ( self.password[i]== None): return False;


		return True;


	def write_to_buf(self,ba):
		if(len(self.password)<>16):return False; 
		for i in range(16):
			if ( not ba.write_uint8(self.password[i])):return False;

		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""#支付密码"""
		print tabs+"[password]=>{";
		for i in range(16):
			print tabs+"    [%d]="%(i)+str(self.password[i]); 

		print tabs+"}"
		
		pass;
	




class vip_add_days_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public days;

	/*  */
	#类型:uint32
	public add_flag;

	/*  */
	#类型:uint32
	public channel_id;

	/*  */
	#定长数组,长度:20, 类型:char 
	public trade_id ;


	"""

	def read_from_buf(self, ba ):
		self.days=ba.read_uint32();
		if ( self.days== None): return False;
		self.add_flag=ba.read_uint32();
		if ( self.add_flag== None): return False;
		self.channel_id=ba.read_uint32();
		if ( self.channel_id== None): return False;
		self.trade_id=ba.read_buf(20);
		if( self.trade_id==None ): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.days)):return False;
		if ( not ba.write_uint32(self.add_flag)):return False;
		if ( not ba.write_uint32(self.channel_id)):return False;
		ba.write_buf(self.trade_id,20);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[days]=%s"%(str(self.days))
		print tabs+"""/*  */"""
		print tabs+"[add_flag]=%s"%(str(self.add_flag))
		print tabs+"""/*  */"""
		print tabs+"[channel_id]=%s"%(str(self.channel_id))
		print tabs+"""/*  */"""
		tmp_str=str(self.trade_id);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[trade_id]=%s"%(tmp_str)
		
		pass;
	




class vip_add_days_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public op_id;

	/*  */
	#类型:uint32
	public vip_ret;


	"""

	def read_from_buf(self, ba ):
		self.op_id=ba.read_uint32();
		if ( self.op_id== None): return False;
		self.vip_ret=ba.read_uint32();
		if ( self.vip_ret== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.op_id)):return False;
		if ( not ba.write_uint32(self.vip_ret)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[op_id]=%s"%(str(self.op_id))
		print tabs+"""/*  */"""
		print tabs+"[vip_ret]=%s"%(str(self.vip_ret))
		
		pass;
	




class vip_get_info_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public vip_result;

	/* 0：非自动续费 1：自动续费 */
	#类型:uint32
	public vip_is_auto_charge;

	/*  */
	#类型:uint32
	public vip_begin_time;

	/*  */
	#类型:uint32
	public vip_end_time;

	/* 渠道id */
	#类型:uint32
	public vip_channel_id;


	"""

	def read_from_buf(self, ba ):
		self.vip_result=ba.read_uint32();
		if ( self.vip_result== None): return False;
		self.vip_is_auto_charge=ba.read_uint32();
		if ( self.vip_is_auto_charge== None): return False;
		self.vip_begin_time=ba.read_uint32();
		if ( self.vip_begin_time== None): return False;
		self.vip_end_time=ba.read_uint32();
		if ( self.vip_end_time== None): return False;
		self.vip_channel_id=ba.read_uint32();
		if ( self.vip_channel_id== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.vip_result)):return False;
		if ( not ba.write_uint32(self.vip_is_auto_charge)):return False;
		if ( not ba.write_uint32(self.vip_begin_time)):return False;
		if ( not ba.write_uint32(self.vip_end_time)):return False;
		if ( not ba.write_uint32(self.vip_channel_id)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[vip_result]=%s"%(str(self.vip_result))
		print tabs+"""/* 0：非自动续费 1：自动续费 */"""
		print tabs+"[vip_is_auto_charge]=%s"%(str(self.vip_is_auto_charge))
		print tabs+"""/*  */"""
		print tabs+"[vip_begin_time]=%s"%(str(self.vip_begin_time))
		print tabs+"""/*  */"""
		print tabs+"[vip_end_time]=%s"%(str(self.vip_end_time))
		print tabs+"""/* 渠道id */"""
		print tabs+"[vip_channel_id]=%s"%(str(self.vip_channel_id))
		
		pass;
	




class vip_sub_days_in :
	"""
参数列表:
	/*  */
	#类型:uint32
	public days;

	/*  */
	#类型:uint32
	public channel_id;

	/*  */
	#定长数组,长度:20, 类型:char 
	public trade_id ;


	"""

	def read_from_buf(self, ba ):
		self.days=ba.read_uint32();
		if ( self.days== None): return False;
		self.channel_id=ba.read_uint32();
		if ( self.channel_id== None): return False;
		self.trade_id=ba.read_buf(20);
		if( self.trade_id==None ): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.days)):return False;
		if ( not ba.write_uint32(self.channel_id)):return False;
		ba.write_buf(self.trade_id,20);
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[days]=%s"%(str(self.days))
		print tabs+"""/*  */"""
		print tabs+"[channel_id]=%s"%(str(self.channel_id))
		print tabs+"""/*  */"""
		tmp_str=str(self.trade_id);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[trade_id]=%s"%(tmp_str)
		
		pass;
	




class vip_sub_days_out :
	"""
参数列表:
	/*  */
	#类型:uint32
	public op_id;

	/*  */
	#类型:uint32
	public vip_ret;

	/*  */
	#类型:uint32
	public vip_endtime;


	"""

	def read_from_buf(self, ba ):
		self.op_id=ba.read_uint32();
		if ( self.op_id== None): return False;
		self.vip_ret=ba.read_uint32();
		if ( self.vip_ret== None): return False;
		self.vip_endtime=ba.read_uint32();
		if ( self.vip_endtime== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.op_id)):return False;
		if ( not ba.write_uint32(self.vip_ret)):return False;
		if ( not ba.write_uint32(self.vip_endtime)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"""/*  */"""
		print tabs+"[op_id]=%s"%(str(self.op_id))
		print tabs+"""/*  */"""
		print tabs+"[vip_ret]=%s"%(str(self.vip_ret))
		print tabs+"""/*  */"""
		print tabs+"[vip_endtime]=%s"%(str(self.vip_endtime))
		
		pass;
	




create_time='2012年12月27日 星期4 12:04:28'
cmd_map={ }
cmd_map[0x008B]=(0x008B,'userinfo_check_pay_password',userinfo_check_pay_password_in,None,0xa96a11a6)
cmd_map[0x008C]=(0x008C,'userinfo_set_pay_password',userinfo_set_pay_password_in,None,0x3a111adc)
cmd_map[0x008D]=(0x008D,'userinfo_isset_pay_password',None,userinfo_isset_pay_password_out,0xcc3fa251)
cmd_map[0x009B]=(0x009B,'userinfo_get_gameflag',userinfo_get_gameflag_in,userinfo_get_gameflag_out,0xaa54b631)
cmd_map[0x2001]=(0x2001,'vip_add_days',vip_add_days_in,vip_add_days_out,0x78b82792)
cmd_map[0x2002]=(0x2002,'vip_sub_days',vip_sub_days_in,vip_sub_days_out,0xbf0c5334)
cmd_map[0x2004]=(0x2004,'vip_get_info',None,vip_get_info_out,0x71ac0042)
cmd_map[0x7601]=(0x7601,'pay_get_mb_left',None,pay_get_mb_left_out,0xf2c4efe9)
cmd_map[0x7602]=(0x7602,'pay_become_vip',pay_become_vip_in,pay_become_vip_out,0xd8165379)
cmd_map[0x7603]=(0x7603,'pay_get_become_vip_price',None,pay_get_become_vip_price_out,0x02701dc7)
cmd_map[0xA021]=(0xA021,'main_login_login',main_login_login_in,main_login_login_out,0xa59348f6)
cmd_map[0xA024]=(0xA024,'main_login_check_session',main_login_check_session_in,None,0x372301cc)
cmd_map[0xA032]=(0xA032,'main_login_with_verif_img',main_login_with_verif_img_in,None,0x72adf6bd)
cmd_map[0xA03A]=(0xA03A,'main_login_check_session_new',main_login_check_session_new_in,None,0xab75005c)
cmd_map[0xA122]=(0xA122,'main_login_add_session',main_login_add_session_in,main_login_add_session_out,0xc0d5347b)
cmd_map[0xA123]=(0xA123,'main_login_add_game',main_login_add_game_in,None,0xfa9feb85)
cmd_map[0xA134]=(0xA134,'main_login_add_game_with_chanel',main_login_add_game_with_chanel_in,None,0x581caa2d)
	
def init_cmd_map():
	pass

try:
	import main_login_proto_base
	base_type=main_login_proto_base.Cmain_login_proto_base
except:
	base_type=object
	pass


class Cmain_login_proto (base_type):
	def __init__(self,ip,port):
		main_login_proto_base.Cmain_login_proto_base.__init__(self,ip,port)

	def userinfo_check_pay_password(self ,password ):
		"""
:
	/* 支付密码 */
	#定长数组,长度:16, 类型:uint8_t 
	public password =array();


	"""
		pri_in=userinfo_check_pay_password_in();

		pri_in.password=password
		self.sendmsg(0x008B, pri_in );
		pass

	def userinfo_set_pay_password(self ,password ):
		"""
:
	/* 支付密码 */
	#定长数组,长度:16, 类型:uint8_t 
	public password =array();


	"""
		pri_in=userinfo_set_pay_password_in();

		pri_in.password=password
		self.sendmsg(0x008C, pri_in );
		pass

	def userinfo_isset_pay_password(self  ):
		"""
用户是否设置过支付密码:

	"""
		pri_in=None;

		self.sendmsg(0x008D, pri_in );
		pass

	def userinfo_get_gameflag(self ,region ):
		"""
检查用户的游戏激活标识:
	/* 忽略字段，添0 */
	#类型:uint32
	public region;


	"""
		pri_in=userinfo_get_gameflag_in();

		pri_in.region=region
		self.sendmsg(0x009B, pri_in );
		pass

	def vip_add_days(self ,days,add_flag,channel_id,trade_id ):
		"""
:
	/*  */
	#定长数组,长度:20, 类型:char 
	public trade_id ;


	"""
		pri_in=vip_add_days_in();

		pri_in.days=days
		pri_in.add_flag=add_flag
		pri_in.channel_id=channel_id
		pri_in.trade_id=trade_id
		self.sendmsg(0x2001, pri_in );
		pass

	def vip_sub_days(self ,days,channel_id,trade_id ):
		"""
:
	/*  */
	#定长数组,长度:20, 类型:char 
	public trade_id ;


	"""
		pri_in=vip_sub_days_in();

		pri_in.days=days
		pri_in.channel_id=channel_id
		pri_in.trade_id=trade_id
		self.sendmsg(0x2002, pri_in );
		pass

	def vip_get_info(self  ):
		"""
:

	"""
		pri_in=None;

		self.sendmsg(0x2004, pri_in );
		pass

	def pay_get_mb_left(self  ):
		"""
查询米币余额:

	"""
		pri_in=None;

		self.sendmsg(0x7601, pri_in );
		pass

	def pay_become_vip(self ,channel_id,verify_code,is_auto_charge,use_coupon,coupon,voucher,which_line,extend ):
		"""
游戏内用米币直接开通VIP:
	/* 扩展字段，暂时用不到，填NULL */
	#定长数组,长度:19, 类型:char 
	public extend ;


	"""
		pri_in=pay_become_vip_in();

		pri_in.channel_id=channel_id
		pri_in.verify_code=verify_code
		pri_in.is_auto_charge=is_auto_charge
		pri_in.use_coupon=use_coupon
		pri_in.coupon=coupon
		pri_in.voucher=voucher
		pri_in.which_line=which_line
		pri_in.extend=extend
		self.sendmsg(0x7602, pri_in );
		pass

	def pay_get_become_vip_price(self  ):
		"""
获取VIP开通价格:

	"""
		pri_in=None;

		self.sendmsg(0x7603, pri_in );
		pass

	def main_login_login(self ,email,passwd_md5_two,channel_id,region,gameid,ip ):
		"""
主登录:
	/* 网络序 */
	#类型:uint32
	public ip;


	"""
		pri_in=main_login_login_in();

		pri_in.email=email
		pri_in.passwd_md5_two=passwd_md5_two
		pri_in.channel_id=channel_id
		pri_in.region=region
		pri_in.gameid=gameid
		pri_in.ip=ip
		self.sendmsg(0xA021, pri_in );
		pass

	def main_login_check_session(self ,gameid,session,del_session_flag ):
		"""
检查session:
	/*  */
	#类型:uint32
	public del_session_flag;


	"""
		pri_in=main_login_check_session_in();

		pri_in.gameid=gameid
		pri_in.session=session
		pri_in.del_session_flag=del_session_flag
		self.sendmsg(0xA024, pri_in );
		pass

	def main_login_with_verif_img(self ,email,passwd_md5_two,channel_id,region,gameid,ip,verif_session,verif_code,channel_code ):
		"""
:
	/* 渠道码 */
	#类型:uint16
	public channel_code;


	"""
		pri_in=main_login_with_verif_img_in();

		pri_in.email=email
		pri_in.passwd_md5_two=passwd_md5_two
		pri_in.channel_id=channel_id
		pri_in.region=region
		pri_in.gameid=gameid
		pri_in.ip=ip
		pri_in.verif_session=verif_session
		pri_in.verif_code=verif_code
		pri_in.channel_code=channel_code
		self.sendmsg(0xA032, pri_in );
		pass

	def main_login_check_session_new(self ,from_game,session,del_session_flag,to_game,ip,region,enter_game,tad ):
		"""
验证session:
	/* 登录渠道推广广告位，用于统计广告位带来的登录统计，如果没有推广全部填’\0’,如:media.4399.top.left等， 该字段目前为变长字符串，最长128个字节，以’\0’作为字符串结尾。 */
	#定长数组,长度:128, 类型:uint8_t 
	public tad =array();


	"""
		pri_in=main_login_check_session_new_in();

		pri_in.from_game=from_game
		pri_in.session=session
		pri_in.del_session_flag=del_session_flag
		pri_in.to_game=to_game
		pri_in.ip=ip
		pri_in.region=region
		pri_in.enter_game=enter_game
		pri_in.tad=tad
		self.sendmsg(0xA03A, pri_in );
		pass

	def main_login_add_session(self ,channel_id,verify_code,gameid,ip ):
		"""
加session:
	/* 网络序 */
	#类型:uint32
	public ip;


	"""
		pri_in=main_login_add_session_in();

		pri_in.channel_id=channel_id
		pri_in.verify_code=verify_code
		pri_in.gameid=gameid
		pri_in.ip=ip
		self.sendmsg(0xA122, pri_in );
		pass

	def main_login_add_game(self ,channel_id,verify_code,idczone,gameid ):
		"""
增加项目:
	/*  */
	#类型:uint32
	public gameid;


	"""
		pri_in=main_login_add_game_in();

		pri_in.channel_id=channel_id
		pri_in.verify_code=verify_code
		pri_in.idczone=idczone
		pri_in.gameid=gameid
		self.sendmsg(0xA123, pri_in );
		pass

	def main_login_add_game_with_chanel(self ,channel_code,verify_code,idczone,gameid,channel_id ):
		"""
增加游戏项目（带渠道号）:
	/* 渠道id */
	#类型:uint16
	public channel_id;


	"""
		pri_in=main_login_add_game_with_chanel_in();

		pri_in.channel_code=channel_code
		pri_in.verify_code=verify_code
		pri_in.idczone=idczone
		pri_in.gameid=gameid
		pri_in.channel_id=channel_id
		self.sendmsg(0xA134, pri_in );
		pass


