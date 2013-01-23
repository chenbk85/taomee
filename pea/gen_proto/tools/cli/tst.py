#!/usr/bin/env python
# -*- coding: utf-8 -*-
import mole2_online_proto
import mole2_online
import login_proto 
import proto_base 
import time 
import mypub 

class Dealmsg: 
	def __init__(self):
		pass

	def do_cli_login(self,result,primsg):
		print "asdfasfa ",  result
		pass

	def dealmsg(self,cmdid,userid,result,primsg):
		if mole2_online_proto.cmd_map.has_key(cmdid):
			#得到类名
			classname=mole2_online_proto.cmd_map[cmdid][3];

			if classname <> None:	
				pri_out=classname() ;
			else:
				pri_out=proto_base.Cmessage();
			
			ba=proto_base.Cbyte_array();
			ba.set_is_big_endian(True);
			ba.init_read_mode(primsg);
			if (result==0 ):
				ret=pri_out.read_from_buf(ba);
				if ( not  ret ):
					print "解析出错:报文不够",classname;
				if (ba.get_left_len() >0 ):
					print "解析出错:有剩余报文";
					mypub.print_hex_16(primsg[ba.get_postion():] );
			

			try:
				#得到调用函数
 				cmd_name=mole2_online_proto.cmd_map[cmdid][1];
				func_name='do_%s'%( cmd_name ) 
				print func_name
				func = getattr(self , func_name  )
				func(result,primsg);
			except AttributeError:
				print "deal: userid=%d, %s[%d] : "%(userid,cmd_name , cmdid )
				pri_out.echo(True, "    ");
				pass
		else:
			print "未处理:", cmdid, result
			mypub.print_hex_16(primsg)


dm=Dealmsg()

def get_msg_return(need_cmdid,asnyc_flag=False):
	global p
	global dm 
	start_time=time.clock();
	while True:
		out_info=p.getrecvmsg();
		if (out_info):
			proto_len ,cmdid,   userid, seq, result,pri_msg=out_info;
			print ">>>>>>OUT:", cmdid, result, userid ,proto_len  ;
			if cmdid==1001:
				p.set_seq(seq);
			dm.dealmsg(cmdid,userid,result,pri_msg );
			if (cmdid==need_cmdid):
				return pri_msg 

		end_time=time.clock();
		if asnyc_flag and end_time-start_time>1:#是异步的
		  	return False;

userid=50066
#logser 
ip,port,key=login_proto.login("10.1.1.46", 8989,9 ,userid,"142857")
print "login online:" ,ip,port
p=mole2_online.Cmole2_online(ip,port );
p.set_is_big_endian(True);
p.login(userid,key);
get_msg_return(1001,False);
p.cli_user_walk(100,135,345,456,0,0,0,0);

get_msg_return(1001,True );

