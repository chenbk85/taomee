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
    

dm=protoCore.Dealmsg()

marketid=4
shopid=2
grid=0
itemid=1300005
cnt=1
uniquekey=0
coins=1

loginser_ip="10.1.1.24"
loginser_port=7788
onlineid=112
userid=105085
passwd="142857"
roletm=1288235881
version=20101223

p=protoCore.mole_login(loginser_ip,loginser_port,onlineid,userid,passwd,roletm,version,dm)

get_msg_return(p,1001);

print "enter market:", marketid
p.player_enter_market(marketid)
p.dealmsg(4001);
print "enter market:",marketid, "OK"

print "trade_buy shopid:",shopid,"grid:",grid,"itemid:",itemid,"uniquekey:",uniquekey,"cnt:",cnt,"coins:",coins
p.player_trade_buy(shopid,grid,itemid,uniquekey,cnt,coins)
#pri_msg=get_msg_return(p,2601);
p.dealmsg(4007);
print "OK:" ,  itemid

