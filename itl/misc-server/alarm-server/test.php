#!/usr/bin/php
<?php
require_once("config.php");
require_once("Rmail.php");

$e1 = "f(n)=f(n-1)+f(n-2),f(1)=300,f(2)=600";
$e2 = "f(n)=f(n-1)*f(n-1),f(1)=30";
$e3 = "f(n)=f(n-1)+10,f(1)=30";
$e4 = "f(n)=f(n-1)*2,f(1)=25";
$e5 = "f(n)=10*2";
$e6 = "f(n)=n";
$e7 = "f(n)=1+n+f(n-1),f(1)=10";
$e8 = "f(n)=(n+1)+f(n-1),f(1)=10";
$e9 = "f(n)=(n+1)*(n-1)+f(n-1),f(1)=(10+2)";
$e10 = "f(n)=(n+1)*(n-1),f(2)=10";
$e11 = "f(n)=(n+1)*(n-1),f(n)=10";
$e12 = "f(1)=10";
$e13 = "f(n)=10*n + f(n-1) - f(n-2) , f(1)=9, f(2)=(12) ";

//$chinese_str='[[killyou]nlhl mason testing]';
//$RTX_URL = 'http://pdc.taomee-ex.com:8012/sendnotify.cgi';
//$msg_info = $RTX_URL . "?msg=" . urlencode($chinese_str) . "&receiver=ping";
//$rval = file_get_contents($msg_info);
//if($rval === false)
//{
//    do_log("send rtx msg failed!", "error");
//    return false;
//}
//return true;

//$str='masontestingVmasontestingVmasontestingVmasontestingVmasontestingvvvvvv';
//$MESSAGE_URL= 'http://192.168.6.19:8080/send_msg?sign=098f6bcd4621d373cade4e832627b4f6&';
//$msg_info = $MESSAGE_URL . "mobile=18616816316" . "&msg=" . $str;
//echo $msg_info . "\n";
//$rval = file_get_contents($msg_info);
//if($rval === false)
//{
//    echo "send msg failed! error\n";
//    return false;
//}
//echo "ok\n";
//return true;

$mobile = array(
1 => '18616816316',
2 => '13316816316',
3 => '13116816316',
4 => '13216816316',
5 => '15616816316',
6 => 'd15616816316',
7 => 'hlhkjds6316',
8 => 'hnkhk',
);
function is_mobile($str)
{       
    return preg_match("/^((\(\d{3}\))|(\d{3}\-))?1[358]\d{9}$/", $str);
}

foreach($mobile as $value)
{
    echo $value . " is" . (is_mobile($value) ? "" :" not") . " a mobile number\n";

}

function utf2ucs($str){
    $n=strlen($str);
    if ($n>=3) {
        $highCode = ord($str[0]); 
        $midCode = ord($str[1]);
        $lowCode = ord($str[2]);
        $a = 0x1F & $highCode;
        $b = 0x7F & $midCode;
        $c = 0x7F & $lowCode;
        $ucsCode = (64*$a + $b)*64 + $c; 
    }
    elseif ($n==2) {
        $highCode = ord($str[0]); 
        $lowCode = ord($str[1]);
        $a = 0x3F & $highCode; //0x3F是0xC0的补数
        $b = 0x7F & $lowCode; //0x7F是0x80的补数
        $ucsCode = 64*$a + $b; 
    }
    elseif($n==1) {
        $ucscode = ord($str);
    }
    return dechex($ucsCode);
}
//$bin = 1;
//$notbin = ~$bin;
//echo "Bin: " . decbin($bin) . "  !bin:  " . decbin($notbin) . "\n";
//echo "Bin: " . decbin($bin) . "  !bin:  " . decbin($notbin >> 1) . "\n";
//var_dump($notbin);
//$notbin = $notbin >> 1;
//echo "Bin: " . decbin($bin) . "  !bin:  " . decbin($notbin) . "\n";

