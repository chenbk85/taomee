#!/usr/bin/python
# -*- coding: utf-8 -*-
import sqldoc
import MySQLdb 
import mypub 
def main():
	sqldoc.fd=mypub.fielddesc();
	sqldoc.fd.init("./sqldesc.txt");
	sqldoc.conn=MySQLdb.connect(host="localhost",user="root",passwd="ta0mee",db="")	
	sqldoc.cursor=sqldoc.conn.cursor()	
	#优先显示的DB列表
	sort_db_list=["IM_"];
	sqldoc.gen_sqldoc(sort_db_list)


if __name__ == '__main__':
	main()
