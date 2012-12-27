#!/bin/bash
create_user()
{
	id=$1
	a1=$((id%100))
	a=`printf "%02d" $a1`
	b1=$(($((id%10000))/100))
	b=`printf "%02d" $b1`
	echo "insert into USER_INFO_$a.t_user_info_$b values($1, 0x01, 1, 1, 0xE10ADC3949BA59ABBE56E057F20F883E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);"
	a1=$((id%100))
	a=`printf "%02d" $a1`
	b1=$(($((id%10000))/1000))
	b=`printf "%01d" $b1`
	echo "insert into USER_$a.t_user_$b values($1, 0x00, 1, 1, 0x00, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0000, 0, 0, 0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000);"
	a1=$((id%100))
	a=`printf "%02d" $a1`
	b1=$(($((id%10000))/1000))
	b=`printf "%01d" $b1`
	echo "insert into USER_$a.t_user_ex_$b values($1, 0, '1234', 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0);"
}



for ((userid=70000000; userid<70002000; userid++));
do 
	create_user $userid
done
