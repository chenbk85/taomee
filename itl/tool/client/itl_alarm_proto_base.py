# -*- coding: utf-8 -*-
import struct
import proto_base 

class  Citl_alarm_proto_base(proto_base.Cproto_base):
    def __init__(self,ip,port):
        proto_base.Cproto_base.__init__(self,ip,port);
        # 设置返回报文的头部长度
        self.setheaderlen(18);
        # 设定字节序
        self.set_is_big_endian(False);
        # 设置返回报文的头部解析方式
        self.set_unpack_header_fmt_str("<LLHLL");
    pass

# 构造头部的报文
    def pack_proto_header(self, pri_len, cmdid ):
        print cmdid;
        print self.userid;
        headermsg=struct.pack("<LLHLL", 
                self.getheaderlen() + pri_len,
                0,
                cmdid,
                self.userid,
                0);

        self.result=self.result+1;
        return headermsg

    # 得到报文的实际长度, 可能需要子类设置
    def get_proto_len(self, recvbuf):

        recvmsg_arr=struct.unpack("<L",recvbuf[0:4]);

        recvmsg_len=recvmsg_arr[0];
        return recvmsg_len;
