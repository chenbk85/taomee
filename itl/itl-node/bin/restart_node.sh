#! /bin/bash


red_clr="\033[31m"
grn_clr="\033[32m"
end_clr="\033[0m"



KILL_SLEEP=9
server="itl_node"
work_dir=`pwd`
target_bin="$work_dir/bin/itl_node"
target_conf="$work_dir/conf/bench.conf"
target_so="$work_dir/bin/libnode.so"


if [ ! -L $target_bin ]; then
    echo "$target_bin does NOT exist or is NOT a symbolic link. Exiting..."
    exit 1
fi

if [ ! -L $target_conf ]; then
    echo "$target_conf does NOT exist or is NOT a symbolic link. Exiting..."
    exit 1
fi

if [ ! -L $target_so ]; then
    echo "$target_so does NOT exist or is NOT a symbolic link. Exiting..."
    exit 1
fi


old_bin=`readlink -f $target_bin`
old_conf=`readlink -f $target_conf`
old_so=`readlink -f $target_so`




function usage()
{
	local lme="$1"

	echo "Usage: $lme [-b bin_file] [-c conf_file] [-s so_file]"
    echo "  Replace bin/conf/so files, and restart itl_node"
	echo "	-b: new bin file, bin/oa_node.xxxxx"
	echo "	-c: new configuration file, conf/bench.conf.xxxxx"
	echo "	-s: new so file, so/libnode.so.xxxxx"
	echo
}


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

function get_full_name()
{
    if [ -z "$1" ]; then
        return 0
    fi
    local dir=$(dirname $1)
    local base=$(basename $1)

    cd $dir;
    local full_dir=$(pwd)
    echo $full_dir/$base;
    return 0
}

pid=`get_pid $server`


function start_server()
{

    $target_bin $target_conf

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

while getopts ":b:c:s:" opt
do
	case $opt in
	b) new_bin=$OPTARG ;;
	c) new_conf=$OPTARG ;;
	s) new_so=$OPTARG ;;

	'?') usage "$0" ; exit 1 ;;
	':') echo "Option -$OPTARG requires an argument." ; exit 1 ;;
	esac
done

new_bin=`get_full_name $new_bin`

new_conf=`get_full_name $new_conf`

new_so=`get_full_name $new_so`




if [ "$new_bin" ]; then
    echo "target bin: "$target_bin
    echo "   old bin: "$old_bin
    echo "   new bin: "$new_bin
    echo 
    ln -sf $new_bin $target_bin
fi


if [ "$new_conf" ]; then
    echo "target conf: "$target_conf
    echo "   old conf: "$old_conf
    echo "   new conf: "$new_conf
    echo
    ln -sf $new_conf $target_conf
fi


if [ "$new_so" ]; then
    echo "taget so: "$target_so
    echo "  old so: "$old_so
    echo "  new so: "$new_so
    echo
    ln -sf $new_so $target_so
fi


stop_server;
start_server;

pid=`get_pid $server`


if [ $pid = "0" ]; then
    # 重启失败，需要恢复
    printf "$red_clr%50s$end_clr\n" "restart failed, rollback and then restart..."
    if [ "$new_bin" ]; then
        ln -sf $old_bin $target_bin
    fi

    if [ "$new_conf" ]; then
        ln -sf $old_conf $target_conf
    fi

    if [ "$new_so" ]; then
        ln -sf $old_so $target_so
    fi

    start_server
fi


exit 0
