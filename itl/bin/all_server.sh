#!/bin/bash

red_clr="\033[31m"
grn_clr="\033[32m"
end_clr="\033[0m"


# kill之后9秒内，程序没有退出则使用kill -9
KILL_SLEEP=9

server=$(basename $0 .sh)
log_dir="./log/"

config_file="./conf/bench.conf"



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
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:`pwd`/bin"


    ./bin/$server $config_file




}

function stop_server()
{
    if [ $pid = "0" ]; then

        printf "$red_clr%50s$end_clr\n" "ERROR: $server is not running"

    else

        result=`ps -p $pid | wc -l`

        if [ $result -le 1 ]; then

            printf "$red_clr%50s$end_clr\n" "ERROR: $server is not running"

        else

            child_pids=`pgrep -P $pid | xargs`
            kill -SIGTERM $pid

            count=0
            while test $result -gt 1; do
                let count="$count+1"
                # 9次之后用kill -9
                if [ $count -gt $KILL_SLEEP ]; then
                    kill -9 $pid $child_pids
                    break
                fi
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

        printf "$red_clr%50s$end_clr\n" "ERROR: $server is not running"

    else

        result=`ps -p $pid | wc -l`
        if [ $result -lt 1 ]; then

            printf "$red_clr%50s$end_clr\n" "ERROR: $server is not running"

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


else

    printf "Usage: %s start|state|stop|restart [log]\n" $0

fi


