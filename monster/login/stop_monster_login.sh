#!/bin/sh
red_clr="\033[31m"
grn_clr="\033[32m"
end_clr="\033[0m"

result=`ps -C monster_login | wc -l`
if test $result -le 1; then
    printf "$red_clr%50s$end_clr\n" "monster_login is not running"
    exit -1
fi

result=`ps -C monster_login | wc -l`
while ! test $result -le 1; do
    pkill -TERM monster_login
    sleep 1
    result=`ps -C monster_login | wc -l`
done

printf "$grn_clr%50s$end_clr\n" "monster_login has been stopped"

if [ "$1" = "log" -o "$1" = "log/" ]; then
    rm ./log/* -rf
    printf "$grn_clr%50s$end_clr\n" "log files has been removed"
fi

