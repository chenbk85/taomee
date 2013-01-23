#!/bin/bash

red_clr="\033[31m"
grn_clr="\033[32m"
end_clr="\033[0m"


server=$(basename $0 .sh)
reload_so="./reload_so"
reload_conf="./conf/reload.conf"
log_dir="./log/"
#lib_path=/home/pea/lib/
#lib_path_release=/opt/taomee/pea/lib/

grep_result=$(grep "cliu" ./bin/$server)
if [ -z "$grep_result" ]; then
    config_file="./bench.lua"
    server_type=2
else
    config_file="./bench.conf"
    server_type=1
    so_file=$(ls ./lib*.so)
fi



function get_pid()
{
    local server_name="$1"
    local pid_list=$(pgrep -P 1 $server_name)
    local pid=
    local cwd=

    if [ -z "$pid_list" ]; then
        echo 0
        return 0
    fi

    for pid in $pid_list
    do
        
        cwd=`readlink /proc/$pid/cwd`
        if [ "$cwd" = $(pwd -P) ]; then
            echo $pid
            return 1
        fi
    done

    
    echo 0
    return 0
}


pid=`get_pid $server`

function clear_log()
{

    if [ "$1" = "log" -o "$1" = "l" ]; then

        rm $log_dir/* -rf
        printf "$grn_clr%50s$end_clr\n" "log files has been removed"

    fi

}

function start_server()
{

    #if [ -d $lib_path ]; then

        #export LD_LIBRARY_PATH=$lib_path

    #else

        #export LD_LIBRARY_PATH=$lib_path_release

    #fi


    if [ $server_type = 1 ]; then

        ./bin/$server $config_file $so_file

    else

        ./bin/$server $config_file

    fi



}

function stop_server()
{
    if [ $pid = "0" ]; then

        printf "$red_clr%50s$end_clr\n" "ERROR: $server is not started"

    else

        result=`ps -p $pid | wc -l`

        if [ $result -le 1 ]; then

            printf "$red_clr%50s$end_clr\n" "ERROR: $server is not started"

        else

            while ! test $result -le 1; do
                kill $pid
                sleep 1
                result=`ps -p $pid | wc -l`
            done

            printf "$grn_clr%50s$end_clr\n" "$server has been stopped"


        fi
    fi


}

if [ "$1" = "start" ]; then

    start_server;
    clear_log "$2"




elif [ "$1" = "state" ]; then

    if [ $pid = "0" ]; then

        printf "$red_clr%50s$end_clr\n" "ERROR: $server is not started"

    else

        result=`ps -p $pid | wc -l`
        if [ $result -lt 1 ]; then

            printf "$red_clr%50s$end_clr\n" "ERROR: $server is not started"

        else

            ps -o user,pid,stat,pcpu,pmem,cmd -s $pid

        fi
    fi

elif [ "$1" = "stop" ]; then

    stop_server;
    clear_log "$2"

elif [ "$1" = "restart" ]; then

    stop_server;
    clear_log "$2";
    start_server;

elif [ "$1" = "r" ]; then

    stop_server;
    clear_log "$2";
    start_server;


elif [ "$1" = "reload" ]; then

    if [ -e $reload_conf ]; then

        $reload_so $reload_conf

    else

        printf "$red_clr%50s$end_clr\n" "ERROR: $server does NOT support reload"

    fi

else

    printf "Usage: %s start|state|stop|restart|reload [log]\n" $0

fi


