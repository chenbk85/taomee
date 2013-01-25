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

		
class session_add_session_in :
	"""
参数列表:
	/* 项目类型:1:摩尔, 2:PP, 3:大玩国,4,DUDU, 5:小花仙, 6,功夫, 7:mole2(勇士),8:摩尔宝宝，21:哈奇,10000:支付平台 */
	#类型:uint32
	public gameid;

	/*  */
	#类型:uint32
	public ip;


	"""

	def read_from_buf(self, ba ):
		self.gameid=ba.read_uint32();
		if ( self.gameid== None): return False;
		self.ip=ba.read_uint32();
		if ( self.ip== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.gameid)):return False;
		if ( not ba.write_uint32(self.ip)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"/* 项目类型:1:摩尔, 2:PP, 3:大玩国,4,DUDU, 5:小花仙, 6,功夫, 7:mole2(勇士),8:摩尔宝宝，21:哈奇,10000:支付平台 */"
		print tabs+"[gameid]=%s"%(str(self.gameid))
		print tabs+"/*  */"
		print tabs+"[ip]=%s"%(str(self.ip))
		
		pass;
	




class session_add_session_out :
	"""
参数列表:
	/* 计算得到的session值 */
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
		print tabs+"/* 计算得到的session值 */"
		tmp_str=str(self.session);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[session]=%s"%(tmp_str)
		
		pass;
	




class session_check_session_in :
	"""
参数列表:
	/* 项目类型:1:摩尔, 2:PP, 3:大玩国,4,DUDU, 5:小花仙, 6,功夫, 7:mole2(勇士),8:摩尔宝宝，21:哈奇,10000:支付平台 */
	#类型:uint32
	public gameid;

	/* 计算得到的session值 */
	#定长数组,长度:16, 类型:char 
	public session ;

	/* 验证后是否要删除session(0:不是,1:是) */
	#类型:uint32
	public del_flag;


	"""

	def read_from_buf(self, ba ):
		self.gameid=ba.read_uint32();
		if ( self.gameid== None): return False;
		self.session=ba.read_buf(16);
		if( self.session==None ): return False;
		self.del_flag=ba.read_uint32();
		if ( self.del_flag== None): return False;

		return True;


	def write_to_buf(self,ba):
		if ( not ba.write_uint32(self.gameid)):return False;
		ba.write_buf(self.session,16);
		if ( not ba.write_uint32(self.del_flag)):return False;
		return True;
	
 
	def echo(self, print_hex=False,tabs="" ):
		print tabs+"/* 项目类型:1:摩尔, 2:PP, 3:大玩国,4,DUDU, 5:小花仙, 6,功夫, 7:mole2(勇士),8:摩尔宝宝，21:哈奇,10000:支付平台 */"
		print tabs+"[gameid]=%s"%(str(self.gameid))
		print tabs+"/* 计算得到的session值 */"
		tmp_str=str(self.session);
		if(print_hex):tmp_str=bin2hex(tmp_str)
		print tabs+"[session]=%s"%(tmp_str)
		print tabs+"/* 验证后是否要删除session(0:不是,1:是) */"
		print tabs+"[del_flag]=%s"%(str(self.del_flag))
		
		pass;
	




create_time='2011年03月01日 星期2 16:51:58'
cmd_map={ }
cmd_map[0x0B01]=(0x0B01,'session_add_session',session_add_session_in,session_add_session_out,0x37b50bf6)
cmd_map[0x0B02]=(0x0B02,'session_check_session',session_check_session_in,None,0xa2a6441f)
	
def init_cmd_map():
	pass

try:
	import session_ser_proto_base
	base_type=session_ser_proto_base.Csession_ser_proto_base
except:
	base_type=object
	pass


class Csession_ser_proto (base_type):
	def __init__(self,ip,port):
		session_ser_proto_base.Csession_ser_proto_base.__init__(self,ip,port)

	def session_add_session(self ,gameid,ip ):
		"""
加入session:
	/*  */
	#类型:uint32
	public ip;


	"""
		pri_in=session_add_session_in();

		pri_in.gameid=gameid
		pri_in.ip=ip
		self.sendmsg(0x0B01, pri_in );
		pass

	def session_check_session(self ,gameid,session,del_flag ):
		"""
检查session:
	/* 验证后是否要删除session(0:不是,1:是) */
	#类型:uint32
	public del_flag;


	"""
		pri_in=session_check_session_in();

		pri_in.gameid=gameid
		pri_in.session=session
		pri_in.del_flag=del_flag
		self.sendmsg(0x0B02, pri_in );
		pass


