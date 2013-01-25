#!/bin/bash
cd `dirname $0`
if [ "$1" == "clean"  ];then
    cd `dirname $0`
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
	#$php_proto $xml_name -o ./php 
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

get_deal_xml  cdn_rate

#------------ online  -----------
# cp  to proto
cplist=" \
	cdn.cpp\
	cdn_rate_bind.h\
	cdn_rate.cpp\
	cdn_rate_db_src.cpp\
	cdn_rate_enum.h\
	cdn_rate_func_def.h\
	cdn_rate.h\
	cdn.h\
"

for  cpfile in $cplist 
do
	cp ./proto/$cpfile ../src/proto/
done

exit
