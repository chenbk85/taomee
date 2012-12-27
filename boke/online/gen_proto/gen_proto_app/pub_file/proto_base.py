# -*- coding: utf-8 -*-
import struct
import socket 
import mypub

class Cbyte_array:
	def __init__(self):
		self.m_buf=""
		self.m_size=0
		self.m_postion=0
		self.m_is_read_mode=False;
		self.set_is_big_endian(False);
		pass
	def buf(self):
		return self.m_buf;
	def is_end(self):
		return self.m_postion==self.m_size;
	def set_is_big_endian(self,value):
		self.m_is_big_endian=value;
		if (self.m_is_big_endian):
			self.m_endian_fmt_str=">";
		else:
			self.m_endian_fmt_str="<";
	
	def is_read_mode(self):
		return self.m_is_read_mode;

	def init_read_mode(self,buf):
		self.m_buf=buf;
		self.m_postion=0;
		self.m_size=len(buf);
		self.m_is_read_mode=True;
		pass
	def init_write_mode(self):
		self.m_buf="";
		self.m_is_read_mode=False;
		pass

	def read_buf(self,item_len):
		if not self.m_is_read_mode : return None;
		if (self.m_postion+item_len>self.m_size): return None;
		value=self.m_buf[self.m_postion:self.m_postion+item_len] ;
		self.m_postion=self.m_postion+item_len
		return value;
	def get_postion(self):
		return self.m_postion;

	def get_left_len(self):
		return self.m_size-self.m_postion;


			
	
	def read_single_value(self,frm_str,item_len):
		if not self.m_is_read_mode : return None;
		if (self.m_postion+item_len>self.m_size): return None;
		value=struct.unpack(self.m_endian_fmt_str+frm_str,self.m_buf[self.m_postion:self.m_postion+item_len] );
		self.m_postion=self.m_postion+item_len
		return value[0];

	def read_uint64(self):
		return self.read_single_value("Q",8);
		pass
	def write_uint64(self,value):
		return self.write_single_value("Q",value);
		pass
	def read_int64(self):
		return self.read_single_value("q",8);
		pass
	def write_int64(self,value):
		return self.write_single_value("q",value);
		pass




	def read_uint32(self):
		return self.read_single_value("L",4);
		pass
	def write_uint32(self,value):
		return self.write_single_value("L",value);
		pass

	def read_uint16(self):
		return self.read_single_value("H",2);
		pass
	def write_uint16(self,value):
		return self.write_single_value("H",value);
		pass

	def read_uint8(self):
		return self.read_single_value("B",1);
		pass
	def write_uint8(self,value):
		return self.write_single_value("B",value);
		pass


	def read_double(self):
		return self.read_single_value("d",8);
		pass
	def write_double(self,value):
		return self.write_single_value("d",value);
		pass



	def read_int32(self):
		return self.read_single_value("l",4);
		pass
	def write_int32(self,value):
		return self.write_single_value("l",value);
		pass

	def read_int16(self):
		return self.read_single_value("h",2);
		pass
	def write_int16(self,value):
		return self.write_single_value("h",value);
		pass

	def read_int8(self):
		return self.read_single_value("b",1);
		pass
	def write_int8(self,value):
		return self.write_single_value("b",value);
		pass


	def write_buf(self,buf,item_len):
		if self.m_is_read_mode : return False;
		buf_len=len(buf);
		if (buf_len>=item_len):
			self.m_buf=self.m_buf+buf[0:item_len];
		else:
			self.m_buf=self.m_buf+buf+"\0"*(item_len-buf_len);
		return True;

	def write_single_value(self,frm_str,value):
		if self.m_is_read_mode : return False;
		self.m_buf=self.m_buf+struct.pack( self.m_endian_fmt_str+frm_str, value);
		return True;

class Cmessage:
	def write_to_buf(self,ba):
		return True;
			
	def read_from_buf(self,ba):
		return True;

	def echo(self,print_hex=False,tab="" ):
		pass


