#!/bin/sh

db_index=0
user="root"
password="ta0mee"
host="localhost"
tmp_file="table.sql"
fix=`date +%Y%m%d`
bakdir=~/dbbak/$fix
mkdir $bakdir
cd $bakdir

dbdump()
{
	mysqldump  --opt -u $user --password="$password" -h $host $1 > "$1"
}

while [ $db_index -lt 100 ] ; do
	dbx=`printf "%02d" $db_index`
	dbdump  "USER_$dbx" 
	db_index=`expr $db_index + 1`
done
dbdump  "EMAIL_DB" 
dbdump  "INCREMENT_DB" 
dbdump  "GAME_SCORE_DB" 
dbdump  "REGISTER_SERIAL_DB" 

cd ..
tar cvf  $fix.tar $fix
scp  -2 -P 56000  $fix.tar  192.168.0.3:~/dbbak/

