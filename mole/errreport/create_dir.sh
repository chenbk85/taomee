#!/bin/bash
errData=/var/www/errData
now=`date +%Y%m%d -d"+1 day"`
old=`date +%Y%m%d -d"-30 day"`

cd $errData
mkdir $now
cd $now
for ((i=0; i<100; i++))
do
    mkdir $i
done
cd ..
chown -R www-data:www-data $now

rm -rf $old
