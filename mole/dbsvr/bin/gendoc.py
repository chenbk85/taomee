#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
import os 
import optparse
import gendoc_core 
def print_filed( fielditem, fix ,struct_list="" ):
	type= fielditem["type"] ;
	field_name= fielditem["fieldname"];
	count_def= fielditem["count"];	
	if  count_def:
		count_def="["+count_def +"]"	
	td_class=""	
			
	#print ( "<tr ><td %s>%s%s</td><td>%s%s</td><td>%s</td></tr>"%(td_class ,fix ,field_name ,
	#			gendoc_core.get_type_doc(type),count_def ,
	#			 struct_list+"."+field_name + gendoc_core.fd.getDesc( struct_list+"."+field_name )))

	print ( "<tr ><td %s>%s%s</td><td>%s%s</td><td>%s</td></tr>"%(td_class ,fix ,field_name ,
				gendoc_core.get_type_doc(type),count_def ,
				gendoc_core.fd.getDesc( struct_list+"."+field_name )))
	pass 


def show_struct(struct_def , fix="", struct_list=""):
	memberlist=struct_def["memberlist"];
	struct_name=struct_def["struct_name"];
	for mv in memberlist:
		fielditem=memberlist[mv]  ;
		print_filed(fielditem ,fix,struct_list+"."+struct_name );
		if fielditem["struct_def"] :
			show_struct( fielditem["struct_def"],fix+">>>>&nbsp;",
					struct_list+"."+struct_name )		
	

#打印命令的明细
def print_all_cmd_def(db):
	#打印DB的头部
	cmdlist=db["cmdlist"];
	keylist=cmdlist.keys();
	keylist.sort();

	id=0
	for k in keylist:
		cmditem=cmdlist[k]
		cmdname=cmditem["cmdname"];
		cmdid=cmditem["cmdid"];
		comment=gendoc_core.command_conf[k][2]
		need_update_flag=cmditem["need_update"]

		print "<table id=\"mytable_1\" cellspacing=\"0\" >";
		print "<tr><th width=50 class=\"title_def\">%d</th><th class=\"title_def\"><a name=%s_1 href=#%s_2 >%s</a><br />%s</th><th class=\"title_def\" width=100 >%s<br />%s</th></tr>"%(
				id,cmdname,cmdname,cmdname,comment, cmdid,need_update_flag )
		print "</table>"		
		in_stru=cmditem["in_stru"];
		#打印请求
		#print header
		print "<table id=\"mytable\" cellspacing=\"0\" >";
		print	"<caption >请求包:</caption>";
		print "<tr><th>字段</th><th>类型</th><th>说明</th></tr>"
		#print in_stru;	
		len_in=len(in_stru);
		if (len_in==1 ):
			show_struct(in_stru[0]);
		elif (len_in==2 ):
			show_struct(in_stru[0],"");
			in_1=in_stru[1];
			print("<tr><td>%s</td><td>无</td><td>%s</td></tr>"%( in_1["struct_name"],
									gendoc_core.fd.getDesc(in_1["struct_name"] )))
			show_struct(in_1 ,">>>>&nbsp;" )
		elif (len_in>2):
			show_struct(in_stru[0],"");
			for i in range (1,len_in):
				in_item=in_stru[i];
				print("<tr><td>%s</td><td>无</td><td>%s</td></tr>"%(in_item["struct_name"],
									gendoc_core.fd.getDesc(in_item["struct_name"] )))

				show_struct(in_item,">>>>&nbsp;" )
		print "</table>";

		#打印返回包
		#print header
		print "<table id=\"mytable\" cellspacing=\"0\" >";
		print	"<caption >返回包:</caption>";
		print "<tr><th>字段</th><th>类型</th><th>说明</th></tr>"

		#print out_stru;	
		out_stru=cmditem["out_stru"];
		len_out=len(out_stru);
		if (len_out==1 ):
			show_struct(out_stru[0]);
		elif (len_out==2 ):
			show_struct(out_stru[0],"");
			out_1=out_stru[1];
			print("<tr><td>%s</td><td>无</td><td>%s</td></tr>"%( out_1["struct_name"],
									gendoc_core.fd.getDesc(out_1["struct_name"] )))
			show_struct(out_1 ,">>>>&nbsp;" )
		elif (len_out>2):
			show_struct(out_stru[0],"");
			for i in range (1,len_out):
				out_item=out_stru[i];
				print("<tr><td>%s</td><td>无</td><td>%s</td></tr>"%(out_item["struct_name"],
									gendoc_core.fd.getDesc(out_item["struct_name"] )))

				show_struct(out_item,">>>>&nbsp;" )
		print "</table>";

		#错误码
		print "<table id=\"mytable\" cellspacing=\"0\" >";
		print "<caption >错误码:</caption>";
		print "<tr><th>名字</th><th>数值</th><th>说明</th></tr>"
		err_list= cmditem["errlist"];
		for k in err_list .keys():
			err=err_list[k]
			v = gendoc_core .err_conf[err];
			print ( "<tr ><td >%s</td><td>%s</td><td>%s</td></tr>"%( err,v[0], v[1] ))
		print "</table>"		

		print "<br />"		

		id=id+1;



