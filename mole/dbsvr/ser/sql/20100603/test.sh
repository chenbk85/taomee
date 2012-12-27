#!/bin/bash
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
alter_user_ex_table_sql() {
cat <<EOF >$tmp_file
	select userid from t_sysarg_db_sports_gamescore where gameid = $1 order by score desc limit 9;
EOF
}

gameid_index=1

	while [ $gameid_index -lt 6 ] ; do
		alter_user_ex_table_sql $gameid_index 
		cat $tmp_file | mysql -u $user --password="$password" -h $host "SYSARG_DB" >> tmp_20100603 
		let "gameid_index+=1"
	done


sed -i -e '/userid/d' tmp_20100603

sort -k1 tmp_20100603 | uniq -c > tmp_20100603 

awk '{
	print "("$2",",$1", 6)"
}' tmp_20100603 > tmp_201006036

sql=`sed -e ':a;N;s#\n#,#g;ta' tmp_201006036`

sql="insert into t_sysarg_db_sports_gamescore(userid, score, gameid) values$sql;"


echo $sql | mysql -u $user --password="$password" -h $host "SYSARG_DB"

rm -f tmp_20100603 tmp_20100603 tmp_sql
