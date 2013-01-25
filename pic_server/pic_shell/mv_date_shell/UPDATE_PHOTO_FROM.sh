#!/bin/sh
line=859
i=1
while [ "$i" -le "$line" ]
do
	awk 'NR=='$i'' key |sh update_photo_albumid
	i=$(($i+1))
done
exit 0
