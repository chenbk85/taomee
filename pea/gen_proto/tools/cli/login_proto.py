# -*- coding: utf-8 -*-
import mole2_proto_base 
import struct
import mypub
class  Clogin_proto(mole2_proto_base .Cmole2_proto_base):
	def login(self,userid,passwd,regflag=0):
		self.set_userid(userid);
		primsg=struct.pack(">32sLLL16s6s",
				mypub.get_md5( passwd),regflag,7,0," "*16," "*6 );
		sendbuf=self.pack_with_buf(103,primsg);
		self.getsock().send(sendbuf)
		pass

	def get_online_list(self,startid,endid):
		#得到 online 列表
		primsg=struct.pack(">LLL",startid,endid,0 );
		sendbuf=self.pack_with_buf(106,primsg);
		self.getsock().send(sendbuf)
		pass

def login(loginser_ip,loginser_port,onlineid,userid, passwd ):
	p=Clogin_proto(loginser_ip,loginser_port)
	p.login(userid, passwd);
	p.setblocking(1)
	login_out=p.getrecvmsg();
	#print  print_hex_16( login_out[5])
	key=login_out[5][0:16]

	#得到online列表  
	p.get_online_list(onlineid,onlineid )
	out_msg=p.getrecvmsg();
	#print  print_hex_16( out_msg[5])

	online_id,online_count=struct.unpack(">LL",out_msg[5][4:12]);
	#print "online_id", online_id, "online_count", online_count
	online_ip= out_msg[5][12:28].split('\0')[0];
	online_port,=struct.unpack(">h",out_msg[5][28:30]);

	return online_ip,online_port,key ;

