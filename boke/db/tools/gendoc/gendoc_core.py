#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
import os 
import optparse
import re 
import mypub 
def get_db_type (cmdid):
	if	cmdid&0x8000 >0:
		return (70+((cmdid&0x7E00)>>6)+((cmdid &0x00E0)>>5))
	else :
		return (cmdid>>9)	


def get_cmd_info(cmdid):
		cmd_info=command_conf[cmdid];
		return cmd_info;

work_dir=os.environ["work_dir"];

struct_conf={}
typedef_conf={}
command_conf={}
type_conf={
	"userid_t":"Uint32",
	"int32_t":"int32",
	"int":"int32",
	"uint32_t":"Uint32",
	"uint8_t":"Uint8",
	"char":"char",
	"uint16_t":"Uint16",
	"int16_t":"int16"
}

php_pack_conf={
	"userid_t":"L",
	"int32_t":"l",
	"int":"l",
	"uint32_t":"L",
	"uint8_t":"C",
	"int8_t":"c",
	"char":"a",
	"uint16_t":"S",
	"int16_t":"s",
	"short":"s",
	"double":"d",
	"float":"f"
}
def get_php_pack_conf(type):
	if (php_pack_conf.has_key(type)):	
		return php_pack_conf[type]
	else:
		return "L" 


err_conf={ }
func_err_conf={}
db_conf={}

#需要屏蔽的错误
cmd_nouse_err_conf={ }

def gen_cmd_nouse_err_conf():
	global cmd_nouse_err_conf 
	pf=open(work_dir+"/src/cmd_nouse_err.txt","r")				
	line=""
	for line in pf.readlines(): 
		items=line.split();
		if len(items)<2:
			continue	
		cmd_nouse_err_conf[items[0].lower()]={}
		for i in range( len(items)-1):
			cmd_nouse_err_conf[items[0].lower()][items[i+1]]=""





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
	pf=open("./db_error.h","r")				
	re_defcmd=re.compile(r'^#define.*_ERR.*')
	re_defdb=re.compile(r'.*=\s*GET_ROUTE.*')
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
			m=re.match("\s*(\w+)\s*=\s*GET_ROUTE\(\s*(\w+)\s*\)\s*,\s*//(.*)",line)
			if (m):
				db_conf[str(get_db_type((int(eval(m.group(2))))))]=(m.group(1),m.group(2),m.group(3) );
		
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
	pf=open(work_dir+"/src/proto.h","r")				
	re_defcmd=re.compile(r'^\s*#define.*_cmd')
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
					cmd_desc=last_line[2:];	
				else:
					cmd_desc="无";	
				if cmd_needupdate=="NEED_UPDATE":
					cmd_id="0x%04X"%(eval(cmd_id)|0x0100);
					command_conf[cmd_id]= (cmd_name,cmd_needupdate,cmd_desc);
				else:
					command_conf[cmd_id]= (cmd_name,cmd_needupdate,cmd_desc);
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
def cmd_sort_cmp(k1,k2):
	v_1=(eval(k1)|(0x0100));
	v_2=(eval(k2)|(0x0100));
	
	if v_1>v_2: return  1
	elif v_1==v_2 :  
		if k1>k2:return 1
		elif k1==k2:   return 0
		else :return -1

	else :return -1
	
def get_db_cmd_def(argv):
	gen_commond_def()
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
	keylist=command_conf.keys()
	keylist.sort(cmd_sort_cmp);

	#DB的命令定义
	db_cmd_def={}
	last_type=-1;
	db_type=-1;
	for k in keylist:
		cmd_info=command_conf[k];
		v=cmd_info[0]
		vl=v.lower()
		need_update_flag=cmd_info[1];
		db_type= get_db_type((int (eval(k))));    
		if (db_type!=last_type):	
			if db_conf.has_key(str(db_type)):
				db_cmd_def[db_type]={"dbid":db_conf[str(db_type)][1],
					"dbname":db_conf[str(db_type)][0],"cmdlist":{}}
			else: 
				db_cmd_def[db_type]={"dbid":"",
					"dbname":"","cmdlist":{}}
			last_type=db_type

		comment=cmd_info[2]
		cmd_id=k
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


