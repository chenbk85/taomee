#!/usr/bin/python
# -*- coding: utf-8 -*-
import sqldoc
import MySQLdb 
import mypub 
def main():
	sqldoc.fd=mypub.fielddesc();
	sqldoc.fd.init("/home/easyeagel/DB/bin/sqldesc.txt");
	sqldoc.conn=MySQLdb.connect(host="10.1.1.5",user="root",passwd="ta0mee",db="")	
	sqldoc.cursor=sqldoc.conn.cursor()	


	#优先显示的DB列表
	sort_db_list=["PP_","PP_PK_TEAM_","PP_OTHER_DB","TASK_DAY_DB","PP_LOG_DB" ];
	sqldoc.gen_sqldoc(sort_db_list)

if __name__ == '__main__':
	main()
