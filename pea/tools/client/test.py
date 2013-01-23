#!/usr/bin/env python
# -*- coding: utf-8 -*-



import sys
import os 
import pea_online_proto
import pea_db_proto
import pea_login_proto
import proto_base 
import time 
import mypub 
import struct 
from mypub import *
from array import *
import md5






class dealmsg(proto_base.Cdealmsg_base): 
    def __init__(self):
        pass

    def do_cli_login(self,result,primsg):
        """xdsfadf"""
        print "asdfasfa "
        pass

def get_msg_return(asnyc_flag=False):
    global p
    global dm 
    dm.set_is_big_endian()

    
    ret = p.recvmsg()
    while (not ret):
        ret = p.recvmsg()

    msg = p.get_msg()
    while (msg):
        print msg
        len, cmdid, userid, seq, ret, body = msg
        #dm.dealmsg( pea_login_proto, cmdid, ret, body );
        dm.dealmsg( pea_online_proto, cmdid, ret, body );
        msg = p.get_msg()

        

dm=dealmsg();


#p=pea_login_proto.Cpea_login_proto("10.1.5.26", 6708);
#p=pea_login_proto.Cpea_login_proto("10.1.1.131", 6708);
#p.set_userid(483);
#password=array("B", md5.new(md5.new("11111").hexdigest()).hexdigest());
#img_id=array("B", '0000000000000000');
#veri=array("B", '000000');
#p.proto_login(password, 0, 1, 0, img_id, veri);
#get_msg_return();
#p.proto_get_server_list("", 0);
#get_msg_return();
#p.proto_get_role_list(8);
#get_msg_return();
#model=model_info_t();
#model.eye_mode=20011;
#model.resource_id=20002;
#p.proto_create_role(20, "test", model);
#get_msg_return();

p=pea_online_proto.Cpea_online_proto("10.1.5.26", 6808)
p.set_userid(483);
p.cli_proto_login("", 483, 1338371101, 8);
get_msg_return();
p.cli_proto_enter_map(10, 450, 540);
get_msg_return();
#p.cli_proto_get_map_user_list();
#get_msg_return();
#p.cli_proto_join_room(524290, "");
#get_msg_return();
#p.cli_proto_get_btl_user_list();
#get_msg_return();
#p.cli_proto_create_room("test", "", 1, 10, 483)
#get_msg_return();
#p.cli_proto_get_bag_pet();
#get_msg_return();
p.cli_proto_get_pet_detail(8);
get_msg_return();
#pet_no=array('l', [1, 4]);
#p.cli_proto_pet_merge(2, pet_no);
#get_msg_return();
#p.cli_proto_upgrade_pet_iq(2);
#get_msg_return();
#p.cli_proto_upgrade_pet_gift(2, 0);
#get_msg_return();
#gift_no=array('l', []);
#p.cli_proto_rand_pet_gift(2, gift_no);
#get_msg_return();
#p.cli_proto_get_items();
#get_msg_return();
#p.cli_proto_try_pet_train(2, 1);
#get_msg_return();
#get_msg_return();
#p.cli_proto_set_pet_train(2, 1);
#get_msg_return();

#p.cli_proto_set_assist_pet(0, 3);
#get_msg_return();
#p.cli_proto_get_bag_pet();
#get_msg_return();
#p.cli_proto_create_room("", "", 1, 1, 101638);
#get_msg_return();

#p.cli_proto_get_items();
#get_msg_return();
#p.cli_proto_get_npc_shop(10002);
#get_msg_return();
#p.cli_proto_buy_npc_shop_item(10002, 10001, 1);
#get_msg_return();
#p.cli_proto_npc_item(18, 90);
#get_msg_return();
#p.cli_proto_get_items();
#get_msg_return();
#p.cli_proto_get_redeem_shop();
#get_msg_return();
#p.cli_proto_redeem_item(0);
#get_msg_return();
#p.cli_proto_get_redeem_shop();
#get_msg_return();
