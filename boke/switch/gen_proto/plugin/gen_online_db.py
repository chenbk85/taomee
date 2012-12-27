#!/usr/bin/python
# -*- coding: utf-8 -*-
g_db_func_fd=0;
def init (output_dir,project_name):
	global  g_db_func_fd;
	g_db_func_fd=open( "%s/%s_online_db_src.cpp"%(output_dir,project_name ) , "w"  )

def deal( cmdid,cmdname, struct_in, struct_out, md5_tag,bind_bitmap,cmddesc ):
	p_out_str="";
	if(struct_out):
		 p_out_str="\t%s * p_out = P_OUT;\n"%(struct_out);

	g_db_func_fd.write ("\n/*%s*/\nvoid %s( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)\n{\n\tif (ret!=SUCC){\n\t\treturn ;\n\t}\n%s\t\n\n\n}\n\n"%(cmddesc,cmdname, p_out_str ))
	pass

def fini( ):
	pass

