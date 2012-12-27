#! /usr/bin/env python
# -*- coding: utf-8 -*-

import mypub 
import sys
import os 
import socket 
import time 
import mypub 
import struct 
from mypub import *






# the global main function for test of mole's login to online server
def mole_online(login_ip, login_port, online_id, user_id, passwd):

	pro = protocol(login_ip, login_port)    #initialize protocol
	pro.set_blocking(1)  #set the mode as blocking to wait until server responses
	pro.login(user_id, passwd);

	login_out=pro.getrecvmsg(); # Get the session returned by login server
        #print  print_hex_16( login_out[5])

	key=login_out[5][4:20] # get key 
	
	
	id1,=struct.unpack(">L",key[10:14] );
        id2,=struct.unpack(">L",key[3:7] );
	md5str = "Bye bYe crAzy B Aby in Our ProgRAm?"    
	k="%u%.11s%u"%(id1,md5str[5:16], id2)

	key2=get_md5(k) #crypt the message
	key2=key2[6:22]

	# obtain the online servers' list 
	pro.get_online_list(online_id,online_id+10 )
	out_msg=pro.getrecvmsg();
	#print  print_hex_16( out_msg[5])

        #parse the message 
	online_id,online_count=struct.unpack(">LL",out_msg[5][4:12]);
	#print "online_id", online_id, "online_count", online_count
	online_ip= out_msg[5][12:28].split('\0');
	online_port,=struct.unpack(">h",out_msg[5][28:30]);
	#print online_ip, online_port
	

	# login to online server 
	pro=protocol(online_ip[0] ,online_port)
	pro.online_login(user_id,online_id ,key ,key2);
	return pro
	pass

	
	
	

class protocol:

	def __init__(self, login_ip, login_port):
                self.header_len = 17
                self.ip = login_ip
		self.port = login_port
		self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.socket.connect((self.ip, self.port))
		self.recvbuf = ""
		self.socket.setblocking(0) # set the default mode as non-blocking

	def get_socket(self):
                return self.socket

	def getheaderlen(self):
                return self.header_len
      
	def set_blocking(self, flag):
                self.get_socket().setblocking(flag)

	def pack(self,cmdid,userid,primsg ):
		#mk header
		result=0
		headermsg=struct.pack(">LBLLL", self.getheaderlen()+len(primsg),1,cmdid,userid, result);
		return  headermsg+ primsg  	
		pass

	def getrecvmsg(self):
		"""得到一个完整报文"""
		tmp_recvfbuf=""
		try :
			#print "in"
		   	tmp_recvfbuf= self.get_socket().recv(4096)     
			#print "getmsg"
		except socket.error, msg:
			#print msg
			if(msg[0]<>11 ):
				#print msg
				exit(1) 	
			pass
		self.recvbuf=self.recvbuf+tmp_recvfbuf	
		#得到报文		
		if (len( self.recvbuf)<=4):
			#print "<4"
			return False 
		else:
		 	#得到长度
		 	#print "begin.."
		 	recvmsg_len,= struct.unpack(">L",self.recvbuf[0:4]);
			#recvmsg_len=recvmsg_arr[0];
			returnmsg=self.recvbuf[0:recvmsg_len];
			self.recvbuf=self.recvbuf[recvmsg_len:];

			pri_len=recvmsg_len- self.getheaderlen();
			return struct.unpack(">LBLLl"+str(pri_len)+"s", returnmsg);



	#These two functions below are used to process on login server 10.1.1.5 
	def login(self,userid,passwd,regflag=0):
		#登入	
		primsg=struct.pack(">32sL",mypub.get_md5( passwd),regflag );
		sendbuf=self.pack(101,userid,primsg); 
		self.get_socket().send(sendbuf) 
		pass

	def get_online_list(self,startid,endid):
		#得到 online 列表
		primsg=struct.pack(">LLL",startid,endid,0 );
		sendbuf=self.pack(103,250,primsg); 
		self.get_socket().send(sendbuf) 
		pass


	#online server login
	def online_login(self,userid,onlineid,key,key2):
		#online login
		self.userid=userid;
		primsg=struct.pack(">h16sL16s",onlineid,key2,16, key );
		sendbuf=self.pack(201,self.userid,primsg);  # pack online login message
		self.get_socket().send(sendbuf) 
		pass



	# action protocols
	def buy_item(self,itemid,count ):
		"""buy props"""
		primsg=struct.pack(">LL",itemid,count );
		sendbuf=self.pack(501,self.userid,primsg); 
		self.get_socket().send(sendbuf) 
		pass

	def throw_item(self,itemid,x,y):
		"""投掷东西"""
		primsg=struct.pack(">LLL",itemid,x,y );
		sendbuf=self.pack(306,self.userid,primsg); 
		self.get_socket().send(sendbuf) 
		pass

	def gen_rand_item(self,itemid):
		primsg=struct.pack(">L",itemid );
		sendbuf=self.pack(320,self.userid,primsg); 
		#print mypub.print_hex_16(sendbuf);
		self.get_socket().send(sendbuf) 
		pass

	def get_rand_item(self,posid):
		primsg=struct.pack(">LL",posid,0);
		sendbuf=self.pack(318,self.userid,primsg); 
		self.get_socket().send(sendbuf) 
		pass

	def change_name(self, new_name):
		primsg  = struct.pack(">16s", new_name)
		sendbuf=self.pack(208, self.userid,primsg); 
		print "change name........."
		self.get_socket().send(sendbuf) 
		pass

	def check_bag(self, itemid):
		primsg=struct.pack(">L",itemid);
		sendbuf=self.pack(513,self.userid,primsg); 
		self.get_socket().send(sendbuf) 
		print "check bag........."
		print mypub.print_hex_16(sendbuf);
		pass

