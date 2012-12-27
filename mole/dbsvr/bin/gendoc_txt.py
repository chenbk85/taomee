#!/usr/bin/python
# -*- coding: utf-8 -*-
FS="    "
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

	print ( "%s%s %s%s \033[1m\033[34m#%s\033[0m "%( fix , type, field_name,  count_def ,
			gendoc_core.fd.getDesc( struct_list+"."+field_name )	))

def show_struct(struct_def , fix="", struct_list=""):
	memberlist=struct_def["memberlist"];
	struct_name=struct_def["struct_name"];
	for mv in memberlist:
		fielditem=memberlist[mv]  ;
		print_filed(fielditem ,fix,struct_list+"."+struct_name );
		if fielditem["struct_def"] :
			show_struct( fielditem["struct_def"],fix+FS,
					struct_list+"."+struct_name )		
	

#打印命令的明细
def print_all_cmd_def(db,cmdid):
	#打印DB的头部
	cmdlist=db["cmdlist"];
	keylist=cmdlist.keys();
	keylist.sort();

	id=0
	for k in keylist:
		if(k<>cmdid):
			continue;
		cmditem=cmdlist[k]
		cmdname=cmditem["cmdname"];
		cmdid=cmditem["cmdid"];
		comment=gendoc_core.command_conf[k][2]
		need_update_flag=cmditem["need_update"]

		print "\033[1m\033[32m==============\033[0m 命令ID: \033[31m%s\033[0m  说明: \033[33m%s\033[0m  "%( cmdid,cmdname )

		in_stru=cmditem["in_stru"];

		print "\033[1m\033[31m请求包=========================================\033[0m"
		#打印请求
		#print in_stru;	
		len_in=len(in_stru);
		if (len_in==1 ):
			show_struct(in_stru[0],FS);
		elif (len_in==2 ):
			show_struct(in_stru[0],FS);
			in_1=in_stru[1];
			print(FS+"item_1")
			show_struct(in_1 ,FS+FS )
		elif (len_in>2):
			show_struct(in_stru[0],"");
			for i in range (1,len_in):
				in_item=in_stru[i];
				print( FS+"item_%d"%(i))
				show_struct(in_item,FS+FS )

		#打印返回包
		print "\033[1m\033[31m返回包========================================\033[0m"

		#print out_stru;	
		out_stru=cmditem["out_stru"];
		len_out=len(out_stru);
		if (len_out==1 ):
			show_struct(out_stru[0],FS);
		elif (len_out==2 ):
			show_struct(out_stru[0],FS);
			print(FS+"item_1")
			out_1=out_stru[1];
			show_struct(out_1 ,FS+FS )
		elif (len_out>2):
			show_struct(out_stru[0],"");
			for i in range (1,len_out):
				out_item=out_stru[i];
				print(FS+"item_%d"%(i))
				show_struct(out_item,FS+FS )

		#错误码
		print "\033[1m\033[31m错误码========================================\033[0m"
		err_list= cmditem["errlist"];
		for k in err_list .keys():
			err=err_list[k]
			v = gendoc_core .err_conf[err];
			print ( "%s%s%s%s%s\033[1m\033[34m#%s\033[0m "%( FS,v[0], FS,err,FS,v[1]  ))

		print "\033[1m\033[32m==============\033[0m 命令ID: \033[31m%s\033[0m  说明: \033[33m%s\033[0m  "%( cmdid,cmdname )



		id=id+1;

def main(argv):
	#sys.argv[1];
	cmdid=argv[1]
	print "\n\n\n"
	id="0x%04X"%(int( (eval("0x"+cmdid )))& 0xFEFF);

	db_cmd_def=gendoc_core.get_db_cmd_def("");
	for db in db_cmd_def:
		print_all_cmd_def(db_cmd_def[db],id )

	print "\n\n\n"
	
if __name__ == '__main__':
	main(sys.argv)


