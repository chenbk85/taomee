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
	mysqldump  --opt -u $user --password="$password" -h $host $1 t_serial_$2 > "$1-$2"
}

dbdump  "SERIAL_DB" $1 