#	def sell_goods(self, itemid, price, count):
#		primsg=struct.pack(">LLL8s",itemid,price,count,self.recv_session);
#		sendbuf=self.pack(514,self.userid,primsg); 
#		self.get_socket().send(sendbuf) 
#		pass

	def sell_goods(self,itemid,price, count, session):
		primsg=struct.pack(">LLL8s",itemid,price,count,session);
		sendbuf=self.pack(514,self.userid,primsg); 
		self.get_socket().send(sendbuf) 
		print "selling goods........"
		print mypub.print_hex_16(sendbuf);
		pass

	def check_vow(self):
		primsg=""
		sendbuf=self.pack(754,self.userid,primsg) 
		self.get_socket().send(sendbuf) 
		print "check vow........"
		print mypub.print_hex_16(sendbuf);
		pass
	
	def make_vow(self, recv_id, item_id, item_name, num_bean):
		primsg=struct.pack(">LL30sL",recv_id, item_id, item_name, num_bean)
		sendbuf=self.pack(751, self.userid, primsg)
		self.get_socket().send(sendbuf)
		print "making vow....."
		print mypub.print_hex_16(sendbuf)
		pass
		
	def move(self,x,y ):
		"""walk"""
		primsg=struct.pack(">LLL",x,y,0);
		sendbuf=self.pack(303,self.userid,primsg); 
		self.get_socket().send(sendbuf) 
		pass
	def action(self, actionid, flag=0 ):
		"""动作"""
		primsg=struct.pack(">LB",actionid,flag );
		sendbuf=self.pack(305,self.userid,primsg); 
		self.get_socket().send(sendbuf) 
		pass

	def talk(self,msg ):
		"""说话"""
		msglen=len(msg)
		primsg=struct.pack(">LL"+str(msglen)+"s",0,msglen,msg);
		sendbuf=self.pack(302,self.userid,primsg); 
		#print mypub.print_hex_16(sendbuf);
		self.get_socket().send(sendbuf) 
		pass

	def cd_map(self,mapid):
		primsg="";
		sendbuf=self.pack(402,self.userid,primsg); 
		print mypub.print_hex_16(sendbuf);
		self.get_socket().send(sendbuf) 

		primsg=struct.pack(">LL",mapid,0);
		sendbuf=self.pack(401,self.userid,primsg); 
		print mypub.print_hex_16(sendbuf);
		self.get_socket().send(sendbuf) 
		pass


if len(sys.argv) <> 4:   
	print u'使用方法：./protocol.py userid password startid\n'   
	exit(0)

userid=sys.argv[1]
passwd=sys.argv[2]
startid=sys.argv[3]


p=protocol("114.80.98.17", 1863)
#p=protocol("10.1.1.5", 7777)
p.set_blocking(1)

#msg=True
#while(msg):
#	msg=p.getrecvmsg();

p.login(int(userid), passwd);

time.sleep(1)

login_out=p.getrecvmsg(); # Get the session returned by login server
#print  print_hex_16( login_out[5])

key=login_out[5][4:20] # get key 


id1,=struct.unpack(">L",key[10:14] );
id2,=struct.unpack(">L",key[3:7] );
md5str = "Bye bYe crAzy B Aby in Our ProgRAm?"
k="%u%.11s%u"%(id1,md5str[5:16], id2)

key2=get_md5(k) #crypt the message
key2=key2[6:22]

#onlineid=1;
#allcount=0;
#for i in range(20):
#print i

time.sleep(1)

#exit(0)

#out_msg=True
#while(out_msg):
#	print "...."
#	out_msg=p.getrecvmsg();
#	print ",,,,"

p.get_online_list(int(startid)+1,int(startid)+100)
time.sleep(1)
out_info=False
while (not out_info):
	out_info=p.getrecvmsg();
	if (out_info):
		proto_len,version,cmdid,userid,result, pri_msg=out_info;
        if(cmdid == 103):
			#print mypub.print_hex_16(pri_msg)
			online_cnt, = struct.unpack(">L", pri_msg[0:4])
			#print online_cnt
			j = 4
			for i in range(online_cnt):
				online_id, usercnt, ip, port, fri = struct.unpack(">LL16shI", pri_msg[j:(j+30)])
				#print online_id, usercnt, ip, port, fri
				online_ip= pri_msg[(j+8):(j+24)].split('\0');
				
				#p=protocol("14.80.98.17", 1863)
				#p.set_blocking(1)
 
				#p.login(38010831, "201217yp");

				#time.sleep(1)

				#login_out=p.getrecvmsg(); # Get the session returned by login server
				#print  print_hex_16( login_out[5])

				#key=login_out[5][4:20] # get key 

				#id1,=struct.unpack(">L",key[10:14] );
				#id2,=struct.unpack(">L",key[3:7] );
				#md5str = "Bye bYe crAzy B Aby in Our ProgRAm?"
				#k="%u%.11s%u"%(id1,md5str[5:16], id2)

				#key2=get_md5(k) #crypt the message
				#key2=key2[6:22]

				pro=protocol(online_ip[0], port);
				pro.online_login(38010831 ,online_id ,key ,key2);		
				time.sleep(1.5)
				
				lo_msg = pro.getrecvmsg();
				if(not lo_msg):
					print "online login failed", online_id
				else:
				 	print "online login ok", online_id

				j = j+ 30


	

#online_ip = out_msg[5][12:28].split('\0');
#online_port,=struct.unpack(">h",out_msg[5][28:30]);
#print online_ip, online_port


#print "online_count:", online_count;
        #for j in range(online_count):
        #id,count=struct.unpack(">LL", out_msg[5][4+j*30: 12+j*30]);
        #print id,count
        #allcount=allcount+count;#pass


#print  allcount

