#!/bin/sh
line=49892
i=1
while [ "$i" -le "$line" ]
do
	awk 'NR=='$i'' get_album_form |sh dealscript 
	i=$(($i+1))
done
exit 0
