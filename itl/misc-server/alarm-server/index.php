<?php
require_once("alarm.php");
require_once("log.php");

if (empty($_POST["cmd"]))
{
    $log = "request command cannot be NULL";
    do_log($log, "ERROR");
    echo $log;
    return;
}

$handle_result = "succ";

switch ($_POST["cmd"])
{
case COMMAND_SEND_TO_MOBILE://mobile
    $log = "cmd_id[{$_POST['cmd']}], mobile_list[{$_POST['contact']}], msg[{$_POST['msg']}]";
    do_log($log, "DEBUG");
    $handle_result = send_mobile_message($_POST['contact'], $_POST['msg']);
    break;
case COMMAND_SEND_TO_EMAIL://email
    $log = "cmd_id[{$_POST['cmd']}], email_list[{$_POST['contact']}], msg[{$_POST['msg']}]";
    do_log($log, "DEBUG");
    $handle_result = send_email_message($_POST['contact'], $_POST['msg']);
    break;
case COMMAND_SEND_TO_RTX://rtx
    $log = "cmd_id[{$_POST['cmd']}], rtx_list[{$_POST['contact']}], msg[{$_POST['msg']}]";
    do_log($log, "DEBUG");
    $handle_result = send_rtx_message($_POST['contact'], $_POST['msg']);
    break;
default:
    $log = "cannot support cmd_id: {$_POST['cmd']}";
    do_log($log, "ERROR");
    $handle_result = $log;
    break;
}

echo $handle_result;

?>
