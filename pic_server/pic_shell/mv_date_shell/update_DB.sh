#!/bin/sh
for x in `seq 0 99`;
do
	if [ $x -lt 10 ];then
		dbid="0$x"
	else
		dbid="$x"
	fi

#for xx in `seq 00 99`;
#do
#	if [ $xx -lt 10 ];then
#		tbid="0$xx"
#	else
#		tbid="$xx"
#	fi
#	sed -i "1ause sns_user_$dbid" /opt/taomee/bus_backup/sns_user_$dbid.user_album_$tbid.sql
#	sed -i "1ause sns_user_$dbid" /opt/taomee/bus_backup/sns_user_$dbid.user_photo_$tbid.sql
#	mysql -u root -pta0mee <<- EOF
#	SOURCE /home/test/bus_backup/sns_user_$dbid.user_album_$tbid.sql;	
#	SOURCE /home/test/bus_backup/sns_user_$dbid.user_album_$tbid.sql; 
#	mysql -u root -pta0mee <<- EOF
#	create database if not exists sns_app_$dbid
#	create database if not exists sns_user_$dbid
#	EOF
#	mysqladmin -uroot -pta0mee drop  sns_app_$dbid
#	mysqladmin -uroot -pta0mee drop  sns_user_$dbid
	mysqladmin -uroot -pta0mee create sns_app_$dbid
	mysqladmin -uroot -pta0mee create sns_user_$dbid
	gunzip /opt/20110121/sns_app_$dbid.sql.gz
	gunzip /opt/20110121/sns_user_$dbid.sql.gz
	mysql -u root -pta0mee sns_app_$dbid < /opt/20110121/sns_app_$dbid.sql
	mysql -u root -pta0mee sns_user_$dbid < /opt/20110121/sns_user_$dbid.sql
#	EOF
#done
done

#echo $mysql
#mysql -u root -pta0mee -t -v -e $mysql
exit 0
