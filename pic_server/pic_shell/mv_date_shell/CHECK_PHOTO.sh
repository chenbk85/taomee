#!/bin/sh
line=3290
i=1
while [ "$i" -le "$line" ]
do
	awk 'NR=='$i'' photo |sh upload_photo &
	i=$(($i+1))
done
exit 0
