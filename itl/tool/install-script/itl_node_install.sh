#!/bin/bash


#echo 'cd /opt/taomee/itl/itl-node/ && ./start_itl_node.sh' >> /etc/rc.local


red_clr="\033[31m"
grn_clr="\033[32m"
end_clr="\033[0m"

install_path="/opt/taomee/itl/itl-node"
bin_path="$install_path/bin/"
so_path="$install_path/so/"
conf_path="$install_path/conf/"
log_path="$install_path/log/"
script_path="$install_path/"

# 确定是走外网还是走内网
/bin/ping -n -U -w1 -c1 192.168.6.51 > /dev/null && result=0 || result=1
if [ "$result" == "0" ]; then
    download_server_ip=192.168.6.51
else
    download_server_ip=114.80.98.51
fi

server_name="itl_node"
script_name="itl_node_install.sh"


function check_ip_valid()
{
    local check_ip=$1
    local ip_reg="^[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}$"
    if [ ! -n "`echo $check_ip | grep -e $ip_reg`" ]; then
        return 1
    else
        return 0
    fi
}

function get_local_ip()
{
    local ip_inside="`ifconfig eth1 | grep '192.168' | awk '{print $2}' | tr -d 'addr:'`"
    if [ -z "$ip_inside" ]; then
        ip_inside="`ifconfig | grep '192.168' | awk '{print $2}' | tr -d 'addr:'`"
    fi

    check_ip_valid $ip_inside
    ret=$?
    if [ $ret == 0 ]; then
        echo $ip_inside;
        return 0;
    fi

    if [ -z "$ip_inside" ]; then
        ip_inside="`ifconfig | grep 'addr:10\.' | awk '{print $2}' | tr -d 'addr:'`"
    fi
    echo $ip_inside
    return 0;
}

function stop_server()
{
    local srv_name=$1
    local pids=`ps auxwf | grep $srv_name | grep -v grep | grep -v $script_name | awk '{print $2}' `
    if [ "$pids" != "" ]; then
        printf "$red_clr%50s$end_clr\n" "$srv_name is running"
        kill -9 $pids
        printf "$grn_clr%50s$end_clr\n" "$srv_name has been stopped"
    else
        printf "$red_clr%50s$end_clr\n" "$srv_name is not running"
    fi

    return 0
}

#$1-download_url, $2-文件保存路径, $3-文件名列表, $4-文件属性, $5-是否连接文件数组 
function download_file()
{
    local download_url=$1
    local dst_path=$2
    local file_list=$3
    local is_excutable=$4
    local link_array=($5)

    local datetime=$(date +%Y%m%d%H%M%S)

    cd $dst_path
    index=0
    for file in $file_list
    do
        wget $download_url/$file
        if [ -f $dst_path/$file ]; then
            printf "$grn_clr%50s$end_clr\n" "wget $download_url/$file succ."
        else
            printf "$red_clr%50s$end_clr\n" "ERROR: wget $download_url/$file failed!"
            return 1
        fi
        chmod 644 $file
        if [ "$is_excutable" == "1" ];then
            chmod u+x $file
            if [ ! -x $dst_path/$file ]; then
                printf "$red_clr%50s$end_clr\n" "ERROR: $dst_path/$file isnot executable!"
                return 1
            fi
        fi
        if [ "${link_array[$index]}" == "1" ];then
            dst_file=$file.$datetime
            mv $file $dst_file
            ln -s $dst_file $file
            if [ ! -L $dst_path/$file ]; then
                printf "$red_clr%50s$end_clr\n" "ERROR: $dst_path/$file link failed!"
                return 1
            fi
        fi

        let index++
    done

    return 0
}


check_ip_valid $download_server_ip
check_result=$?
if [ $check_result != 0 ]; then
    printf "$red_clr%50s$end_clr\n" "Wrong download service IP: $download_server_ip"
    exit 0
fi

ip_inside=`get_local_ip`
check_ip_valid $ip_inside
check_result=$?
if [ $check_result != 0 ]; then
    printf "$red_clr%50s$end_clr\n" "Wrong inside IP: $ip_inside"
    exit 0