//echo "~0 bin: " . decbin(~0). "\n";
//echo "max int bin: " . substr_replace(decbin(~0), "0", 0, 1). "\n";
//echo "max int dec:" . bindec(substr_replace(decbin(~0), "0", 0, 1)) . "\n";
//echo "1: " . decbin(1) . "\n";
//echo "-0: " . decbin(0) . "\n";
//var_dump($notbin);
//$i = 0;
//$noti = ~$i;
//echo $noti . "\n" ;
//$MAX_INT = intval(~-1 | 1);
//var_dump(intval(~-1));
//var_dump(~0>>1);
//var_dump($MAX_INT);
//for($i = 10; $i < 20; $i++)
//{
//    echo "公式:" . $e1 . "  n:$i" . "   值:" . f($e1, $i);
//    echo "\n";
//    echo "公式:" . $e2 . "  n:$i" . "   值:" . f($e2, $i);
//    echo "\n";
//    echo "公式:" . $e3 . "  n:$i" . "   值:" . f($e3, $i);
//    echo "\n";
//    echo "公式:" . $e4 . "  n:$i" . "   值:" . f($e4, $i);
//    echo "\n";
//    echo "公式:" . $e5 . "  n:$i" . "   值:" . f($e5, $i);
//    echo "\n";
//    echo "公式:" . $e6 . "  n:$i" . "   值:" . f($e6, $i);
//    echo "\n";
//    echo "公式:" . $e7 . "  n:$i" . "   值:" . f($e7, $i);
//    echo "\n";
//    echo "公式:" . $e8 . "  n:$i" . "   值:" . f($e8, $i);
//    echo "\n";
//    echo "公式:" . $e9 . "  n:$i" . "   值:" . f($e9, $i);
//    echo "\n";
//    echo "公式:" . $e10 . "  n:$i" . "   值:" . f($e10, $i);
//    echo "\n";
//    echo "公式:" . $e11 . "  n:$i" . "   值:" . f($e11, $i);
//    echo "\n";
//    echo "公式:" . $e12 . "  n:$i" . "   值:" . f($e12, $i);
//    echo "\n";
//    echo "公式:" . $e13 . "  n:$i" . "   值:" . f($e13, $i);
//    echo "\n";
//}


function f($expr, $n)
{
    $ret = 30;

    if($expr == NULL || $expr == "")
        $expr = "f(n)=f(n-1)+f(n-2),f(1)=300,f(2)=600";

    $expr = trim($expr);
    $expr_field = split(',', $expr);
    $field_count = count($expr_field);
    if($field_count == 0 || $filed_count > 4)
        return $ret;

    $is_has_value1 = 0;
    $is_has_value2 = 0;
    $is_has_value3 = 0;

    $value1 = 0;
    $value2 = 0;
    $value3 = 0;

    $matches = array();
    if(ereg("(f\(1\) {0,}= {0,})([\(\)\+\-\*\/0-9]+)", $expr, $matches))
    {
        $is_has_value1 = 1;
        eval("\$value1 = $matches[2];");
    }

    if(ereg("(f\(2\) {0,}= {0,})([\(\)\+\-\*\/0-9]+)", $expr, $matches))
    {
        $is_has_value2 = 1;
        eval("\$value2 = $matches[2];");
    }

    if(ereg("(f\(3\) {0,}= {0,})([\(\)\+\-\*\/0-9]+)", $expr, $matches))
    {
        $is_has_value3 = 1;
        eval("\$value3 = $matches[2];");
    }

    if($is_has_value3 == 1 && ($is_has_value1 == 0 || $is_has_value2 == 0))
    {
        return $ret;
    }

    if($is_has_value2 == 1 && $is_has_value1 == 0)
    {
        return $ret;
    }

    if($is_has_value1 == 0 && $is_has_value2 == 0 && $is_has_value3 == 0 && $field_count != 1)
    {
        return $ret;
    }

    switch($is_has_value1 + $is_has_value2 + $is_has_value3)
    {
    case 0:
        $formula = split('=', $expr_field[0]);
        $operations = preg_replace('/f\(n/', 'f($expr,$n', $formula[1]);
        //将$n或者n替换为$n
        $code = preg_replace('/[\$]*n/', '$n', $operations);
        eval("\$ret = $code;");
        break;
    case 1:
        if($n == 1)
        {
            $ret = $value1;
        }
        else 
        {
            $formula = split('=', $expr_field[0]);
            $operations = preg_replace('/f\(n/', 'f($expr,$n', $formula[1]);
            //将$n或者n替换为$n
            $code = preg_replace('/[\$]*n/', '$n', $operations);
            eval("\$ret = $code;");
        }
        break;
    case 2:
        if($n == 1)
        {
            $ret =  $value1;
        }
        else if($n == 2)
        {
            $ret = $value2;
        }
        else
        {
            $formula = split('=', $expr_field[0]);
            $operations = preg_replace('/f\(n/', 'f($expr,$n', $formula[1]);
            //将$n或者n替换为$n
            $code = preg_replace('/[\$]*n/', '$n', $operations);
            eval("\$ret = $code;");
        }
        break;
    case 3:
        if($n == 1)
        {
            $ret = $value1;
        }
        else if($n == 2) 
        {
            $ret =  $value2;
        }
        else if ($n == 3)
        {
            $ret =  $value3;
        }
        else 
        {
            $formula = split('=', $expr_field[0]);
            $operations = preg_replace('/f\(n/', 'f($expr,$n', $formula[1]);
            //将$n或者n替换为$n
            $code = preg_replace('/[\$]*n/', '$n', $operations);
            eval("\$ret = $code;");
        }
        break;
    default:
        break;	
    }
    return $ret;
}
