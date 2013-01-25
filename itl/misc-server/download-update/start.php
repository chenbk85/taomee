#!/usr/bin/php
<?php
require_once("log.php");
require_once("config.php");
require_once("Rmail.php");

declare(ticks = 1);
$g_continue_working = true;

daemon();

$lock_fp = fopen(LOCK_FILE, 'c');
if($lock_fp == false) {
    echo "Can not open file:" . LOCK_FILE . "for write\n";
    do_log("Can not open file:" . LOCK_FILE . "for write", "error");
    exit(0);
}

if(!flock($lock_fp, LOCK_EX | LOCK_NB)) {
    echo "already running, exist\n";
    do_log("already running", "error");
    exit(0);
}

$pid = posix_getpid();
fwrite($lock_fp, $pid);

echo "Daemon running ...\n";

start_run();

function daemon()
{
    if (pcntl_fork()) {   
        exit(0);
    } else {   
        $sid = posix_setsid();
        if ($sid < 0) {   
            exit;
        }   
    }   
}

function start_run()
{
    global $g_continue_working;
    $check_file = UPDATE_PATH . UPDATE_FILE_NAME;

    pcntl_signal(SIGINT, "sig_handler");
    pcntl_signal(SIGTERM, "sig_handler");
    pcntl_signal(SIGQUIT, "sig_handler");
    pcntl_signal(SIGCHLD, "sig_handler"); 

    sleep(1);
    if (!is_writable(UPDATE_PATH . "bin/32/") || !is_writable(UPDATE_PATH . "bin/64/") ||
        !is_writable(UPDATE_PATH . "so/32/") || !is_writable(UPDATE_PATH . "so/64/") ||
        !is_writable(UPDATE_PATH . "conf/")   ) {
        echo "don't have write authority to dir bin,so,conf.\nexit";
        return;
    }
    
    echo "start successfully.";
	do_log("before main loop.", "debug");
    while ($g_continue_working) {
        if (file_exists($check_file) && filesize($check_file) > 0) {
            if (!is_writable($check_file)) {
                do_log("user must have read&write authority to " . $check_file, "error");
                return; 
            }
            $fp = fopen($check_file, "r+");
            if ($fp == false) {
                do_log("cann't open " . $check_file, "error");
                sleep(1);
                continue;
            }
            do_log("open file " . $check_file, "debug");

            $lines = array();
            if (flock($fp, LOCK_EX)) {
                do_log("read cmd from file " . $check_file, "debug");
                $lines = file($check_file);
                ftruncate($fp, 0);                 
                flock($fp, LOCK_UN);
            } else {
                do_log("flock " . $check_file . " failed.", "error");
            }

            fclose($fp);

            $handled_line = array();
            foreach ($lines as $line) {
                $line = trim($line);
                // 去掉里面重复的请求
                if (in_array($line, $handled_line)) {
                    continue;
                }
                $handled_line[] = $line;
                do_log("update notice: " . $line, "debug");
                get_update_file($line);
            }
        } else {
                do_log("no cmd in file " . $check_file, "debug");
        }        

        sleep(10);
    }  
	do_log("after main loop.", "debug");
}

function get_update_file($line)
{
    $update_cmd = split(";", $line);

    if ($update_cmd[0] == 1001) {           // so文件需要更新
        do_so_update($update_cmd); 
    } else if ($update_cmd[0] == 1002) {    // bin文件需要更新
        do_bin_update($update_cmd); 
    } else if ($update_cmd[0] == 1003) {    // config文件需要更新
        do_config_update($update_cmd); 
    } else if ($update_cmd[0] == 1004 || $update_cmd[0] == 1005) {
        do_alarm_update($update_cmd); 
    } else {
        do_log("unkonwn update cmd: $update_cmd[0].", "error");
    }
}

