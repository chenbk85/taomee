#!/bin/bash
cd `dirname $0`
if [ "$1" == "clean"  ];then
	rm -rf  bak xml python php proto
	exit;
fi

bin_dir="./gen_proto_app/bin/"
cpp_proto="$bin_dir/cpp_proto"
py_proto="$bin_dir/py_proto"
php_proto="$bin_dir/php_proto"
plugin_list="./gen_proto_app/pub_plugin/gen_db_bind_func.py,./plugin/gen_online_db.py, ./plugin/gen_online_cli.py"

#$1: 项目名 $2:cpp中结构体的前缀配置
function get_deal_xml  {
	log_date=$(date +%Y%m%d_%H%M%S )
	project_name=$1
	xml_name="./xml/$project_name.xml"
	#备份原xml文件
	mv $xml_name ./bak/${project_name}.xml.$log_date 

	wget  "http://10.1.1.5/su/proto_test/download.php?project=$project_name" -O $xml_name 
	$cpp_proto --struct_fix_name="$2" --plugin_name_list="$plugin_list" ./$xml_name -o ./proto/ 
	$php_proto $xml_name -o ./php 
	$py_proto $xml_name -o  ./python

}

#创建备份文件夹
[  -d  ./bak ] || mkdir ./bak
[  -d  ./proto ] || mkdir ./proto
[  -d  ./python ] || mkdir ./python
[  -d  ./php ] || mkdir ./php
[  -d  ./xml ] || mkdir ./xml

#-----------------------------------

#get_deal_xml pop_db 
#get_deal_xml session_ser
#get_deal_xml seer2_btl 
#get_deal_xml seer2_db 
#get_deal_xml seer2_online
get_deal_xml pop_online 
get_deal_xml pop_db
get_deal_xml pop_switch
get_deal_xml main_login 
#get_deal_xml pop_switch

#------------ online  -----------
# cp  to proto
cplist=" \
	pop.cpp\
	pop_db_bind_for_online.h\
	pop_db_online_db_src.cpp\
	pop_online_online_cli_src.cpp\
	pop_online_func_def.h\
	pop_db.cpp\
	pop_db_enum.h\
	pop_db.h\
	pop.h\
	pop_online_bind_for_cli_request.h\
	pop_online_bind.h\
	pop_online.cpp\
	pop_online_enum.h\
	pop_online.h\
	pop_switch.h\
	pop_switch.cpp\
	pop_switch_bind_for_online.h\
	pop_switch_enum.h\
	main.cpp\
	main.h\
	main_login.cpp\
	main_login.h\
	main_login_bind_for_add_session.h\
	main_login_bind_for_check_session.h\
	main_login_bind_get_gameflag.h\
	main_login_enum.h\
"

for  cpfile in $cplist 
do
	cp ./proto/$cpfile ../src/proto/
done

exit

#-------------- db --------------------
# cp  to proto
cplist=" \
    seer2_db.h\
    seer2_db.cpp\
    seer2_db_bind.h\
    seer2_db_enum.h\
    seer2_db_func_def.h\
    seer2_db_db_src.cpp\
"

for  cpfile in $cplist 
do
   cp ./proto/$cpfile ~/seer2_db/proto
done


#-------------- switch --------------------
# cp  to proto
cplist=" \
	seer2_switch.h \
	seer2_switch.cpp \
	seer2_switch_bind.h \
	seer2_switch_enum.h \
	seer2_switch_func_def.h \
	seer2_db_enum.h\
"

for  cpfile in $cplist 
do
   cp ./proto/$cpfile ~/seer2_switch/proto
done


#-------------- btl --------------------
# cp  to proto
cplist=" \
	pop_online.h \
	pop_online.cpp \
	pop_online_bind_for_btl.h \
	pop_online_enum.h \
	pop_online_func_def.h \
	pop_online_online_src.cpp \
"

for  cpfile in $cplist 
do
   cp ./proto/$cpfile ~/seer2_btl/proto
done