#打印命令的摘要
def print_cmd_def(db):
	#打印DB的头部
	print "<table id=\"mytable\" cellspacing=\"0\" >";
	print	"<caption >%s00:%s:%s</caption>"%(db["dbid"],db["dbname"], 
			gendoc_core.db_conf[str(eval( db["dbid"]))][2]   );
	print "<tr><th>命令号</th><th>名字</th><th>说明</th></tr>"
		
	cmdlist=db["cmdlist"];
	keylist=cmdlist.keys();
	keylist.sort();

	for k in keylist:
		cmditem=cmdlist[k]
		cmdname=cmditem["cmdname"];
		cmdid=cmditem["cmdid"];
		comment=gendoc_core.command_conf[k][2]
		print("<tr><td>%s</td><td><a name=%s_2 href=\"#%s_1\">%s</a></td><td>%s</td></tr>"%( cmdid,
					cmdname,cmdname,cmdname,comment))
		pass

	print "</table>";


def main(argv):
	print """
<style type="text/css">
/* CSS Document */

body {
font: normal 12px auto "Trebuchet MS", Verdana, Arial, Helvetica, sans-serif;
color: #4f6b72;
background: #E6EAE9;
}

a{
color: #c75f3e;
}

#mytable {
width: 700px;
padding: 0;
margin: 0;
}

#mytable_1 {
width: 750px;
padding: 0;
margin: 0;
}



caption {
padding: 0 0 5px 0;
width: 700px;
font: bold  15px "Trebuchet MS", Verdana, Arial, Helvetica, sans-serif;
text-align: left;
}

th {
font: bold 12px "Trebuchet MS", Verdana, Arial, Helvetica, sans-serif;
color: #4f6b72;
border-right: 1px solid #C1DAD7;
border-bottom: 1px solid #C1DAD7;
border-top: 1px solid #C1DAD7;
letter-spacing: 2px;
text-align: left;
padding: 6px 6px 6px 12px;
background: #CAE8EA url(images/bg_header.jpg) no-repeat;
}
th.title_def {
font: bold 12px "Trebuchet MS", Verdana, Arial, Helvetica, sans-serif;
color: #4f6b72;
border-right: 1px solid #FFD8C0;
border-bottom: 1px solid #FFD8C0;
border-top: 1px solid #FFD8C0 ;
letter-spacing: 2px;
text-align: left;
padding: 6px 6px 6px 12px;
background: #FFD8C0 url(images/bg_header.jpg) no-repeat;
}



th.nobg {
border-top: 0;
border-left: 0;
border-right: 1px solid #C1DAD7;
background: none;
}

td {
border-right: 1px solid #C1DAD7;
border-bottom: 1px solid #C1DAD7;
background: #fff;
width:200;
font-size:12px;
padding: 6px 6px 6px 12px;
color: #4f6b72;
}


td.alt {
background: #F5FAFA;
color: #797268;
}
td.spec {
border-left: 1px solid #C1DAD7;
border-top: 0;
background: #fff url(images/bullet1.gif) no-repeat;
font: bold 10px "Trebuchet MS", Verdana, Arial, Helvetica, sans-serif;
}


th.spec {
border-left: 1px solid #C1DAD7;
border-top: 0;
background: #fff url(images/bullet1.gif) no-repeat;
font: bold 10px "Trebuchet MS", Verdana, Arial, Helvetica, sans-serif;
}

th.specalt {
border-left: 1px solid #C1DAD7;
border-top: 0;
background: #f5fafa url(images/bullet2.gif) no-repeat;
font: bold 10px "Trebuchet MS", Verdana, Arial, Helvetica, sans-serif;
color: #797268;
}
</style>

<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<head>
<link id="themestyle" rel="stylesheet" type="text/css" href="./css.css">
<SCRIPT LANGUAGE="JavaScript" src="../pub/pub.js"></SCRIPT>
</head>
<body  >
<div align=center>
	"""
	db_cmd_def= gendoc_core .get_db_cmd_def(sys.argv );
	for db in db_cmd_def:
		print_cmd_def(db_cmd_def[db])
		print "<br />";


	print "<br />";
	print "<br />";
	for db in db_cmd_def:
		print_all_cmd_def(db_cmd_def[db])
		print "<br />";



	print "<br /> <hr />"
	err_sort_list={ }
	for k,v in gendoc_core.err_conf.items():
		err_sort_list[v[0]]=[k, v[1] ];

	err_keylist= err_sort_list.keys();
	err_keylist.sort();
	#错误列表 print 
	print "<table id=\"mytable\" cellspacing=\"0\" >";
	print	"<caption color=red >错误定义:</caption>";
	print "<tr><th>名字</th><th>数值</th><th>说明</th></tr>"
	

	for k in  err_keylist:
		print ( "<tr ><td >%s</td><td>%s</td><td>%s</td></tr>"%( k,
					err_sort_list[k][0], err_sort_list[k][1] ))

	print "</table>"		


	print "</div>"		
	print "</body></html>"		
	
if __name__ == '__main__':
	main(sys.argv)


