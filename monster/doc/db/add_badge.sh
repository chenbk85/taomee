#!/bin/sh
host=10.1.1.24
port=3306
user=monster
password=monster@24

series="(1,2,3,4) (5,6,7,8) (9,10,11,12) (13,14,15,16) (17,18,19,20) (21,22,23,24)"

for i in `seq 0 9`
do
    for j in `seq 0 99`
    do
	line=14
	for ser in $series 
	do
        	tb_sentence="SELECT user_id, count(*) FROM db_monster_$i.t_pet_$j where pet_id in $ser group by user_id;"
         	buffer=$(mysql -h$host -u$user -p$password -N -e "$tb_sentence") 
		if [[ $buffer == "" ]]; then
		continue;
		fi
		echo "$buffer" | while read user_id value 
		do
			if [ $value -ge 4 ]; then
				status=1
			else
				status=0 
			fi	
			ins_sentence="INSERT INTO db_monster_$i.t_badge_$j(user_id, badge_id, status, progress) values($user_id, $line, $status, $value);"	
			echo $ins_sentence
			mysql -h$host -u$user -p$password -e "$ins_sentence"
		done		
		line=$(($line + 1))	
	done
    done
done