function do_so_update($update_cmd) 
{
    $so_name = $update_cmd[1];
    $is_32bit = $update_cmd[2];
    $action = $update_cmd[3];

    $bits = 0;
    if ($is_32bit == 1) {
        $bits = 32;
    }  else if ($is_32bit == 2) {
        $bits = 64;
    } else {
	    do_log("so bits:$bits is invalid.", "error");
        return;
    }

    if ($action == 2) {                 // 需要删除so
        @unlink(UPDATE_PATH . "so/" . $bits . "/" . $so_name); 
    } else if ($action == 1) {          // 需要更新so
        $data = array(); 
        $data["cmd"] = 2001;
        $data["so_name"] = $so_name;
        $data["is_32bit"] = $is_32bit;
        
        $key = md5(SIGN_KEY);
        $sign_str = "cmd=2001&" . "so_name=" . $so_name . "&is_32bit=" . $is_32bit . "&key=" . $key; 
        $data["sign"] = md5($sign_str);

        // 获得返回的字符串
        $contents = get_url_contents(UPDATE_URL, $data, "post", "");        
        //var_dump($contents);
        if (empty($contents)) {
            do_log("get url contents failed.", "error");
            write_failed_cmd_to_file($update_cmd);
            return;
        }
        
        $result = substr($contents, 0, 1);
        // 如果获取失败，则把这条记录再写入update_list_file，以后继续请求
        if ($result == 1) {
	        do_log("get so from url failed.", "error");
            //write_failed_cmd_to_file($update_cmd);
            return;
        } 

        $md5 = substr($contents, 1, 32);
        $so_content = substr($contents, 33);
        if ($md5 != md5($so_content)) {
	        do_log("md5 get from so is not equal to return md5.", "error");
            //write_failed_cmd_to_file($update_cmd);
            return;     
        }

        // 更新相应的so  
        $fp = fopen(UPDATE_PATH . "so/" . $bits . "/" . $so_name, "a");
        if ($fp == false) {
            do_log("fopen ". UPDATE_PATH . "so/" . $bits . "/" . $so_name . " failed.", "error");
            write_failed_cmd_to_file($update_cmd);
            return; 
        }
        if (flock($fp, LOCK_EX)) {
            sleep(2);                   // 等待对这个文件的下载完成
            ftruncate($fp, 0);                 
            fwrite($fp, $so_content);
            
            flock($fp, LOCK_UN);
        } else {
	        do_log("flock ". UPDATE_PATH . "so/" . $bits . "/" . $so_name . " failed.", "error");
            write_failed_cmd_to_file($update_cmd);
        }         

        fclose($fp);
    } 
}

function do_bin_update($update_cmd) 
{
    $bin_name = $update_cmd[1];
    $is_32bit = $update_cmd[2];
    $action = $update_cmd[3];
    $md5file = $update_cmd[4];
    $type = $update_cmd[5];
    
    if ($type == 1) {       // head需要跟新
        return;
    } 

    $bits = 0;
    if ($is_32bit == 1) {
        $bits = 32;
    }  else if ($is_32bit == 2) {
        $bits = 64;
    } else {
	    do_log("bin bits:$bits is invalid.", "error");
        return;
    }

    if ($action == 2) {                 // 需要删除bin
        @unlink(UPDATE_PATH . "bin/" . $bits . "/" . $bin_name); 
    } else if ($action == 1) {          // 需要更新bin
	    $bin_full_name = UPDATE_PATH . "bin/" . $bits . "/" . $bin_name; 	
        // 先判断md5是否一样，如果不一样才更新
	    if (file_exists($bin_fill_name) && $md5file == md5_file($bin_full_name)) {
	        do_log("No need to update, md5file is equal to local bin md5.", "debug");
            return;
        }

        $data = array(); 
        $data["cmd"] = 2002;
        $data["bin_name"] = $bin_name;
        $data["is_32bit"] = $is_32bit;
        $data["type"] = $type;

        $key = md5(SIGN_KEY);
        $sign_str = "cmd=2002&" . "bin_name=" . $bin_name . "&is_32bit=" . $is_32bit . "&type=" . $type . "&key=" . $key; 
        $data["sign"] = md5($sign_str);

        $contents = get_url_contents(UPDATE_URL, $data, "post", "");        
        //var_dump($contents);
        if (empty($contents)) {
            write_failed_cmd_to_file($update_cmd);
	        do_log("get url contents failed.", "error");
            return;
        }

        $result = substr($contents, 0, 1);
        if ($result == 1) {
            //write_failed_cmd_to_file($update_cmd);
	        do_log("get bin from url failed.", "error");
            return;
        } 

        $md5 = substr($contents, 2, 32);
        $so_content = substr($contents, 34);
        if ($md5 != md5($so_content)) {
            //write_failed_cmd_to_file($update_cmd);
	        do_log("md5 get from bin is not equal to return md5.", "error");
            return;     
        }

        // 更新相应的bin文件  
        $fp = fopen(UPDATE_PATH . "bin/" . $bits . "/" . $bin_name, "a");
        if ($fp == false) {
            write_failed_cmd_to_file($update_cmd);
	        do_log("fopen ". UPDATE_PATH . "bin/" . $bits . "/" . $bin_name . " failed.", "error");
            return;
        }
        if (flock($fp, LOCK_EX)) {
            sleep(2);                   // 等待对这个文件的下载完成
            ftruncate($fp, 0);                 
            fwrite($fp, $so_content);
            flock($fp, LOCK_UN);
        } else {
            write_failed_cmd_to_file($update_cmd);
	        do_log("flock ". UPDATE_PATH . "bin/" . $bits . "/" . $bin_name . " failed.", "error");
        }      

        fclose($fp);
    } 
}

