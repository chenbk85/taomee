#!/bin/bash
comip="localhost"
comport=21001
attiretype=0
attireid=150002
attirecount=1
useridlist=`./get_fire_day.sh | sed -n  '2,$p' `
#
for userid in $useridlist  ;
do
    ./add_attire $comip  $comport $userid  $attiretype $attireid $attirecount
done

