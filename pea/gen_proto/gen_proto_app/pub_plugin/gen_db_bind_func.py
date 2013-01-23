#!/usr/bin/python
# -*- coding: utf-8 -*-
g_db_func_fd=0;
def init (output_dir,project_name):
	global  g_db_func_fd;
	g_db_func_fd=open( "%s/%s_db_src.cpp"%(output_dir,project_name ) , "w"  )

def deal( cmdid,cmdname, struct_in, struct_out, md5_tag,bind_bitmap,cmddesc ):
	p_in_str="";
	if(struct_in):
		 p_in_str="\t%s* p_in=P_IN;\n"%(struct_in);
	p_out_str="";
	if(struct_out):
		 p_out_str="\t%s* p_out=P_OUT;\n"%(struct_out);

	g_db_func_fd.write ("/* ----------------CODE FOR %s  ---------*/\n"%(cmdname))
	g_db_func_fd.write ("// ./Croute_func.cpp\n\n/*%s*/\nint Croute_func::%s(DEAL_FUNC_ARG){\n%s%s\n\n\n\treturn ret;\n}\n\n"%(cmddesc,cmdname, p_in_str,p_out_str ))
	pass

def fini( ):
	pass

