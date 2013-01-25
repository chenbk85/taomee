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
plugin_list="./gen_proto_app/pub_plugin/gen_db_bind_func.py,./plugin/gen_online.py"

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

get_deal_xml session_ser
get_deal_xml mole2_btl 
get_deal_xml mole2_db 
get_deal_xml mole2_online
get_deal_xml bus_server 
get_deal_xml mole2_home
get_deal_xml mole2_switch

#------------ online  -----------
# cp  to proto
cplist=" \
	mole2.cpp\
	mole2_db_bind_for_online.h\
	mole2_online_func_def.h\
	mole2_db.cpp\
	mole2_db_enum.h\
	mole2_db.h\
	mole2.h\
	mole2_online_bind_for_cli_request.h\
	mole2_online_bind.h\
	mole2_online.cpp\
	mole2_online_enum.h\
	mole2_online.h\
	mole2_online_online_src.cpp\
	mole2_home_enum.h\
	mole2_home.h\
	mole2_home.cpp\
	mole2_home_bind_for_online.h\
	mole2_switch_enum.h\
	mole2_switch.h\
	mole2_switch.cpp\
	mole2_switch_bind_for_switch.h\
	mole2_switch_bind_for_cli_request.h\
	bus.cpp\
	bus_server_bind_for_online.h\
	mole2_online_func_def.h\
	bus_server.cpp\
	bus_server_enum.h\
	bus_server.h\
	bus.h\
	
"

for  cpfile in $cplist 
do
	cp ./proto/$cpfile ../proto/
done

exit
