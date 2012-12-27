#!/bin/bash
#不同的项目 要取不同的名字

project_name=$1

cd $(dirname $0 ) 
shell_dir=$(pwd)

work_dir=$2
export work_dir=$work_dir

src_dir="$work_dir/src"


tmp_dir="/tmp/tmp_${project_name}_$RANDOM"
out_file_name="$tmp_dir/$project_name.html"
[ -d $tmp_dir ] ||  mkdir $tmp_dir

cd $tmp_dir
#得到错误码列表
cat /usr/include/dbser/db_error_base.h  $src_dir/db_error.h  >./db_error.h

$shell_dir/gendoc.py >$out_file_name
# 发送到1.5 通过web 查看
scp -2 $out_file_name mole@10.1.1.5:/var/www/
rm -rf $tmp_dir

