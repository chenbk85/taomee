#!/bin/sh
line=762
i=1
while [ "$i" -le "$line" ]
do
	awk 'NR=='$i'' logo |sh upload_logo 
	i=$(($i+1))
done
exit 0