function do_config_update($update_cmd) 
{
    $id_list = $update_cmd[1];
    if (empty($id_list)) {
        do_log("id_list is null", "error");
        return;
    }
    $type = $update_cmd[2];

    if ($type == 1) {       // head需要跟新
        return;
    } 

    // 需要更新的机器是用,分割的ip列表
    $ids = split(",", $id_list); 
    $key = md5(SIGN_KEY);
    foreach ($ids as $id) {
        $data = array(); 
        $data["cmd"] = 2003;
        //$data["type"] = $type;
        $data["server_id"] = $id;

        $sign_str = "cmd=2003" . "&server_id=" . $id . "&key=" . $key; 
        $data["sign"] = md5($sign_str);

        $failed_cmd = array(1003, $id, $type);

        $contents = get_url_contents(UPDATE_URL, $data, "post", "json");        
        //var_dump($contents);
        if (empty($contents)) {
            write_failed_cmd_to_file($failed_cmd);
            do_log("get url contents failed: empty.", "error");
            continue;
            //return;
        }

        if ($contents["result"] != 0) {
            //write_failed_cmd_to_file($failed_cmd);
            do_log("get config from url failed:" . $id, "error");
            continue;
            //return;
        } 

        if ($contents["md5"] != md5($contents["content"])) {
            //write_failed_cmd_to_file($failed_cmd);
            do_log("contents: " . $contents["content"] . " is not equal to return md5: " . $contents["md5"], "error");
            //return;     
            continue;     
        }

        // 更新相应的config文件  
        $fp = fopen(UPDATE_PATH . "conf/" . $contents["ip"], "a");
        if ($fp == false) {
            write_failed_cmd_to_file($failed_cmd);
            do_log("fopen ". UPDATE_PATH . "conf/" . $contents["ip"] . " failed.", "error");
            return; 
        }
        if (flock($fp, LOCK_EX)) {
            sleep(2);                   // 等待对这个文件的下载完成
            ftruncate($fp, 0);                 
            fwrite($fp, $contents["content"]);
            flock($fp, LOCK_UN);
        } else {
            write_failed_cmd_to_file($failed_cmd);
            do_log("flock ". UPDATE_PATH . "conf/" . $contents["ip"] . " failed.", "error");
        }      

        fclose($fp);
    } 
}

function do_alarm_update($update_cmd) 
{
    $host_list = $update_cmd[1];
    if (empty($host_list)) {
        do_log("host_list is null", "error");
        return;
    }

    // 需要更新的机器是用,分割的metric列表
    $host = split(",", $host_list); 
    $key = md5(SIGN_KEY);
    foreach ($host as $host_name) {
        $data = array(); 
        switch($update_cmd[0])
        {
        case '1004': 
        {
            $data["cmd"] = 2004; 
            $data["server_id"] = $host_name;
            $sign_str = "cmd=" . $data["cmd"] . "&server_id=" . $host_name . "&key=" . $key; 
            break;
        }
    case '1005': 
        {
            $data["cmd"] = 2005; 
            $data["switch"] = $host_name;
            $sign_str = "cmd=" . $data["cmd"] . "&switch=" . $host_name . "&key=" . $key; 
            break;
        }
    default :
        {
            do_log("host type is wrong", "error");
            return;
        }
        }

        $data["sign"] = md5($sign_str);

        $failed_cmd = array($update_cmd[0], $host_name);

        $contents = get_url_contents(UPDATE_URL, $data, "post", "json");        
        //var_dump($contents);
        if (empty($contents)) {
            write_failed_cmd_to_file($failed_cmd);
            do_log("get url contents failed.", "error");
            return;
        }

        if ($contents["result"] == 1) {
            //write_failed_cmd_to_file($failed_cmd);
            do_log("get alarm-config from url failed,host id:" . $host_name, "error");
            return;
        } 

        if ($contents["md5"] != md5($contents["content"])) {
            //write_failed_cmd_to_file($failed_cmd);
            do_log("contents:" . $contents["content"] . 
                " md5 get from content is not equal to return md5.", "error");
            return;     
        }

        switch($update_cmd[0])
        {
        case '1004':
        {
            $host_ip = $contents["host"];
            break;
        }
    case '1005':
        {
            $host_ip = $contents["switch"];
            break;
        }
        }
        // 更新相应的config文件  
        $fp = fopen(UPDATE_PATH . "alarm-conf/" . $host_ip . ".php", "a");
        if ($fp == false) {
            write_failed_cmd_to_file($failed_cmd);
            do_log("fopen ". UPDATE_PATH . "alarm-conf/" . $host_ip . " failed.", "error");
            return;
        }
        if (flock($fp, LOCK_EX)) {
            sleep(2);                   // 等待对这个文件的下载完成
            ftruncate($fp, 0);                 
            $config_file = json_decode($contents["content"], true);
            fwrite($fp, "<?php\n##this file is created by update program automaticly, do not modify it by yourself\nreturn ");
            fwrite($fp, var_export($config_file, true));
            fwrite($fp,  ";");
            flock($fp, LOCK_UN);
        } else {
            write_failed_cmd_to_file($failed_cmd);
            do_log("flock ". UPDATE_PATH . "alarm-conf/" . $host_ip . " failed.", "error");
        }      

        fclose($fp);
    } 
}


