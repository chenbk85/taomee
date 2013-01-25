#!/usr/bin/php
<?php
require_once("log.php");
require_once("config.php");
require_once("function.php");
require_once("Rmail.php");

declare(ticks = 1);

$g_mail = NULL;
//保存报警信息
$g_alarm_event = array();
$g_continue_working = true;

function send_mobile_message($mobile_list, $message)
{
    $week_num = date("w");
    $today = date("Y-m-d");
    $work_start_time = strtotime($today . " 09:30");
    $work_end_time = strtotime($today . " 18:30");
    $now = time();
    //周一到周五的工作时间不发短信
    if ($week_num > 0 && $week_num < 6 && $now >= $work_start_time && $now <= $work_end_time)
    {
	    return ALARM_SUCC;
    }

    $mobile_list = discard_repeated($mobile_list);
    if(strlen($mobile_list) < MOBILE_MIN_LEN || strlen($message) < MSG_MIN_LEN)
    {
        $log = "mobile_list[{$mobile_list}] or message[{$message}] is too short.";
        do_log($log, "ERROR");
        return $log;
    }

    if(TEST_VERSION == '1')
    {
        $message .= "\nWolf comes, pls ignore^_^."; 
    }

    $fail_send_list = "";
    $contact_array = explode(',', $mobile_list);
    foreach($contact_array as $contact)
    {
        if(!is_mobile($contact))
        {
            $fail_send_list .= $contact.",";
            continue;
        }
        $msg_content = substr($message, 0, MOBILE_MSG_MAX_LEN);
        $url_prefix = MOBILE_MESSAGE_URL . "mobile={$contact}&msg=";
        
        $rval = file_get_contents($url_prefix . urlencode(mb_convert_encoding($msg_content, 'gb2312', 'utf8')));
        if(false === $rval)
        {
            $log = "Fail to send message[{$msg_content}] to {$contact}";
            do_log($log, "ERROR");
            $fail_send_list .= $contact.",";
        }
    }

    if ($fail_send_list == "")
    {
        return ALARM_SUCC;
    }

    return "Fail to send to mobile_list[{$fail_send_list}]";
}

function send_email_message($email_list, $message)
{
    $mail_handler = new Rmail();
    $mail_handler->setTextCharset('UTF-8');
    $mail_handler->setHTMLCharset('UTF-8');
    $mail_handler->setHeadCharset('UTF-8');
    $mail_handler->setSMTPParams(SMTP_HOST, 25, 'helo', false, '', '');
    $mail_handler->setFrom(SMTP_FROM_EMAIL);
    $mail_handler->setSubject(EMAIL_TITLE);

    $email_list = discard_repeated($email_list);
    if(strlen($email_list) < EMAIL_MIN_LEN || strlen($message) < MSG_MIN_LEN)
    {
        $log = "email_list[{$email_list}] or message[{$message}] is too short.";
        do_log($log, "ERROR");
        return $log;
    }

    if(TEST_VERSION == '1')
    {
        $message .= "\nWolf comes, pls ignore^_^."; 
    }

    $mail_handler->setHTML("ITL监控系统报警，报警内容:\n" . $message);
    if(!$mail_handler->send(array($email_list), 'smtp'))
    {
        $log = "Fail to send email[{$message}] to [{$email_list}]";
        do_log($log, "ERROR");
        return $log;
    } 

    return ALARM_SUCC;
}

function send_rtx_message($rtx_list, $message)
{
    $rtx_list = discard_repeated($rtx_list);
    if(strlen($rtx_list) < RTX_MIN_LEN || strlen($message) < MSG_MIN_LEN)
    {
        $log = "rtx_list[{$rtx_list}] or message[{$message}] is too short.";
        do_log($log, "ERROR");
        return $log;
    }

    if(TEST_VERSION == '1')
    {
        $message .= "\nWolf comes, pls ignore^_^."; 
    }

    $msg_info = RTX_MESSAGE_URL .
        "?msg=" . urlencode(mb_convert_encoding($message, 'gb2312', 'utf8')) .
        "&receiver=" . $rtx_list;

    $rval = file_get_contents($msg_info);
    if(false === $rval)
    {
        $log = "Fail to send rtx msg[{$message}] to [{$rtx_list}]";
        do_log($log, "ERROR");
        return $log;
    }

    return ALARM_SUCC;
}

?>
