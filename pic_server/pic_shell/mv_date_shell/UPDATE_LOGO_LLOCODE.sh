#!/bin/sh
line=69
i=1
while [ "$i" -le "$line" ]
do
#	j=$(($i%2))
#	if [ "$j" -eq "0" ]
#	then
	awk 'NR=='$i'' lost_resu |sh update_logo_llocode
#	fi
	i=$(($i+1))
done
exit 0
