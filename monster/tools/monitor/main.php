#!/usr/bin/php
<?php
require_once("./Rmail/Rmail.php");
$g_file_prefix="./data/";
$online_ip = "192.168.61.238";


$msg_arr[40101] = "注册协议";
$msg_arr[40102] = "登录协议";
$msg_arr[40103] = "拉取背包";
$msg_arr[40104] = "拉取房间信息";
$msg_arr[40105] = "更新房间buf";
$msg_arr[40106] = "拉取房间个数";
$msg_arr[40107] = "添加新房间";
$msg_arr[40108] = "购买商品";
$msg_arr[40109] = "拉取益智游戏信息";
$msg_arr[40110] = "提交益智游戏结果";
$msg_arr[40111] = "拉取好友列表";
$msg_arr[40112] = "拉取单个好友信息";
$msg_arr[40113] = "好友操作";
$msg_arr[40114] = "查找好友";
$msg_arr[40115] = "添加好友";
$msg_arr[40116] = "拉取好友申请";
$msg_arr[40117] = "对好友申请的操作";
$msg_arr[40118] = "拉取用户的留言板信息";
$msg_arr[40119] = "新增一条留言";
$msg_arr[40120] = "对留言的操作";
$msg_arr[40121] = "顶用户的小屋";
$msg_arr[40122] = "拉取种植园信息";
$msg_arr[40123] = "种植园种植物";
$msg_arr[40124] = "种植园铲除植物";
$msg_arr[40125] = "对吸引到的小怪兽的操作";


$yest = date("Ymd", time() - 24*60*60); //昨天

$timeout_arr_0_5; //[0,5)秒一个段
$timeout_arr_5_10;//[5,10)秒一个段
$timeout_arr_10;// [10,)超时的一个段

main();

function main()
{
    global $g_file_prefix;
    global $msg_arr;
    global $timeout_arr_0_5;
    global $timeout_arr_5_10;
    global $timeout_arr_10;
    global $yest;


    $dir_name = $g_file_prefix.$yest;

    if($dh = opendir($dir_name))
    {
        while(($file = readdir($dh)) !== false)
        {
            if(is_dir($file))
            {
                continue;
            }
            echo $file;
            echo "\n";
            $file = $dir_name.'/'.$file;
            parse_file($file);
            unset($file);
        }

        closedir($dh);
    }

   // print_r($timeout_arr_0_5);
   // print_r($timeout_arr_5_10);
   // print_r($timeout_arr_10);
    send_mail();
}

function parse_file($file_name)
{
    global $timeout_arr_0_5;
    global $timeout_arr_5_10;
    global $timeout_arr_10;

    $handle = fopen($file_name, "rb");
    do
    {
        unset($msg_id);
        unset($msg_time);

        $msg_id = fread($handle, 2);   //读取消息id
        if(strlen($msg_id) == 0)
        {
            break;
        }

        $msg_time = fread($handle, 1);  //读取处理时间
        if(strlen($msg_time) == 0)
        {
            break;
        }

        unset($msg_id_arr);
        unset($msg_time_arr);

        $msg_id_arr = unpack('S', $msg_id);
        $msg_time_arr = unpack('C', $msg_time);

        unset($msg_id);
        unset($msg_time);

        $msg_id = $msg_id_arr[1];
        $msg_time = $msg_time_arr[1];
       // echo "msg_id=".$msg_id."  msg_time=".$msg_time;
       // echo "\n";

        if($msg_time > 0 && $msg_time < 5)
        {
            $timeout_arr_0_5[$msg_id] += 1;
        }
        else if($msg_time >= 5 && $msg_time < 10)
        {
            $timeout_arr_5_10[$msg_id] += 1;
        }
        else
        {
            $timeout_arr_10[$msg_id] += 1;
        }

    }while(true);
     fclose($handle);
}

function send_mail()
{
    global $yest;
    global $msg_arr;
    global $timeout_arr_0_5;
    global $timeout_arr_5_10;
    global $timeout_arr_10;
    //读取邮件配置
    $xml = simplexml_load_file('./mail.xml');
    if($xml == false)
    {
        echo "parse mail.xml failed.";
        exit(-1);
    }

    $mail_host = (string)$xml->mail[0]->host;
    $mail_port = (string)$xml->mail[0]->port;
    $mail_from = (string)$xml->mail[0]->from;
    $mail_to   = (string)$xml->mail[0]->to;
    $mail_cc   = (string)$xml->mail[0]->cc;
    $mail_user = (string)$xml->mail[0]->user;
    $mail_passwd = (string)$xml->mail[0]->passwd;
    $mail_title = (string)$xml->mail[0]->title."--".$yest."(".$online_ip.")";

    $mail_body = "";
    $mail_body .= "<h4 align = 'center'>调用超过10s的接口</h4>";
    $mail_body .= "<table border = '2' align = 'center'>";
    $mail_body  .= "<tr><td>协议号</td><td>协议名称</td><td>次数</td></tr>";
    foreach($timeout_arr_10 as $msg_id => $count)
    {
        $mail_body  .= "<tr><td>".$msg_id."</td><td>". $msg_arr[$msg_id] ."</td><td>". $count ."</td></tr>";
    }
    $mail_body .= "</table>";



    $mail_body .= "</br>";
    $mail_body .= "<h4 align = 'center'>调用处于5到10s(不包括10s)的接口</h4>";
    $mail_body .= "<table border = '2' align = 'center'>";
    $mail_body  .= "<tr><td>协议号</td><td>协议名称</td><td>次数</td></tr>";
    foreach($timeout_arr_5_10 as $msg_id => $count)
    {
        $mail_body  .= "<tr><td>".$msg_id."</td><td>". $msg_arr[$msg_id] ."</td><td>". $count ."</td></tr>";
    }
    $mail_body .= "</table>";

    $mail_body .= "</br>";
    $mail_body .= "<h4 align = 'center'>调用处于1到5s(不包括5s)的接口</h4>";
    $mail_body .= "<table border = '2' align = 'center'>";
    $mail_body  .= "<tr><td>协议号</td><td>协议名称</td><td>次数</td></tr>";
    foreach($timeout_arr_0_5 as $msg_id => $count)
    {
        $mail_body  .= "<tr><td>".$msg_id."</td><td>". $msg_arr[$msg_id] ."</td><td>". $count ."</td></tr>";
    }
    $mail_body .= "</table>";


    $mail = new Rmail();
    $mail->setTextCharset('UTF-8');
    $mail->setHTMLCharset('UTF-8');
    $mail->setHeadCharset('UTF-8');
    $mail->setSMTPParams($mail_host, $mail_port, 'helo', false, $mail_user, $mail_passwd);
    $mail->setFrom($mail_from);
    $mail->setCc($mail_cc);
    $mail->setSubject($mail_title);
    $mail->setHTML($mail_body);


    if (!$mail->send(array($mail_to), 'smtp'))
    {
            echo "ERROR: failed to send mail";
    }

}

?>
