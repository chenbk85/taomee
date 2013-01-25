#!/bin/sh
line=61
i=1
while [ "$i" -le "$line" ]
do
	j=$(($i%10))
	if [ $j = "0" ]
	then
		`sleep 1`
	fi
#		awk 'NR=='$i'' photo |sh upload_photo &
		awk "NR==$i" ll  |sh upload_photo $i >>result_u_p &
	i=$(($i+1))
done
exit 0
