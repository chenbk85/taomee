#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
import os 
import re 
import mypub 
import MySQLdb 
work_dir="/home/tommychen/DB"
fd=mypub.fielddesc();
fd.init( work_dir+"/bin/sqldesc.txt");
conn=MySQLdb.connect(host="10.1.1.23",user="root",passwd="ta0mee",db="")	
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
	print	"<caption color=red > <a name=%s_2 href=#%s_1 >%s</a>(%s)</caption>"%(tablename,tablename,tablename ,fd.getDesc(tablename.lower()) );
	print "<tr><th>字段名称</th><th>类型<th>数值范围</th><th>初始值</th><th>描述</th><th>附加说明</th></tr>"
	#print cds
	for i in range (0,len(cds)):
		fieldname=tablename +"."+ cds[i][0];
		fieldname_init=fieldname+"._init_";
		fieldname_range=fieldname+"._range_";

		print("<tr><td>%s</td><td>%s </td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>"%(
					cds[i][0], get_type_str_ex( cds[i][1]),
					fd.getDesc(fieldname_range ,'-') ,
					fd.getDesc(fieldname_init,'-' ) ,
					fd.getDesc(fieldname ) ,
					get_field_str_ex( cds[i][3] ) ))

	print "</table>"		
def get_table_list( dbname):
	table_list={ }
	sql="show tables from "+ dbname ;
	#print sql
	#sys.exit(0);
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
		try:
			while  re.search( db_fix[len(db_fix)-1],"0123456789"): 
				db_fix=db_fix[:-1];
		except :
			sys.stderr.write("===="+ dbname+"=====\n");


		#print db_fix
		#目前数据名都是大写
		if (db_fix.isupper()):
			if (  not db_list.has_key(db_fix) ):
				db_list[db_fix]={"dbcount":len(dbname)-len(db_fix)};
				db_list[db_fix]["tablelist"]=get_table_list(dbname);

	return db_list;

def print_db_table_simple_info(db,dblist):
	fix_conf={ 0:"", 1:"0",2:"00" }

	dbcount=dblist[db]["dbcount"];
	db_name=db+fix_conf[dbcount];
	#print db, dblist[db];
	#print tablename
	print "<table id=\"mytable\" cellspacing=\"0\" >";
	print	"<caption color=red >"+db_name+"("+fd.getDesc(db_name.lower()) +")</caption>";
	print "<tr><th>序列 <th> 表名</th><th>分表情况</th><th> 描述</th></tr>"
	tablelist=dblist[db]["tablelist"];
	i=1;
	for table  in tablelist:
		tablecount=tablelist[table]["tablecount"];
		if (tablecount>2):continue; 
		table_name= table+fix_conf[tablecount];
		db_table_name=db_name + "."+ table_name;
		print("<tr><td>%d</td><td> <a name=%s_1 href=#%s_2 >%s</a></td><td>1%s</td><td>%s</td></tr>"%(i,db_table_name,db_table_name ,table_name,fix_conf[tablecount] ,fd.getDesc(db_table_name.lower()) ));
		i=i+1
	print "</table>"


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

def gen_sqldoc( sort_db_list): 
	dblist=get_db_list();
	print mypub.get_html_table_header();
	for db  in sort_db_list :
		print_db_table_simple_info(db,dblist)		

	for db  in sort_db_list :
		print_db_table_def(db,dblist)		
	print "</div>"		
	print "</body></html>"		
	pass
def do_mole_main(argv):
	global work_dir , fd, conn, cursor 
	work_dir="/home/tommychen/DB"
	fd=mypub.fielddesc();
	fd.init( work_dir+"/bin/sqldesc.txt");
	conn=MySQLdb.connect(host="10.1.1.23",user="root",passwd="ta0mee",db="")	
	cursor=conn.cursor()	

	sort_db_list=["USER_INFO_", "EMAIL_DB_","USER_","EMAIL_SYS_","ROOMINFO_","ROOMMSG_",
		"MMS_","HOPE_DB","PARTY_DB","GROUP_",
		"SYSARG_DB", "GROUP_MAIN","USER_SERIAL_DB"];
	no_do_list=["CUP_DB","PP_","SPRING_DB","CHRISTMAS_DB",
		"REGISTER_SERIAL_DB","CONFECT_DB","TEMPGS_DB","PET_SPORTS_DB","SERIAL_DB" ]

	gen_sqldoc(sort_db_list,no_do_list )

