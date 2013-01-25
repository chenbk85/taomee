<?php
require_once("config.php");

/**
 * 写日志
 *
 * @author       Rooney<rooney.zhang@gmail.com>
 * @author       aceway<aceway15@sohu.com>
 */
function do_log($str, $level, $line="", $func="")
{
    $log_file = "";
    $time= date("[H:i:s]");
    if(empty($log_file)) 
    {
        $log_name = LOG_PREFIX.date("Y_m_d").".log" ;
        $log_file = LOG_DIR.$log_name;
        if(!is_dir(LOG_DIR)) 
        {
            exec("mkdir ".LOG_DIR) ;
            exec("chmod -R 777 ".LOG_DIR) ;
        }
    }

    if(!is_file($log_file)) 
    {
        exec("touch ".$log_file) ;
        exec("chmod 666 ".$log_file);
    }
    clearstatcache() ;
    if(is_file($log_file) && !is_writable($log_file)) 
    {
        exec("touch ".$log_file) ;
        exit("没有权限操作日志文件：".$log_file) ;
    }
    $fp = fopen($log_file, "a+b");
    if($fp) 
    {
        $level_str = "";
        $line_str = "";
        $func_str = "";
        if(!empty($level))
        {
            $level_str = "[{$level}]";
        }
        if(!empty($line))
        {
            $line_str = "[ln-{$line}]";
        }
        if(!empty($func))
        {
            $func_str = "[fn-{$func}()]";
        }
        $log_str = $time.$level_str.$time_str.$line_str.$func_str.$str."\n";
        fwrite($fp, $log_str) ;
        fclose($fp) ;
    } 
    return true;
}

?>
