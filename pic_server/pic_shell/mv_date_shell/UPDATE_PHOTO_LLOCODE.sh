#!/bin/sh
line=20
i=1
while [ "$i" -le "$line" ]
do
#	j=$(($i%2))
#	if [ "$j" -eq "0" ]
#	then
		awk 'NR=='$i'' up_photo_res |sh update_photo_llocode
#	fi
	i=$(($i+1))
done
exit 0
