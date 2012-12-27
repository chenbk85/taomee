#!/bin/bash
#不同的项目 要取不同的名字
project_name="pop_db"
#db程序所在位置
work_dir="/home/jim/pop/db/trunk/"
#----------------------------------------------------
#----------------------------------------------------
#----------------------------------------------------
#----------------------------------------------------
cd $(dirname $0 ) 
shell_dir=$(pwd)

src_dir="$work_dir/src"
output_file="./${project_name}_err.xml"


tmp_dir="/tmp/tmp_dir_$RANDOM"
out_file_name="$tmp_dir/$project_name.html"
[ -d $tmp_dir ] ||  mkdir $tmp_dir

cd $tmp_dir

#得到错误码列表
cat /usr/include/dbser/db_error_base.h  $src_dir/db_error.h  >./db_error.h

cat $src_dir/*.cpp > ./func_source.cpp
sed -e '/\s*#/d'  -e 's/^\s*//g' -e  's/+\s*/+/g' -e 's/-\s*/-/g'  $shell_dir/err_change.txt >./err_change.txt.tmp
awk -F'\(|,' '{print $2," ",$3}'   $src_dir/proto/${project_name}_bind.h  >./need_gen_cmd.txt

$shell_dir/generr.py > $output_file
# 发送到1.5 通过web 查看
scp -2  $output_file mole@10.1.1.5:/var/www/
rm -rf $tmp_dir


