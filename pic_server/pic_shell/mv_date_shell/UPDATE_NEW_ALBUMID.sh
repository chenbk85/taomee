#!/bin/sh
line=40803
i=1
while [ "$i" -le "$line" ]
do
	awk 'NR=='$i'' create_album |sh insert_new_album_id
	i=$(($i+1))
done
exit 0