#协议通信
class  Cproto_base: 

	def getsock(self):
		return self.sock
	def getheaderlen(self):
		return self.headerlen

	def set_is_big_endian(self,value):
		self.m_is_big_endian=value;
	

	#可能需要子类设置
	def setheaderlen(self,headerlen):
		self.headerlen=headerlen
	
	def __init__(self,ip,port):
		self.set_is_big_endian(False);		
		self.result=0
		self.headerlen=18
		self.ip=ip;
		self.userid=0;
		self.port=port
		self.set_unpack_header_fmt_str();
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)     
		self.getsock().connect((self.ip,self.port))     
		self.recvbuf=""
		#设置为非阻塞连接	
		self.sock.setblocking(0)
		#print "init succ"
	def setblocking(self, flag):
		self.sock.setblocking(flag)
	def set_userid(self, userid):
		self.userid=userid

	def pack(self,cmdid,pri_in):

		ba=Cbyte_array();
		ba.set_is_big_endian(self.m_is_big_endian);
		if (pri_in <>None ) :
			pri_in.write_to_buf(ba);

		headermsg=self.pack_proto_header(len( ba.buf()),cmdid  );

		msg=headermsg+ba.buf();
		return msg;
		pass
	def pack_with_buf(self,cmdid,buf ):
		headermsg=self.pack_proto_header(len(buf),cmdid  );
		msg=headermsg+buf;
		return msg;
		pass



	#用于构造发送包的头部报文,可能需要子类实现
	def pack_proto_header(self, pri_len, cmdid ):
		headermsg=struct.pack("<LHLLL",self.getheaderlen() +pri_len ,
				cmdid, self.userid , 0 , self.result );
		self.result=self.result+1
		return headermsg

	#用于还原接收包的头部报文,可能需要子类实现
	def set_unpack_header_fmt_str(self,  frm_str="<LHLLL" ):
		self.unpack_header_fmt_str=frm_str

	#得到报文的实际长度, 可能需要子类设置
	def get_proto_len(self,  recvbuf):
		recvmsg_arr=struct.unpack("<L",recvbuf[0:4]);
		recvmsg_len=recvmsg_arr[0];
		return recvmsg_len;


	#得到返回的消息
	def getrecvmsg(self):
		tmp_recvfbuf=""
		try :
			tmp_recvfbuf= self.getsock().recv(4096)     
			if ( tmp_recvfbuf =="" ):
				print "socket close.."
			 	exit(1)	
		except socket.error, msg:
			if(msg[0]<>11 ):
				exit(1) 	
			pass
		self.recvbuf=self.recvbuf+tmp_recvfbuf	
		#得到报文		
		if (len( self.recvbuf)<=4):
			return False 
		else:
		 	#得到长度
			recvmsg_len=self.get_proto_len(self.recvbuf);
			print "XXX" , recvmsg_len
			if (recvmsg_len>len(self.recvbuf)):
				return False 

			returnmsg=self.recvbuf[0:recvmsg_len];
			self.recvbuf=self.recvbuf[recvmsg_len:];

			pri_len=recvmsg_len- self.getheaderlen();
			return struct.unpack(self.unpack_header_fmt_str+str(pri_len)+"s", returnmsg);


	def sendmsg(self, cmdid,pri_in ):
		sendbuf=self.pack(cmdid,pri_in);
		#mypub.print_hex_16(sendbuf );
		self.getsock().send(sendbuf)

	def net_io(self, sendbuf ):
		self.setblocking(1);
		sendbuf=self.pack(cmdid,pri_in);
		#mypub.print_hex_16(sendbuf );
		self.getsock().send(sendbuf)
		recvbuf="" 
		try :
			recvbuf= self.getsock().recv(4096)     
		except socket.error, msg:
			return False;

		proto_len=	0;
		if len(recvbuf)>=4:
			proto_len,=struct.unpack("<L",recvbuf[0:4]);
		else:
			return False;
		while len(recvbuf)<proto_len :
			try:
				recvfbuf=recvbuf+self.getsock().recv(4096)     
			except socket.error, msg:
				return False;
		
		return recvbuf;

