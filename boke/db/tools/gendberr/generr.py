#!/usr/bin/python
# -*- coding: utf-8 -*-
import re
err_conf={ }
need_cmd_conf={ }
func_err_conf={}
add_err_conf={ }
sub_err_conf={ }

def gen_neen_cmd_list():
	global need_cmd_conf
	pf=open("./need_gen_cmd.txt","r")				
	for line in  pf.readlines():
		arr=line.split()
		if len(arr)==2:
			need_cmd_conf[arr[1]]=arr[0]
			pass


def gen_change_err_conf ():
	global add_err_conf,sub_err_conf
	pf=open("./err_change.txt.tmp","r")				
	for line in  pf.readlines():
		arr=line.split()
		if len(arr)>1:
			item_name=arr[0]	
			for i in range(len(arr)-1):
				opt_item=arr[i+1];
				err_name=opt_item[1:];
				if (not  err_conf.has_key(err_name) ):#没有找到
					continue;
				#errno=err_conf[err_name];


				if (opt_item[0]=="+"):
					if not add_err_conf.has_key(item_name): add_err_conf[item_name]=[]
					add_err_conf[item_name].append(err_name);
				elif (opt_item[0]=="-"):
					if not sub_err_conf.has_key(item_name): sub_err_conf[item_name]=[]
					sub_err_conf[item_name].append(err_name);
		pass


def gen_err_def():
	global err_conf
	pf=open("./db_error.h","r")				
	re_defcmd=re.compile(r'^#define.*_ERR.*')
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

g_default_function_err={
	"set_int_value":"USER_ID_NOFIND_ERR",
	"get_int_value":"USER_ID_NOFIND_ERR",
	"set_int_value_bit":"USER_ID_NOFIND_ERR"
}

def get_err_from_func(func_name):
	global func_err_conf,g_default_function_err
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
	else:#没有找到,看看是不是调用get_int .set_int 
		f_name=func_name.split(".")[1];
		if g_default_function_err.has_key(f_name):
			ret_list.update({g_default_function_err[f_name] :1});
		pass
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
		if (m0 ):
			if (cur_func != "" ):
				func_err_conf[cur_func]["C"+m0.group(2)+"."+m0.group(3)]=1; 
		else:
			m1=re.match(r"\s*(int)*\s+ret\s*=\s*this->\s*\b(\w+)\b.*",line)
			if (not m1):
				m1=re.match(r"\s*(int)*\s+return\s+this->\s*\b(\w+)\b.*",line)
	
			m2=re.match(r'.*\s*\b(\w\w\w\w+_ERR)\b.*',line)
			if (m1 and not m2  ) :
				if (cur_func != "" ):
					func_err_conf[cur_func][cur_class+'.'+m1.group(2)]=1; 
	
			#得到错误码
			m3=re.match(".*_LOG\b.*",line)
			m4=re.match(".*==.*",line)

			if (m2 and (not m3) and (not m4) ) :
				if (cur_func != "" ):
					func_err_conf[cur_func][m2.group(1)]=1; 
	#print func_err_conf;

#-------------------------------------------------

gen_neen_cmd_list()
gen_err_def()
gen_change_err_conf()
gen_func_err_conf()
for err_name in err_conf : 
	item=err_conf[err_name];
import xml.dom.minidom
impl = xml.dom.minidom.getDOMImplementation()
dom = impl.createDocument(None, 'root', None)
root = dom.documentElement

cmd_err_item = dom.createElement('cmd_err_list')
root.appendChild(cmd_err_item)
errlist_item = dom.createElement('err_list')
root.appendChild(errlist_item )


for  func_name  in  func_err_conf: 
	class_name=func_name.split(".")[0];
	f_name=func_name.split(".")[1];
	if class_name=="Croute_func" and  need_cmd_conf.has_key(f_name) :
		err_list=get_err_from_func(func_name )
		#处理修正
		if (add_err_conf.has_key(f_name)):
			for err_name in add_err_conf[f_name]:
				err_list[err_name]=[ err_conf[err_name] ];

		#处理修正
		if (sub_err_conf.has_key(f_name)):
			for err_name in sub_err_conf[f_name]:
				if err_list.has_key(err_name ):
					err_list.pop(err_name);


		for err_name in err_list: 
			item = dom.createElement('item')
			item.setAttribute('errno', err_conf[err_name][0] )
			item.setAttribute('cmdid', need_cmd_conf[f_name])
			cmd_err_item.appendChild(item)

for   errname in err_conf  : 
	item = dom.createElement('item')
	item.setAttribute('errno', err_conf[errname][0] )
	item.setAttribute('err_desc', err_conf[errname][1].strip() )
	item.setAttribute('err_name', errname)
	errlist_item.appendChild(item)

print  dom.toxml()

