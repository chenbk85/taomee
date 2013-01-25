#!/bin/bash

red_clr="\033[31m"
grn_clr="\033[32m"
end_clr="\033[0m"
oa_node_path="/opt/taomee/oa/oa-node"
bin_path="$oa_node_path/bin/"
OA_RUN=0
OA_BIN=0
PID=`ps auxwf | grep 'oa_node' | grep -v $0 | grep -v grep | awk '{print $2}' `


#if [ $# -ne 1 ]; then
#    printf "$grn_clr%50s$end_clr\n" "Usage: $0  download_service_ip"
#    exit 0
#fi
download_service_ip="192.168.6.51"

dl_srv_ip=$1
localhost_ip="`ifconfig|grep '192.168'|awk '{print $2}'|tr -d 'addr:'`"
#检查IP的合法性
ip_reg="^[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}$"
if [ ! -n "`echo $localhost_ip | grep $ip_reg`" ]; then
    printf "$red_clr%50s$end_clr\n" "Wrong localhost IP: $localhost_ip"
    exit 0
fi
printf "$grn_clr%50s$end_clr\n" "Localhost IP: $localhost_ip    [OK]"

if [ ! -n "`echo $dl_srv_ip | grep $ip_reg`" ]; then
    printf "$red_clr%50s$end_clr\n" "Wrong download service IP: $dl_srv_ip"
    exit 0
fi


if [ -n $PID ]; then
	echo "oa_node is running, Its PID is $PID"
	echo "killing oa_node"
	kill -9 $PID
fi

rm -fr $oa_node_path

#配置检查和安装
#if [ ! -n "`dpkg -l | grep libcurl3`" ]; then
    printf "$grn_clr%50s$end_clr\n" "Lack of package \"libcurl\", start to install it firstly..."
    date;apt-get -y autoremove;apt-get --assume-yes  --force-yes install libcurl3;date
    printf "$grn_clr%50s$end_clr\n" "End of \"libcurl\" installation."
#else
#    printf "$grn_clr%50s$end_clr\n" "Package \"libcurl\" has been installed."
#fi


#下载服务URL
download_path="http://$dl_srv_ip/oa-auto-update"
#更新服务URL
server_url="$download_path/index.php"
#服务启动命令
start_cmd="./oa_node -c \"$localhost_ip\" -s \"$server_url\""

#获取机器位数
system_bit=`getconf LONG_BIT`

#检查服务是否已运行
if [ -n "`ps -ef | grep oa_node | grep -v $0 | grep -v grep`" ]; then
    printf "$red_clr%50s$end_clr\n" "Server \"oa_node\" is running, if you want to renew it, pls stop the old."
    exit 0
fi

#检查文件是否已存在
if [ -f $bin_path/oa_node ]; then
    printf "$red_clr%50s$end_clr\n" "File \"$bin_path/oa_node\" has existed, if you want to reload it, pls clean the old"
    exit 0
fi

#创建bin目录
mkdir -p $bin_path
chmod 755 -R /opt/taomee/oa

if [ -d $bin_path ]; then
    #下载脚本文
    cd $oa_node_path
    wget $download_path/script/start_oa_node.sh
    wget $download_path/script/state_oa_node.sh
    wget $download_path/script/stop_oa_node.sh
    chmod u+x *.sh
    printf "$grn_clr%50s$end_clr\n" "End of script downloading."

    cd $bin_path
    #下载bin文件
    wget $download_path/bin/$system_bit/oa_node
    if [ -f $bin_path/oa_node ]; then
        printf "$grn_clr%50s$end_clr\n" "wget $download_path/bin/$system_bit/oa_node succ."
    else
        printf "$red_clr%50s$end_clr\n" "ERROR: wget $download_path/bin/$system_bit/oa_node failed!"
        exit -1
    fi
    chown nobody $bin_path/oa_node


    #修改oa_node为可执行文件
    chmod u+x oa_node
    if [ ! -x $bin_path/oa_node ]; then
        printf "$red_clr%50s$end_clr\n" "ERROR: oa_node isnot executable!"
        exit -1
    fi
else
    printf "$red_clr%50s$end_clr\n" "ERROR: directory \"$bin_path\" does not exist."
    exit -1
fi

#启动服务
#./oa_node -c client_ip -s server_url
./oa_node -c "$localhost_ip" -s "$server_url"
