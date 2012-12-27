#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import os 
import protoCore
import time 
import mypub 
import struct 
from mypub import *

def get_msg_return(p, need_cmdid):
    while True:
        out_info=p.getrecvmsg();
        if (out_info):
            proto_len,cmdid,seq,userid,result, pri_msg=out_info;
            #print proto_len, cmdid, result, userid , mypub.print_hex_16(pri_msg,0 );
            if (cmdid==need_cmdid):
                return pri_msg; 
    

itemid=int(sys.argv[1])
itemcnt=int(sys.argv[2])
dm=protoCore.Dealmsg()

loginser_ip="10.1.1.24"
loginser_port=7788
onlineid=112
userid=105084
passwd="142857"
roletm=1287653233
version=20101216

p=protoCore.mole_login(loginser_ip,loginser_port,onlineid,userid,passwd,roletm,version,dm)

get_msg_return(p,1001);

print "buy_item:" , itemid, "count", itemcnt
p.buy_item(itemid, itemcnt)
#pri_msg=get_msg_return(p,2601);
p.dealmsg(2601);
print "OK:" ,  itemid

