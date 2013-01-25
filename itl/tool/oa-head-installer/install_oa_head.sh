#!/bin/bash
#the project list.separated by ',' or  ';' or ':'.the project name can be chinese name or english alias
#when you using the chinese project name please do not worry,we will convert it to english alias
#要安装的head对应的project
project_list=''
#安装路径
install_path='/opt/taomee/oa/'
#数据库ip
db_host='10.1.1.27'
#数据库用户名
db_user='root'
#数据库
db='db_itl'
#数据库密码
db_pass='ta0mee'

if [ ! -e $install_path ]
then
    mkdir -m 0755 -p $install_path
else
    chmod 0755 $install_path
fi

cd ./bin  && ./oa_head_install -i$install_path -h$db_host -d$db -u$db_user -p$db_pass -n$project_list
