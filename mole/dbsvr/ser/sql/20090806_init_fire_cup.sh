#!/bin/sh
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"

update_fire_cup_table(){
cat << EOF >$tmp_file
	insert into SYSARG_DB.t_sys_arg values($1, 0);
EOF
}


table_index=110

while [ $table_index -lt 131 ]; do
		update_fire_cup_table $table_index
		cat $tmp_file | mysql -u $user --password="$password" -h $host "SYSARG_DB"
		table_index=$(( $table_index+1 ))
done

#打气设置(这个部分可能有所不同)
update_type_value() {
cat << EOF >$tmp_file
	update SYSARG_DB.t_sys_arg set value=1 where type=110;
	update SYSARG_DB.t_sys_arg set value=1 where type=111;
	update SYSARG_DB.t_sys_arg set value=1 where type=112;
	update SYSARG_DB.t_sys_arg set value=2 where type=113;
	update SYSARG_DB.t_sys_arg set value=1 where type=114;
	update SYSARG_DB.t_sys_arg set value=4 where type=130; 
	update SYSARG_DB.t_sys_arg set value=4 where type=130; 
	update SYSARG_DB.t_sys_arg set value=1 where type=120; 
	update SYSARG_DB.t_sys_arg set value=2 where type=121; 
	update SYSARG_DB.t_sys_arg set value=3 where type=122; 
	update SYSARG_DB.t_sys_arg set value=4 where type=123; 
	update SYSARG_DB.t_sys_arg set value=5 where type=124; 
	update SYSARG_DB.t_sys_arg set value=1 where type=125; 
	update SYSARG_DB.t_sys_arg set value=2 where type=126; 
	update SYSARG_DB.t_sys_arg set value=4 where type=127; 
	update SYSARG_DB.t_sys_arg set value=8 where type=128; 
	update SYSARG_DB.t_sys_arg set value=16 where type=129; 
EOF
}

update_type_value
cat $tmp_file | mysql -u $user --password="$password"

insert_team_table(){
cat << EOF >$tmp_file
	insert into SYSARG_DB.t_sysarg_team_medal values($1, $2, 0, 0, 0, 0);
EOF
}

date=20090807
team=1
while [ $date -lt 20090821 ]; do
	team=1
	while [ $team -lt 6 ]; do
		insert_team_table $date $team
		cat $tmp_file | mysql -u $user --password="$password" -h $host "SYSARG_DB"
		team=$(( $team+1 ))
	done
	date=$(($date+1))
done


