# -*- coding: utf-8 -*-
import mole2_proto_base 
import struct
import mypub
class  Cmole2_online_proto_base(mole2_proto_base .Cmole2_proto_base):
	def set_seq(self, seq) :
		self.seq=seq
	def calc_next_seq(self ,len, cmdid):
		#int32_t seq = p->seq - p->seq / 7 + 140 + pkg_head.len % 26 + pkg_head.cmd % 21;
		self.seq=self.seq-self.seq/7 +140 + len   % 26 + cmdid % 21
	#得到头部的报文
	def pack_proto_header(self, pri_len, cmdid ):
		self.calc_next_seq(self.getheaderlen() +pri_len,cmdid  )
		headermsg=struct.pack(">LHLLL",self.getheaderlen() +pri_len ,
			cmdid, self.userid , self.seq ,self.result );
		return headermsg
