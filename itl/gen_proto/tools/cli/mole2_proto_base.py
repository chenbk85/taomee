# -*- coding: utf-8 -*-
import proto_base
import struct
class  Cmole2_proto_base(proto_base.Cproto_base ):
	def __init__(self,ip,port):
		proto_base.Cproto_base.__init__(self,ip,port)
		#设置返回报文的头部长度
		self.setheaderlen(18);
		#设置返回报文的头部解析方式
		self.set_unpack_header_fmt_str(">LHLLL");
		self.seq=0;
		pass

	#得到头部的报文
	def pack_proto_header(self, pri_len, cmdid ):
		headermsg=struct.pack(">LHLLL",self.getheaderlen() +pri_len ,
			cmdid, self.userid , self.seq ,self.result );
		return headermsg

	def get_proto_len(self,  recvbuf):
		recvmsg_arr=struct.unpack(">L",recvbuf[0:4]);
		recvmsg_len=recvmsg_arr[0];
		return recvmsg_len;

