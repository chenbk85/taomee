<?php
require_once("update.php");
require_once("log.php");
//----------------------------request format----------------------------
//cmd=xxx&ip=xxx&system=xxx&bin_num=xxx&bin_info=xxx&so_num=xxx&so_info=xxx&script_num=xxx&script_info=xxx&conf_num=xxx&conf_info=xxx
//ip: 内网IP
//machine: 1-debian, 2-centos
//bin_num: bin目录下文件数量
//bin_info格式: bin1_name;bin1_md5;bin2_name;bin2_md5;...
//bin_info格式: async_server;284da097f11ac20712c0bc8b62939a31;libnode.so;fbdf560721eb2436d29f4cb317eb5108
//----------------------------------------------------------------------
//----------------------------return format----------------------------
//失败返回命令格式: 1;fail_description
//成功返回命令格式: 0;bin_up_cmd;so_up_cmd;script_up_cmd;conf_up_cmd
//其中bin_up_cmd格式: bin_up_num;bin1_up_name;bin1_up_cmd;...
//so_up_cmd、script_up_cmd、conf_up_cmd格式同bin_up_cmd
//---------------------------------------------------------------------
if (empty($_POST["cmd"]))
{
    $log = "request cmd cannot be NULL";
    do_log($log, "ERROR");
    echo "1;".$log;
    return;
}

$handle_result = "1;failed";

switch ($_POST["cmd"])
{
    case 10001:
        $handle_result = check_update($_POST);
        break;
    default:
        $log = "cannot support cmd:{$_POST["cmd"]}";
        do_log($log, "ERROR");
        $handle_result = "1;{$log}";
        break;
}

//返回格式
//is_succ;bin_num;bin1_name;bin1_cmd;...;so_num;so1_name;so1_cmd;
echo $handle_result;

?>
