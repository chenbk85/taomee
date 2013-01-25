line=16
i=1
while [ "$i" -le "$line" ]
do
#	awk 'NR=='$i'' upload_result |sh mv_2.sh
	var=`awk 'NR=='$i'' upload_result `
	lloccode=`echo $var | awk -F"\"" '{print $18}'`
	cmd="-l ${lloccode} $cmd"
	i=$(($i+1))
done
	echo $cmd
	i=$(($i-1))
	`./client -f $i $cmd -k 2000 -m 3002 >3`
exit 0

