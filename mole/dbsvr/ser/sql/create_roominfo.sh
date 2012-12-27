#!/bin/bash
#
#保存小屋信息
#
host="10.1.1.5"
user="swp"
pass="swp"
tmp_file=".tmp.sql"

for ((i=0; i<10; i++))
do
    for ((p=0; p<10; p++))
    do
        # 创建sql脚本
        cat <<EOF >$tmp_file
        use DB_ROOMINFO_0;
        
        drop table if exists t_roominfo_$i$p;
        create table t_roominfo_$i$p\p
        (
            room_id int(10) unsigned not null default 0
            , room_flag tinyint unsigned not null default 0
            , room_hot int(10) unsigned not null default 0
            , room_flower int(10) unsigned not null default 0
            , room_bug int(10) unsigned not null default 0
            , room_puglist varbinary(1452) not null default 0x0000
            , room_neighbor varbinary(802) not null default 0x0000
            , unique index(room_id)
        );
EOF
        
        # 执行sql脚本
        echo 'mysql -h '$host' -u '$user' -p'$pass;
        mysql -h $host -u $user -p$pass < $tmp_file
    done
done

