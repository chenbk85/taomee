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
	as_xml_name="./xml/${project_name}_as.xml"
	#备份原xml文件
	if [ -e $xml_name ]; then
        mv -f $xml_name ./bak/${project_name}.xml.$log_date
    fi

	wget -q "http://10.1.1.5/su/proto_test/download.php?project=$project_name" -O $xml_name 
	#wget  "http://10.1.1.5/su/proto_test/download_as.php?project=$project_name" -O $as_xml_name 
	$cpp_proto --struct_fix_name="$2" --plugin_name_list="$plugin_list" ./$xml_name -o ./proto/ 
	#$php_proto $xml_name -o ./php 
	#$php_proto $xml_name -o ./php  -b 2097152 
	#mv ./php/${project_name}_proto.php   ./php/su_${project_name}_proto.php  
	#$php_proto $xml_name -o ./php  
	$py_proto $xml_name -o  ./python

}

#创建备份文件夹
[  -d  ./bak ] || mkdir ./bak
[  -d  ./proto ] || mkdir ./proto
[  -d  ./python ] || mkdir ./python
[  -d  ./php ] || mkdir ./php
[  -d  ./xml ] || mkdir ./xml

#-----------------------------------

rm -rf ./proto/*

get_deal_xml  $1
#get_deal_xml  pea_switch

#---

