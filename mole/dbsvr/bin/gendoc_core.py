#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
import os 
import optparse
import re 
import mypub 

work_dir="/home/tommychen/DB/"

struct_conf={}
typedef_conf={}
command_conf={}
type_conf={
	"userid_t":"Uint32",
	"int32_t":"int32",
	"uint32_t":"Uint32",
	"uint8_t":"Uint8",
	"char":"char",
	"uint16_t":"Uint16"
}
fd=mypub.fielddesc();
fd.init( work_dir+"/include/field_desc.txt");

err_conf={ }

func_err_conf={}
db_conf={}

#需要屏蔽的错误
cmd_nouse_err_conf={ }

def gen_cmd_nouse_err_conf():
	global cmd_nouse_err_conf 
	pf=open(work_dir+"/bin/cmd_nouse_err.txt","r")				
	line=""
	for line in pf.readlines(): 
		items=line.split();
		if len(items)<2:
			continue	
		cmd_nouse_err_conf[items[0]]={}
		for i in range( len(items)-1):
			cmd_nouse_err_conf[items[0]][items[i+1]]=""





def get_err_from_func(func_name):
	global func_err_conf
	err_list={}
	ret_list={}
	if func_err_conf.has_key(func_name):
		err_list=func_err_conf[func_name]
		for k in err_list.keys():
			if re.match(r".*_ERR", k):
				ret_list.update({k:1});
			else:
				ret=get_err_from_func(k)
				ret_list.update(ret);
	return ret_list;

def gen_func_err_conf():
	global func_err_conf

	pf=open("./func_source.cpp","r")				
	cur_func=""
	cur_class=""
	for line in pf.readlines(): 
		m=re.match("^\s*int\s+(\w+)\s*::\s*(\w+)\s*\(.*",line)
		if (m) :
			cur_class=m.group(1)
			cur_func=m.group(1) + "."+m.group(2)
			func_err_conf[cur_func]={ }

		#得到调用的函数列表
		m0=re.match(r"^\s*(int)*\s+ret\s*=\s*this->\s*\b(\w+)\b\s*\.\s*\b(\w+)\b.*",line)
		if (m0):
			if (cur_func != "" ):
				func_err_conf[cur_func]["C"+m0.group(2)+"."+m0.group(3)]=1; 
		else:
			m1=re.match(r"\s*(int)*\s+ret\s*=\s*this->\s*\b(\w+)\b.*",line)
			if (not m1):
				m1=re.match(r"\s*(int)*\s+return\s+this->\s*\b(\w+)\b.*",line)
	
			if (m1) :
				if (cur_func != "" ):
					func_err_conf[cur_func][cur_class+'.'+m1.group(2)]=1; 
	
			#得到错误码
			m2=re.match(r'.*\s*\b(\w\w\w\w+_ERR)\b.*',line)
			m3=re.match(".*_LOG.*",line)
			m4=re.match(".*==.*",line)
			if (m2 and (not m3) and (not m4) ) :
				if (cur_func != "" ):
					func_err_conf[cur_func][m2.group(1)]=1; 
	#print func_err_conf;



def gen_err_def():
	global err_conf
	global db_conf
	global   work_dir
	pf=open(work_dir+"/include/common.h","r")				
	re_defcmd=re.compile(r'^#define.*_ERR.*')
	re_defdb=re.compile(r'.*=\s*SET_ROUTE.*')
	last_line=""
	line=""

	for line in pf.readlines(): 
		if (re_defcmd.findall (line)):
			m=re.match("#define\s+(\w+)\s+(\w+)\s*",line)
			if (m):
				err_name=m.group(1);
				err_value=str(m.group(2));
				if last_line[0]=="/":
					err_conf[err_name]= (err_value ,last_line[2:]);
				else:
					err_conf[err_name]= (err_value ,"无");
		last_line=line

		if (re_defdb.findall (line)):
			#print "12"
			#m=re.match("\s*(\w+)\s*=\s*SET_ROUTE\(\s*(\w+)\s*\)//(\w*)",line)
			m=re.match("\s*(\w+)\s*=\s*SET_ROUTE\(\s*(\w+)\s*\)\s*,\s*//(.*)",line)
			if (m):
				#print '%s, %s, %s' %(m.group(1),m.group(2),m.group(3))
				db_conf[str(int(eval(m.group(2))))]=(m.group(1),m.group(2),m.group(3) );
		
