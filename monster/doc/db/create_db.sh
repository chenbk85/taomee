#!/bin/sh
host=127.0.0.1
port=3306
user=monster
password=monster@24

for i in `seq 0 9`
do
    db_sentence="set names utf8;create database db_monster_$i;"
        mysql -h$host -u$user -p$password -e"$db_sentence"
done

db_sentence="set names utf8;create database db_monster_config;"
mysql -h$host -u$user -p$password -e"$db_sentence"

