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
    

taskid=int(sys.argv[1])
dm=protoCore.Dealmsg()

loginser_ip="10.1.1.24"
loginser_port=7788
onlineid=112
userid=105084
passwd="142857"
roletm=1287653233
version=20101118

p=protoCore.mole_login(loginser_ip,loginser_port,onlineid,userid,passwd,roletm,version,dm)

get_msg_return(p,1001);

print "accept_NPC_task:" ,  taskid
p.accept_NPC_task(taskid)
#pri_msg=get_msg_return(p,2201);
p.dealmsg(2201);
print "OK:" ,  taskid

print "complete_NPC_task:" ,  taskid
p.complete_NPC_task(taskid,1)
#pri_msg=get_msg_return(p,2202)
p.dealmsg(2202);
print "OK:" ,  taskid