else
    printf "$grn_clr%50s$end_clr\n" "Inside IP: $ip_inside"
fi

#检查服务是否已运行
#if [ -n "`ps -ef | grep $server_name | grep -v $0 | grep -v grep`" ]; then
#    printf "$red_clr%50s$end_clr\n" "Server \"$server_name\" is running, if you want to renew it, pls stop the old."
#    exit 0
#fi

stop_server $server_name

rm -rf $install_path

#配置检查和安装
#if [ ! -n "`dpkg -l | grep libcurl3`" ]; then
#    printf "$grn_clr%50s$end_clr\n" "Lack of package \"libcurl\", start to install it firstly..."
#    date;apt-get -y autoremove;apt-get --assume-yes  --force-yes install libcurl3;date
#    printf "$grn_clr%50s$end_clr\n" "End of \"libcurl\" installation."
#else
#    printf "$grn_clr%50s$end_clr\n" "Package \"libcurl\" has been installed."
#fi


##下载服务URL
download_path="http://$download_server_ip/itl-auto-update"
#获取机器位数
system_bit=`getconf LONG_BIT`
# 获取机器发行版，debian or centos
if [ -e "/etc/debian_version" ]; then
    distribution="debian"
elif [ -e "/etc/redhat-release" ]; then
    distribution="centos"
else
    printf "$red_clr%50s$end_clr\n" "unsupport linux distribution"
    exit 0;
fi




#创建目录
mkdir -p $bin_path
mkdir -p $so_path
mkdir -p $conf_path
mkdir -p $log_path
mkdir -p $script_path
chmod 755 -R /opt/taomee/itl

##### 下载文件 #####



#download bin
bin_list="itl_node libnode.so"
bin_link="1 1"

if [ -d $bin_path ]; then
    download_url="$download_path/bin/$distribution"
    is_excutalbe=1
    download_file "$download_url" "$bin_path" "$bin_list" "$is_excutalbe" "$bin_link"
    func_ret=$?
    if [ $func_ret != 0 ];then
        exit -1
    fi
    printf "$grn_clr%50s$end_clr\n" "End of bin downloading."
else
    printf "$red_clr%50s$end_clr\n" "ERROR: directory \"$bin_path\" does not exist."
    exit -1
fi


#download script
script_list="itl_node.sh"
script_link="0"
if [ -d $script_path ]; then
    download_url="$download_path/script"
    is_excutalbe=1
    download_file "$download_url" "$script_path" "$script_list" "$is_excutalbe" "$script_link"
    func_ret=$?
    if [ $func_ret != 0 ];then
        exit -1
    fi
    printf "$grn_clr%50s$end_clr\n" "End of script downloading."
else
    printf "$red_clr%50s$end_clr\n" "ERROR: directory \"$script_path\" does not exist."
    exit -1
fi


# download conf
conf_list="work.conf"
conf_link="1"
if [ -d $conf_path ]; then
    download_url="$download_path/conf"
    is_excutalbe=0
    download_file "$download_url" "$conf_path" "$conf_list" "$is_excutalbe" "$conf_link"
    func_ret=$?
    if [ $func_ret != 0 ];then
        exit -1
    fi

    src_url="$download_path/conf/$ip_inside"
    dst_file="$conf_path/bench.conf.$(date +%Y%m%d%H%M%S)"
    wget $src_url -O $dst_file
    if [ -s $dst_file ]; then
        ln -sf $dst_file "$conf_path/bench.conf"
    else
        rm -f $dst_file
        conf_list="bench.conf"
        download_file "$download_url" "$conf_path" "$conf_list" "$is_excutalbe" "$conf_link"
        func_ret=$?
        if [ $func_ret != 0 ]; then
            exit -1
        fi
    fi
    printf "$grn_clr%50s$end_clr\n" "End of config downloading."
else
    printf "$red_clr%50s$end_clr\n" "ERROR: directory \"$conf_path\" does not exist."
    exit -1
fi

cd $conf_path
bench_conf=`ls | grep bench.conf.`
sed -i "s/0.0.0.0/$ip_inside/g" $bench_conf

#启动服务
cd $install_path
./itl_node.sh start 1>/dev/null 2>/dev/null &
exit 0;
