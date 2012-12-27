#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import os
import time
import struct
from read_data import *

LOG_FILE = "log_file_deal_daily.`date -I`"
###############################################################################
def main(xml_file):
	print "--------------------------------------------------------------------------"
	all_comm_data = parse_xml(xml_file)
	str_comm_param = []
	for comm in all_comm_data:
		print_comm_param(comm, comm.comm_type, str_comm_param)
	
	os.system("echo logbegin: >" + LOG_FILE)
	for str in str_comm_param:
		#print str
		os.system(str + " >> " + LOG_FILE)
	print "--------------------------------------------------------------------------"

###############################################################################
################################   main function  #############################
###############################################################################
if __name__ == "__main__":
	main("conf.xml")
