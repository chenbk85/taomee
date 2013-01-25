#!/bin/sh
for x in `seq 0 99`;
do
	if [ $x -lt 10 ];then
		dbid="0$x"
	else
		dbid="$x"
	fi

for xx in `seq 00 99`;
do
	if [ $xx -lt 10 ];then
		tbid="0$xx"
	else
		tbid="$xx"
	fi
	mysql -u root -pta0mee <<- EOF
	use "sns_user_$dbid"
	alter table sns_user_$dbid.user_photo_$tbid add album_key int(10) NOT NULL default 0;
	EOF
	echo "alter table sns_user_$dbid.user_photo_$tbid add album_key int(10) NOT NULL default 0;\n"
done
done

#echo $mysql
#mysql -u root -pta0mee -t -v -e $mysql
exit 0