function sig_handler($signo)
{
    global $g_continue_working;
    switch($signo)
    {   
        case SIGCHLD:
        case SIGINT:
        case SIGQUIT:
		    do_log("receive $signo.", "debug");
            break;
        case SIGTERM:
		    do_log("receive $signo.", "debug");
            $g_continue_working = false;
            break;
        default:
		    do_log("receive $signo.", "error");
            break;
    }   
}

function write_failed_cmd_to_file($update_cmd)
{
    $failed = false;
    $fp = fopen(UPDATE_PATH . UPDATE_FILE_NAME, "a");
    if ($fp == false) {
        $failed = true;
        do_log("write  download failed cmd to file failed.", "error");
    }

    if ($failed == false && flock($fp, LOCK_EX)) {
        if ($update_cmd[0] == 1001) {           // so文件需要更新
            fwrite($fp, $update_cmd[0] . ";" . $update_cmd[1] . ";" . $update_cmd[2] . ";" . $update_cmd[3] . ";". $update_cmd[4] . "\n");
	        do_log("write failed so cmd to file.", "debug");
        } else if ($update_cmd[0] == 1002) {    // bin文件需要更新
            fwrite($fp, $update_cmd[0] . ";" . $update_cmd[1] . ";" . $update_cmd[2] . ";" . $update_cmd[3] . ";" . 
			$update_cmd[4] . ";" . $update_cmd[5] . "\n");
	        do_log("write failed bin cmd to file.", "debug");
        } else if ($update_cmd[0] == 1003) {    // config文件需要更新
            fwrite($fp, $update_cmd[0] . ";" . $update_cmd[1] . ";" . $update_cmd[2] . "\n");
	        do_log("write failed config cmd to file.", "debug");
        } else if ($update_cmd[0] == 1004) { 
            fwrite($fp, $update_cmd[0] . ";" . $update_cmd[1] . "\n");
	        do_log("write failed alarm cmd to file.", "debug");
        } else {
	        do_log("unkonwn update cmd: $update_cmd[0].", "error");
        }

        flock($fp, LOCK_UN);

    } else {
        $failed = true;
        do_log("write  download failed cmd to file failed.", "error");
    }

    if (REPORT_ERROR && $failed) {
        $mail_body = "";
        foreach ($update_cmd as $cmd) {
            $mail_body .= $cmd . "  ";
        }

        $mail = new Rmail();
        $mail->setTextCharset('UTF-8');
        $mail->setHTMLCharset('UTF-8');
        $mail->setHeadCharset('UTF-8');
        $mail->setSMTPParams(SMTP_HOST, 25, 'helo', false, '', '');
        $mail->setFrom(SMTP_FROM_EMAIL);
        $mail->setSubject('OA更新服务器');
        $mail->setHTML("更新服务器下载更新程序失败，通知命令为:\n" . $mail_body);


        if (!$mail->send(array(SMTP_TO_EMAIL), 'smtp')) {
            do_log("failed to send mail", "error");
        }   
    }

    fclose($fp);
}

/**
 * 向主机发送post请求，并对字符串进行 urlencode 编码
 *
 * @param     string url 请求主机
 * @param     string data post数据内容（格式：用key=value&key=value，或者关联数组）
 * @param     string method 请求方式 get or post
 * @param     string data type 请求的数据格式， var_export 或者 json 格式
 * @param     int timeout 请求的超时设置
 * @return    mixed
 * @author    Rooney<rooney.zhang@gmail.com>
 */
function get_url_contents($url, $data, $method="get", $data_type='var_export', $timeout=60)
{
    /* 配置完整URL */
    $url = (false === strpos($url, 'http://')) ?  'http://' . $url : $url;

    $ch = curl_init();
    if('get' == strtolower($method)) {
        $url = is_array($data) ? $url.'?'.http_build_query($data) : $url . '?' .$data ;
        curl_setopt($ch, CURLOPT_URL, $url);
    } else {
        $data = is_array($data) ? http_build_query($data) : $data ;
        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_POST, 1);
        curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
    }

    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, $timeout) ;
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1); // Return the transfer as a string
    $output = curl_exec($ch);
    curl_close($ch);

    if($data_type == 'var_export') {
        $result = '' ;
        @eval("\$result=$output;");
        return $result ;
    } elseif ('json' == $data_type) {
        return json_decode($output, true) ;
    } else {
        return $output ;
    }
}

?>