def get_type_doc(type):
	global type_conf
	if (type_conf.has_key(type)):	
		return type_conf[type]
	else:
		return type

def get_struct_def(struct_name ):
	global struct_conf;
	global typedef_conf;
	global type_conf;
	struct_def={"struct_name":struct_name, "memberlist":{} }
	memberlist=struct_conf[typedef_conf[struct_name]]
	i=0;
	for mv in memberlist:
		#print_filed(mv[1],fix,struct_list+"."+struct_name );
		field_def=mv[1];
		m=re.match(r"^\s*(\w+)\s+(\w+)\s*\[*(\w+)*\]*\s*",field_def)
		field_type=m.group(1);
		field_name=m.group(2);
		count_def=""	
		if m.group(3):
			count_def=m.group(3)
		struct_def["memberlist"][i]={"fieldname":field_name,"type":field_type,
			"count":count_def,"struct_def":{} }

		if typedef_conf.has_key(field_type):
			struct_def["memberlist"][i]["struct_def"]=get_struct_def(field_type)		
		i=i+1;
	return struct_def;
	

def gen_commond_def():
	pf=open(work_dir+"/include/proto.h","r")				
	re_defcmd=re.compile(r'^\s*#define.*_CMD')
	line=""
	global command_conf
	for line in pf.readlines(): 
		if (re_defcmd.findall (line)):
			#print line
			m=re.match("^\s*#define\s+(\w+)\s*\(\s*(\w+)*[ \t|]*(\w+)*\s*\)\s*",line)
			if (m):
				#print line
				cmd_name=m.group(1)[:-4];
				cmd_id=str(m.group(2));
				cmd_needupdate=m.group(3);
				if last_line[0]=="/":
					command_conf[cmd_id]= (cmd_name,cmd_needupdate,last_line[2:]);
				else:
					command_conf[cmd_id]= (cmd_name,cmd_needupdate,"无");
		last_line=line
	#print command_conf;
		#pass
		##print( "%s=%s"%(k,struct_conf[v]) )

def gen_struct_def():
	pf=open("tags","r")				
	line=""
	global struct_conf
	global typedef_conf

	for line in pf.readlines(): 
		m1=re.match("^(\w+)[\t ]+\/\^[\t ]*(.*);\$\/;\"[\t ]+m[\t ]+struct:(\w+)",line)
		m2=re.match("^(\w+)[\t ]+\/\^[\t ]*.*;\$\/;\"[\t ]+t[\t ]+typeref:struct:(\w+)",line)
		m3=re.match("^(\w+)[\t ]+\/\^[\t ]*typedef[\t ]+(.*)[\t ]+\w+[\t ]*;\$\/;\"[\t ]+t",line)
		#get_server_info_out	/^struct get_server_info_out{$/;"	s
		m4=re.match("^(\w+)[\t ]+\/\^[\t ]*struct[\t ]+.*\$\/;\"[\t ]+s",line)

		if (m1):
			struct_name=m1.group(3);
			member_name=m1.group(1);
			member_def=m1.group(2);
			if struct_conf.has_key(struct_name):
			 	struct_conf[struct_name].append ([member_name,member_def]);
				pass
			else:
			 	struct_conf[struct_name]=[[member_name,member_def]];
				pass
			pass
		elif(m2):
			def_name=""
			struct_name=m2.group(1);
			def_name=m2.group(2);
			while def_name.isupper():
				def_name=typedef_conf[def_name]
			typedef_conf[struct_name]=def_name;
		elif(m3):
			def_name=""
			struct_name=m3.group(1);
			def_name=m3.group(2);
			while def_name<>typedef_conf[def_name]:
				def_name=typedef_conf[def_name]
			typedef_conf[struct_name]=def_name;
			pass
		elif(m4):
			def_name=""
			struct_name=m4.group(1);
			typedef_conf[struct_name]=struct_name;
			pass


	#print( "xxxx" )
	#for k,v in typedef_conf.items():
		#print( "%s=%s"%(k,struct_conf[v]) )
