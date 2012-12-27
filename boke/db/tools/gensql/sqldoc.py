#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
import os 
import re 
import mypub 
import MySQLdb 
import cgi
fd=None;
conn=None;
cursor=None;

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


def get_table_def_xls(tablename):
	#print tablename
	cursor.execute("desc " + tablename)
	cds=cursor.fetchall()
	print """
	<Row >
   	</Row>
	<Row ss:StyleID="s25">
    <Cell ss:StyleID="s39"><Data ss:Type="String">表名</Data></Cell>
    <Cell><Data ss:Type="String">%s</Data></Cell>
    <Cell><Data ss:Type="String">%s</Data></Cell>
   </Row>
	<Row >
    <Cell ss:StyleID="s33"><Data ss:Type="String">序号</Data></Cell>
    <Cell ss:StyleID="s33"><Data ss:Type="String">列名</Data></Cell>
    <Cell ss:StyleID="s33"><Data ss:Type="String">中文解释</Data></Cell>
    <Cell ss:StyleID="s33"><Data ss:Type="String">主键</Data></Cell>
    <Cell ss:StyleID="s33"><Data ss:Type="String">数据类型</Data></Cell>
    <Cell ss:StyleID="s33"><Data ss:Type="String">默认值</Data></Cell>
    <Cell ss:StyleID="s33"><Data ss:Type="String">说明</Data></Cell>
   </Row>
	"""%(tablename ,fd.getDesc(tablename.lower()))


	for i in range (0,len(cds)):
		fieldname=tablename +"."+ cds[i][0];
		fieldname_init=fieldname+"._init_";
		fieldname_ex=fieldname+"._ex_";
		print("""
		<Row >
	   	<Cell><Data ss:Type="Number">%d</Data></Cell>
	    <Cell><Data ss:Type="String">%s</Data></Cell>
	    <Cell><Data ss:Type="String">%s</Data></Cell>
	    <Cell><Data ss:Type="String">%s</Data></Cell>
	    <Cell><Data ss:Type="String">%s</Data></Cell>
	    <Cell><Data ss:Type="String">%s</Data></Cell>
	    <Cell><Data ss:Type="String">%s</Data></Cell>
		</Row>
	"""%(
					i,
					cgi.escape(cds[i][0]), 
				 	cgi.escape(fd.getDesc(fieldname )) ,
					cgi.escape(get_field_str_ex( cds[i][3] )),
					cgi.escape(get_type_str_ex( cds[i][1])),
					cgi.escape(fd.getDesc(fieldname_init,'-' )) ,
					cgi.escape(fd.getDesc(fieldname_ex,'-' )) 
	))



def get_table_def(tablename):
	#print tablename
	cursor.execute("desc " + tablename)
	cds=cursor.fetchall()
	print "<table id=\"mytable\" cellspacing=\"0\" >";
	print	"<caption color=red > <a name=%s_2 href=#%s_1 >%s</a>(%s)</caption>"%(tablename,tablename,tablename ,fd.getDesc(tablename.lower()) );
	print "<tr><th>字段名称</th><th>中文名</th><th>类型<th>索引</th><th>初始值</th><th>附加说明</th></tr>"
	#print cds
	for i in range (0,len(cds)):
		fieldname=tablename +"."+ cds[i][0];
		fieldname_init=fieldname+"._init_";
		fieldname_ex=fieldname+"._ex_";

		print("<tr><td>%s</td><td>%s </td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>"%(
					cds[i][0], 
					fd.getDesc(fieldname ) ,
					get_type_str_ex( cds[i][1]),
					get_field_str_ex( cds[i][3] ) ,
					fd.getDesc(fieldname_init,'-' ) ,
					fd.getDesc(fieldname_ex,'-' )
				))

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
		while  not re.search(db_fix[0], "0123456789") and re.search( db_fix[len(db_fix)-1],"0123456789"): 
			db_fix=db_fix[:-1];
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

def print_db_table_def_xls(db,dblist):
	fix_conf={ 0:"", 1:"0",2:"00" }
	#print db, dblist[db];
	dbcount=dblist[db]["dbcount"];
	db_name=db+fix_conf[dbcount];
	print """<Row /> <Row /> 
   <Row ss:Height="18.75" ss:StyleID="s24">
    <Cell ss:StyleID="s50"><Data ss:Type="String">库名</Data></Cell>
    <Cell ss:StyleID="s51"><Data ss:Type="String">%s</Data></Cell>
    <Cell ss:StyleID="s51"><Data ss:Type="String">%s</Data></Cell>
    <Cell ss:Index="8" ss:StyleID="s30"/>
   </Row>"""%(db_name, fd.getDesc(db_name.lower()) )
 
	tablelist=dblist[db]["tablelist"];
	for table  in tablelist:
		tablecount=tablelist[table]["tablecount"];
		if (tablecount>2):continue; 
		table_name=table+fix_conf[tablecount];
		#print db_name + "."+ table_name
		get_table_def_xls( db_name + "."+ table_name)		



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
def gen_sqldoc_xls( sort_db_list): 
	dblist=get_db_list();
	print mypub.get_xls_header();
	for db  in sort_db_list :
		print_db_table_def_xls(db,dblist)		
	print mypub.get_xls_end()


