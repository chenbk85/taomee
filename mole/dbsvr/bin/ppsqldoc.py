#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
import os 
import re 
import mypub 
import MySQLdb 
work_dir="~/DB"
fd=mypub.fielddesc();
fd.init( work_dir+"/bin/sqldesc.txt");
conn=MySQLdb.connect(host="10.1.1.5",user="root",passwd="ta0mee",db="")	
cursor=conn.cursor()	

def get_field_str_ex(s):

	key_type={
	"PRI":"主键",
	}
	if s:
		if( key_type.has_key(s) ):
			return key_type[s] 
		else:
			return s 
	else:
		return "　"
def get_type_str_ex(s):

	key_type={
	"int(10) unsigned":"uint32",
	}
	if s:
		if( key_type.has_key(s) ):
			return key_type[s] 
		else:
			return s 
	else:
		return "　"




def get_table_def(tablename):
	#print tablename
	cursor.execute("desc " + tablename)
	cds=cursor.fetchall()
	print "<table id=\"mytable\" cellspacing=\"0\" >";
	print	"<caption color=red >"+tablename +"("+fd.getDesc(tablename.lower()) +")</caption>";
	print "<tr><th>字段名称</th><th>类型</th><th>描述</th><th>附加说明</th></tr>"
	#print cds
	for i in range (0,len(cds)):
		print("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>"%(
					cds[i][0], get_type_str_ex( cds[i][1]),
					fd.getDesc(tablename +"."+ cds[i][0] ) ,
					get_field_str_ex( cds[i][3] ) ))

	print "</table>"		
def get_table_list( dbname):
	table_list={ }
	sql="show tables from "+ dbname ;
	#print sql
	cursor.execute(sql)
	cds=cursor.fetchall()
	#print  cds;
	for i in range (0,len(cds) ):
		tablename=cds[i][0];
		#print tablename
		table_fix=tablename;
		while  re.search( table_fix[len(table_fix)-1],"0123456789"): 
			table_fix=table_fix[:-1];
		#print table_fix
		if ( not table_list.has_key(table_fix) ):
			table_list[table_fix]={"tablecount":len(tablename)-len(table_fix)};
								

	#print  table_list;
	return table_list;



def get_db_list():
	db_list={ }

	cursor.execute("show  databases ")
	cds=cursor.fetchall()
		
	#print  cds;
	for i in range (0,len(cds) ):
		dbname=cds[i][0];
		db_fix="";
		db_fix=dbname;
		while  re.search( db_fix[len(db_fix)-1],"0123456789"): 
			db_fix=db_fix[:-1];
		#print db_fix
		#目前数据名都是大写
		if (db_fix.isupper()):
			if (  not db_list.has_key(db_fix) ):
				db_list[db_fix]={"dbcount":len(dbname)-len(db_fix)};
				db_list[db_fix]["tablelist"]=get_table_list(dbname);

	return db_list;
def print_db_table_def(db,dblist):
	fix_conf={ 0:"", 1:"0",2:"00" }
	#print db, dblist[db];
	dbcount=dblist[db]["dbcount"];
	db_name=db+fix_conf[dbcount];
	print "<br /><table id=\"mytable_1\" cellspacing=\"0\" >";
	print "<tr><th width=50 class=\"title_def\">%s</th><th class=\"title_def\">%s</th><th class=\"title_def\" width=100 > </th></tr>"%(
			db_name, fd.getDesc(db_name.lower()) )
	print "</table>"		


	tablelist=dblist[db]["tablelist"];
	for table  in tablelist:
		tablecount=tablelist[table]["tablecount"];
		if (tablecount>2):continue; 
		table_name=table+fix_conf[tablecount];
		#print db_name + "."+ table_name
		get_table_def( db_name + "."+ table_name)		

def main(argv):
	dblist=get_db_list();
	sort_db_list=["PP_"];
	print mypub.get_html_table_header();

	for db  in sort_db_list :
		print_db_table_def(db,dblist)		

	#get_table_def("USER_00.t_user_0")
	print "</div>"		
	print "</body></html>"		
	
if __name__ == '__main__':
	main(sys.argv)
