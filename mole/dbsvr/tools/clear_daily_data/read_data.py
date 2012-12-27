#! /usr/bin/env python
# -*- coding: utf-8 -*-

from xml.dom import minidom
import sys
import string
import struct
import time
import thread
import socket

###############################################################################
class XmlParser:
	def __init__(self, file):
		self.loadSource(file)
	
	def load(self, source):
		"""load XML input source, return parsed XML document
		   - a filename of a local XML file ("./conf.xml")
		"""
		try:
			sock = open(source)
		except IOError, (errno, strerror):
			print "I/O error(%s): %s" % (errno, strerror)
	
		xmldoc = minidom.parse(sock).documentElement
		sock.close()
		return xmldoc

	def loadSource(self, source):
		"""load source"""
		self.source = self.load(source)

###############################################################################
class CommData:
	
	def __init__(self):
		self.db_name = []
		self.db_to = []
		self.tb_name = []
		self.tb_to = []
		self.field = []
		self.comm_name = []
		self.log_file = []
		self.comm_type = 0
	
	def append(self,db_name,db_to,tb_name,tb_to,field,comm_name,log_file):
		self.db_name.append(db_name)
		self.db_to.append(db_to)
		self.tb_name.append(tb_name)
		self.tb_to.append(db_to)
		self.field.append(field)
		self.comm_name.append(comm_name)
		self.log_file.append(log_file)

##############################################################################
def parse_xml(source):
	p = XmlParser(source)
	xmldoc = p.source

	all_comm_data = []
	comm_list = xmldoc.getElementsByTagName('Command')
	for comm in comm_list:
		data = parse_comm(comm)
		if data != None:
			all_comm_data.append(data)

	return all_comm_data


###############################################################################
def parse_comm(comm):

	data = CommData()
	comm_type = comm.getAttribute("comm_type")
	comm_flag = 1
	if comm_type == "sqlupdate":
		comm_flag = 1
	elif comm_type == "sqldelete":
		comm_flag = 2
	elif comm_type == "shellcomm":
		comm_flag = 3

	comm_mow = comm.getAttribute("dow")	#check time
	cur_mow = time.strftime("%w")
	if comm_mow == "" or comm_mow == cur_mow:
		parse_comm_param(comm, data, comm_flag)
	
	return data

##############################################################################
def parse_comm_param(comm, data, flag):
	
	data.comm_type = flag;
	param_list = comm.childNodes
	for node in param_list:
		if node.nodeType == node.ELEMENT_NODE and node.nodeName == "Comm" :
			db_name = node.getAttribute("db_name");
			db_to = node.getAttribute("db_to");
			tb_name = node.getAttribute("tb_name");
			tb_to = node.getAttribute("tb_to");
			field = node.getAttribute("field");
			comm_name = node.getAttribute("comm_name")
			log_file = node.getAttribute("log_file")
			data.append(db_name,db_to,tb_name,tb_to,field,comm_name,log_file)

	return True

#############################################################################
####  生成可执行的shell命令
#############################################################################
def print_comm_param(comm, flag, all_str_comm):
	for i in range(0, len(comm.db_name)):
		str_comm = ""
		if flag == 1 and comm.db_name[i] != "":
			str_comm = "./sql_clear_daily_data.sh 1 "
			str_comm = str_comm + comm.db_name[i]+" "
			if comm.db_to[i] == "":
				str_comm = str_comm + "1 "
			else:
				str_comm = str_comm + comm.db_to[i] + " "
			str_comm = str_comm + comm.tb_name[i] + " "
			if comm.tb_to[i] == "":
				str_comm = str_comm + "1 "
			else:
				str_comm = str_comm + comm.tb_to[i] + " "
			str_comm = str_comm + comm.field[i]

		elif flag == 2 and comm.db_name[i] != "":
			str_comm = "./sql_clear_daily_data.sh 2 "
			str_comm = str_comm + comm.db_name[i]+" "
			if comm.db_to[i] == "":
				str_comm = str_comm + "1 "
			else:
				str_comm = str_comm + comm.db_to[i] + " "
			str_comm = str_comm + comm.tb_name[i] + " "
			if comm.tb_to[i] == "":
				str_comm = str_comm + "1 "
			else:
				str_comm = str_comm + comm.tb_to[i]

		elif flag == 3 and comm.comm_name[i] != "":
			str_comm = str_comm + comm.comm_name[i] + " " + comm.field[i] + " " + comm.log_file[i]
		
		if str_comm != "":
			all_str_comm.append(str_comm)


