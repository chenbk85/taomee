#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

alter_user_ex_table_sql() {
cat <<EOF >$tmp_file

alter TABLE t_user_pet_$1  
    add logic_birthday integer unsigned NOT NULL DEFAULT '0',
    add fire_skill_bitflag integer unsigned NOT NULL DEFAULT '0',
    add water_skill_bitflag integer unsigned NOT NULL DEFAULT '0',
    add wood_skill_bitflag integer unsigned NOT NULL DEFAULT '0';

update t_user_pet_$1 set logic_birthday=birthday;

EOF
}

db_index=0
end_index=100

while [ $db_index -lt $end_index ] ; do
    dbx=`printf "%02d" $db_index`
    echo $dbx
    table_index=0
    while [ $table_index -lt 10 ] ; do
        tbx=`printf "%d" $table_index`
        alter_user_ex_table_sql $tbx
        cat $tmp_file | mysql -u $user --password="$password" -h $host "USER_$dbx"
        let "table_index+=1"
    done
    let "db_index+=1"
done

