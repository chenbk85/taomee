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
	$php_proto $xml_name -o ./php  -b 2097152 
	mv ./php/${project_name}_proto.php   ./php/su_${project_name}_proto.php  
	$php_proto $xml_name -o ./php  
	$py_proto $xml_name -o  ./python

}



#创建备份文件夹
[  -d  ./bak ] || mkdir ./bak
[  -d  ./proto ] || mkdir ./proto
[  -d  ./python ] || mkdir ./python
[  -d  ./php ] || mkdir ./php
[  -d  ./xml ] || mkdir ./xml

get_deal_xml mole2_db 



# cp  to proto
cplist=" \
	mole2_db_bind.h\
	mole2_db_db_src.cpp\
	mole2_db_enum.h\
	mole2_db_func_def.h\
	mole2_db.cpp\
	mole2_db.h\
	mole2.h\
	mole2.cpp\
"

for  cpfile in $cplist 
do
	cp ./proto/$cpfile ../src/proto/
done

cp ./php/mole2_db_proto.php ../proto -f
#scp ./php/su_mole2_db_proto.php jim@10.1.1.5:/var/www/app/
#cd ~/mole2_db/proto/

#svn ci mole2_db_proto.php -m "$(date +%Y%m%d_%H%M%S )"
