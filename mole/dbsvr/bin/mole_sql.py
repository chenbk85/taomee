#!/usr/bin/python
# -*- coding: utf-8 -*-
import sqldoc
import MySQLdb 
import mypub 
def main():
	sqldoc.fd=mypub.fielddesc();
	sqldoc.fd.init("/home/tommychen/DB/bin/sqldesc.txt");

	sqldoc.conn=MySQLdb.connect(host="10.1.1.23",user="root",passwd="ta0mee",db="")	
	sqldoc.cursor=sqldoc.conn.cursor()	

	#优先显示的DB列表
	sort_db_list=["USER_INFO_", "EMAIL_DB_","USER_","EMAIL_SYS_","ROOMINFO_","ROOMMSG_",
		"MMS_","HOPE_DB","PARTY_DB","GROUP_",
		"SYSARG_DB", "GROUP_MAIN","USER_SERIAL_DB"];

	sqldoc.gen_sqldoc(sort_db_list)

if __name__ == '__main__':
	main()
