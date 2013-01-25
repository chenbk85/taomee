# -*- coding: utf-8 -*-
import proto_base 
import struct 
class  Cdb_proto_base(proto_base.Cproto_base ): 
	def __init__(self,ip,port):
		proto_base.Cproto_base.__init__(self,ip,port)
		#设置返回报文的头部长度
		self.setheaderlen(18);
		#设置返回报文的头部解析方式
		self.set_unpack_header_fmt_str("<LLHLL");
		pass

	#得到头部的报文
	def pack_proto_header(self, pri_len, cmdid ):
		headermsg=struct.pack("<LLHLL",self.getheaderlen() +pri_len ,
			  0 , cmdid, self.result  ,self.userid);
		self.result=self.result+1
		return headermsg