def get_in_frmlist(stru_in):
	memberlist=stru_in["memberlist"];
	in_fmt="";in_str=",";
	for mv in memberlist:
		fielditem=memberlist[mv];
		if fielditem["struct_def"] :
			tmp_in_fmt,tmp_in_str=get_in_frmlist(fielditem["struct_def"]);
			in_fmt=in_fmt+tmp_in_fmt;	
			in_str=in_str[0:-1]+tmp_in_str+",";	
			pass
		else:
			type= fielditem["type"] ;
			field_name= fielditem["fieldname"];
			count= fielditem["count"];	
			if type=="char":
				in_fmt=in_fmt+"a"+count;	
				in_str=in_str+"$%s,"%field_name;	
			else:
				if count=='':
					in_fmt=in_fmt+get_php_pack_conf(type);	
					in_str=in_str+"$%s,"%field_name;	
				else: 
					count=int(count)
					for i in range(count):
						in_fmt=in_fmt+get_php_pack_conf(type);	
						in_str=in_str+"$%s_%d,"%(field_name,i);	


	return in_fmt,in_str[0:-1]
	pass

def get_out_frmlist(stru_out,end_str=""):
	memberlist=stru_out["memberlist"];
	out_fmt=""
	for mv in memberlist:
		fielditem=memberlist[mv];
		if fielditem["struct_def"] :
			out_fmt=out_fmt+get_out_frmlist(fielditem["struct_def"],end_str)+'/'
			pass
		else:
			type= fielditem["type"] ;
			field_name= fielditem["fieldname"];
			count= fielditem["count"];	
			if type=="char":
				out_fmt=out_fmt+"a%s%s%s/"%(count,field_name,end_str);	
			else:
				if count=='':
					out_fmt=out_fmt+get_php_pack_conf(type)+field_name+end_str+"/";	
	
	return out_fmt[0:-1];



def get_php_code(cmditem):
	cmdname=cmditem["cmdname"];
	cmdid=cmditem["cmdid"];
	need_update_flag=cmditem["need_update"]

	in_stru=cmditem["in_stru"];
	len_in=len(in_stru);
	use_in_flag=(len_in==1); 
	in_frt=""; in_str=""
	if (use_in_flag):
		in_frt,in_str=get_in_frmlist(in_stru[0]);

	out_str="";
	out_stru=cmditem["out_stru"];
	len_out=len(out_stru);
	use_out_list_flag=(len_out==2);

	if (len_out==1 ):
		out_str=get_out_frmlist(out_stru[0]);
	elif  (len_out>1 ) :
		out_str=get_out_frmlist(out_stru[0]);
		out_item_str=get_out_frmlist(out_stru[1],"_$i");

	php_code=""
	php_code=php_code+ "\n\tfunction %s ($userid %s){"%( cmdname,in_str )
	php_code=php_code+ "\n\t\t$cmdid='%s';"%cmdid[2:];
	if (use_in_flag): 
		php_code=php_code+ "\n\t\t$in_msg=pack('%s' %s );"%(in_frt,in_str);
	else: 
		php_code=php_code+ "\n\t\t$in_msg='';";

	if (not use_out_list_flag ):
		php_code=php_code+ "\n\t\t$out_msg='%s';"%out_str;
		php_code=php_code+ "\n\t\treturn  $this->send_cmd($cmdid,$userid,$in_msg,$out_msg);";
	else:
		php_code=php_code+"\n\n\t\t$sendbuf=$this->park($cmdid,$userid,$in_msg);";
		php_code=php_code+"\n\t\t$recvbuf=$this->sock->sendmsg($sendbuf);"
		php_code=php_code+"\n\t\t$fmt='%s';"%out_str;
		php_code=php_code+"\n\t\t$recvarr=$this->unpark($recvbuf,$fmt );"
		php_code=php_code+"\n\t\tif ($recvarr && $recvarr['result']!=SUCC){"
		php_code=php_code+"\n\t\t\treturn $recvarr;"
		php_code=php_code+"\n\t\t}\n"
		php_code=php_code+"\n\t\t//SUCC"
		php_code=php_code+"\n\t\t$recv_count=$recvarr['count'];"
		php_code=php_code+"\n\t\tfor ($i=0;$i<$recv_count;$i++){"
		php_code=php_code+"\n\t\t\t$fmt=$fmt .'/'. \"%s\";"%out_item_str
		php_code=php_code+"\n\t\t}"
		php_code=php_code+"\n\t\treturn $this->unpark( $recvbuf,$fmt);"

	php_code=php_code+ "\n\t}"
	return php_code