def get_db_cmd_def(argv):
	gen_commond_def()
	gen_struct_def()
	gen_err_def()
	gen_func_err_conf()
	gen_cmd_nouse_err_conf()

	global struct_conf
	global typedef_conf
	global command_conf
	global err_conf
	global func_err_conf
	global db_conf
	global cmd_nouse_err_conf 

	#print func_err_conf;
	#print struct_conf[typedef_conf["ATTIRE_NOUSED_ITEM"]]

	id=0;	
	keylist=command_conf.keys();
	keylist.sort();

	#DB的命令定义
	db_cmd_def={}
	last_type=-1;
	db_type=-1;
	for k in keylist:
		v=command_conf[k][0]
		vl=v.lower()
		need_update_flag=command_conf[k][1];
		db_type= (int (eval(k)) & 0xFF00)>>8;    
		#print 'db_type:==== %x' %(db_type)
		if (db_type!=last_type):	
			db_cmd_def[db_type]={"dbid":db_conf[str(db_type)][1],
				"dbname":db_conf[str(db_type)][0],"cmdlist":{}}
			#print	"<caption >%s00:%s:%s</caption>"%(db_conf[str(db_type)][1],db_conf[str(db_type)][0],db_conf[str(db_type)][2]   );
			last_type=db_type
		comment=command_conf[k][2]

		if (need_update_flag):
			cmd_id='0x%04X'%(int (eval(k ) )+ 0x0100 )
		else:
			cmd_id='0x%04X'%(int (eval(k ) ))

		db_cmd_def[db_type]["cmdlist"][k]={"cmdid":cmd_id, "cmdname":v, 
			"need_update":need_update_flag,
			"in_stru":{},"out_stru":{},"errlist":{} }

		str_in=vl+"_in";	
		str_in_header=vl+"_in_header";	
		str_in_item=vl+"_in_item";	
		str_out=vl+"_out";	
		str_out_header=vl+"_out_header";	
		str_out_item=vl+"_out_item";	
		str_func="Croute_func."+vl;	



		#导入IN的数据						
		in_stru=db_cmd_def[db_type]["cmdlist"][k]["in_stru"];
		if (typedef_conf.has_key(str_in)):
			in_stru[0]=get_struct_def(str_in)
		elif (typedef_conf.has_key(str_in_header)):
			#列表模式
			in_stru[0]=get_struct_def(str_in_header)
			if (typedef_conf.has_key(str_in_item) ):
				in_stru[1]=get_struct_def(str_in_item)
			else:
				for i in range (1,5):
					str_item_id=str_in_item + "_" + str(i)
					if ( typedef_conf.has_key(str_item_id ) ): 
						in_stru[i]=get_struct_def(str_item_id)

		#导入OUT的数据						
		out_stru=db_cmd_def[db_type]["cmdlist"][k]["out_stru"];
		if (typedef_conf.has_key(str_out)):
			out_stru[0]=get_struct_def(str_out)
		elif (typedef_conf.has_key(str_out_header)):
			out_stru[0]=get_struct_def(str_out_header)
			if (typedef_conf.has_key(str_out_item) ):
				out_stru[1]=get_struct_def(str_out_item)
			else:
				for i in range (1,5):
					str_item_id=str_out_item + "_" + str(i)
					if ( typedef_conf.has_key(str_item_id ) ): 
						out_stru[i]=get_struct_def(str_item_id)
	
		#错误码		
		err_list=get_err_from_func(str_func)

		i=0;
		for err in err_list.keys():
			show_flag=False
			if  cmd_nouse_err_conf.has_key(v):
				if not cmd_nouse_err_conf[v].has_key(err):
					show_flag=True
			else:
				show_flag=True
			if show_flag:
				db_cmd_def[db_type]["cmdlist"][k]["errlist"][i]=err
				i=i+1;



	#db_cmd_def["errdef"]={}
	#err_sort_list={ }
	#for k,v in err_conf.items():
		#err_sort_list[v[0]]=[k, v[1] ];
	#err_keylist=err_sort_list.keys();
	#err_keylist.sort();
	#i=0;
	#for k in  err_keylist:
		#db_cmd_def["errdef"][i]=k
		#i=i+1

	return db_cmd_def;
if __name__ == '__main__':
	print get_db_cmd_def(sys.argv)

