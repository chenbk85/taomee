#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import os 
import time 
import mypub 
import struct 
import socket
from mypub import *
from array import *
import hashlib
import itl_db_proto


ip=struct.unpack("I", socket.inet_aton("10.1.6.245"))[0];
p=itl_db_proto.Citl_db_proto("10.1.6.245", 16000)
print ip;
p.set_userid(0);

p.db_p_get_node_server_config(ip);
#p.db_p_get_node_alarm_config(3180);
p.get_msg_return(itl_db_proto);
