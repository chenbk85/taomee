# -*- coding: utf-8 -*-
import mole2_online_proto
import struct
import mypub
class  Cmole2_online (mole2_online_proto.Cmole2_online_proto ):
	def login(self,userid,key):
		self.set_userid(userid);
		primsg=struct.pack(">16s",key);
		sendbuf=self.pack_with_buf(1001,primsg);
		self.getsock().send(sendbuf)
		pass

