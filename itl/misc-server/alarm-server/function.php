<?php
//$a1='13232.00';
//$a2='132323232323.45';
//$a3='132323232341431.00';
//$a4='13232788.76';
//$a5='32.00';
//$a6='2.56';
//$a7='39979779789797972.00';
//$a8='25555555555555.56';
//$a9='32888439888888843343434343434.00';
//$a10='222222222222222222222222222222222.56';
//$a11='2268682222222222222222.56';
//
//echo 'convert:' . auto_indent($a1) . ' source:' . $a1 . "\n";
//echo 'convert:' . auto_indent($a2) . ' source:' . $a2 . "\n";
//echo 'convert:' . auto_indent($a3) . ' source:' . $a3 . "\n";
//echo 'convert:' . auto_indent($a4) . ' source:' . $a4 . "\n";
//echo 'convert:' . auto_indent($a5) . ' source:' . $a5 . "\n";
//echo 'convert:' . auto_indent($a6) . ' source:' . $a6 . "\n";
//echo 'convert:' . auto_indent($a7) . ' source:' . $a7 . "\n";
//echo 'convert:' . auto_indent($a8) . ' source:' . $a8 . "\n";
//echo 'convert:' . auto_indent($a9) . ' source:' . $a9 . "\n";
//echo 'convert:' . auto_indent($a10) . ' source:' . $a10 . "\n";
//echo 'convert:' . auto_indent($a11) . ' source:' . $a11 . "\n";

//传入一个十进制数的字符串
function auto_indent($src_val)
{
    if($src_val == NULL || $src_val == "") {
        return "";
    }

    if(!is_numeric($src_val)) {
        return "";
    }

    ///我们传过来的值都是保留两位小数的,如果不是保留两位小数则转之
    $tmp = sprintf("%01.2f", $src_val);
    ///如果本身是个整数后面有两位小数00给去掉
    $tmp = str_replace(".00", "", $tmp);
    if($tmp == '-1') {
        return "Unknown";
    }
    ///获取小数点前的位数
    if(($pos = strpos($tmp, ".")) === false) {
        $len_before_dot = strlen($tmp);
    } else {
        $len_before_dot = $pos;
    }
    ///三位以内不需要加单位了
    if($len_before_dot <= 3)
        return $tmp;

    if($len_before_dot > 3 && $len_before_dot < 7) {
        $tmp = sprintf("%01.2fK", $tmp / 1024);
    }
    else if($len_before_dot >= 7 && $len_before_dot < 10) {
        $tmp = sprintf('%01.2fM', $tmp / (1024*1024));
    }
    else if($len_before_dot >= 10 && $len_before_dot < 13) {
        $tmp = sprintf('%01.2fG', $tmp / (1024*1024*1024));
    }
    else if($len_before_dot >= 13 && $len_before_dot < 16) {
        $tmp = sprintf('%01.2fT', $tmp / (1024*1024*1024*1024));
    }
    else if($len_before_dot >= 16 && $len_before_dot < 19) {
        $tmp = sprintf('%01.2fP', $tmp / (1024*1024*1024*1024*1024));
    }
    else if($len_before_dot >= 19 && $len_before_dot < 22) {
        $tmp = sprintf('%01.2fE', $tmp / (1024*1024*1024*1024*1024*1024));
    }
    else if($len_before_dot >= 22 && $len_before_dot < 25) {
        $tmp = sprintf('%01.2fB', $tmp / (1024*1024*1024*1024*1024*1024*1024));
    }
    else {
        $tmp = $tmp[0] . '.' . $tmp[1] . $tmp[2] . 'e+' . ($len_before_dot - 1);
    }

    $tmp = str_replace(".00", "", $tmp);
    return $tmp;
}

function get_interval($range_ary_list, $n)
{
    $ret = 300;
    if($range_ary_list == NULL || $range_ary_list == "" || $n <= 0) {
        return $ret;
    }

    foreach($range_ary_list as $key => $value)
    {
        if($n >= $value['min'] && $n <= $value['max'])
        {
            $ret = $value['step'];
            break;
        }
    }
    return $ret;
}

//the ruler string is like "1:5,2-3:10,4-0:30" means the step of alarm count of 1 is 5 min, and 
//the step of alarm count from 2 to 3 is 10 min, 
//and the step of alarm count from 4 to positive_max_count is 30 min
function get_range_ary_list($str)
{
    $str = trim($str);
    $str_field = split(',', $str);
    if(count($str_field) <= 0) {
        do_log("The alarm ruler string is NULL,using the default range array list.", "error");
        global $DEFAULT_RANGE_ARY_LIST;
        return $DEFAULT_RANGE_ARY_LIST;
    }

    $range_ary_list = array();

    global $MAX_INT;
    $i = 0;
    foreach($str_field as $key => $value) {
        $range_ary = array();
        $fields = split(':', $value);
        if(count($fields) == 2) {
            $range_fields = split('-', $fields[0]);
            $range_fields_count = count($range_fields);
            if($range_fields_count == 1) {
                $range_ary['min'] = $range_ary['max'] = intval($range_fields[0]);
            } else if($range_fields_count == 2) {
                $range_ary['min'] = intval($range_fields[0]);
                if($range_fields[1] == '0') {//用0表示正的无穷大整数
                    $range_ary['max'] = $MAX_INT; 
                } else {
                    $range_ary['max'] = intval($range_fields[1]);
                }
            }
            else 
            {
                do_log("The alarm ruler string's format is not correct.", "error");
                continue;
            }
            //前端配置的单位是分钟
            $range_ary['step'] = intval($fields[1]) * 60;
            $range_ary_list[$i++] = $range_ary; 
        }
    }

    if(!check_range_ary_list($range_ary_list))
    {
        do_log("Check range array list failed,using the default range array list.", "error");
        unset($range_ary_list);
        global $DEFAULT_RANGE_ARY_LIST;
        return $DEFAULT_RANGE_ARY_LIST;
    }
    return $range_ary_list;
}

function is_cover($x1, $y1, $x2, $y2)
{
    //x1<=y1 且 x2<=y2 
    if($x2 > $y1 || $y2 < $x1) {
        return false;
    }

    return true;
}

function check_range_ary_list($range_ary_list)
{
    if($range_ary_list == NULL) {
        return false;
    }

    global $MAX_INT;
    $rang_cover = 0;//区间的覆盖
    foreach($range_ary_list as $key => $value)
    {
        //无效的区间
        if($value['max'] < $value['min']) {
            do_log("wrong sub range,max[{$value['max']}} < min[{$value['min']}].", "error");
            return false;
        }
        $rang_cover += ($value['max'] - $value['min'] + 1);
    }
    //区间没有完全覆盖
    if($rang_cover != $MAX_INT) {
        do_log("not all the range is covered.", "error");
        return false;
    }

    //检查各区间之间有没有重叠
    $i = $j = 0;
    for(; $i < count($range_ary_list); $i++) {
        for(; $j < count($range_ary_list); $j++) {
            //如果不同的区间存在重叠且步长不同，那么认为错误
            $mini = $range_ary_list[$i]['min'];
            $maxi = $range_ary_list[$i]['max'];
            $minj = $range_ary_list[$j]['min'];
            $maxj = $range_ary_list[$j]['max'];
            if($i != $j && is_cover($mini, $maxi,$minj, $maxj) &&
                $range_ary_list[$i]['step'] != $range_ary_list[$j]['step']) {
                do_log("Two sub range[{$mini}-{$maxi}, {$minj}-{$maxj}] covered.", "error");
                return false;
            }
        }
    }
    return true;
}

function is_mobile($str)
{       
    return preg_match("/^((\(\d{3}\))|(\d{3}\-))?1[358]\d{9}$/", $str);
}

/** 
 * @brief 将字符串中重复的部分剔除掉（如: 手机号、email、联系人英文名等）
 * 
 * @param $str_list: 以逗号分隔的字符串
 * 
 * @return 剔除重复部分的字符串
 */
function discard_repeated($str_list)
{
    $dst_str = "";
    $str_array = explode(',', trim($str_list));
    $key_array = array();
    foreach ($str_array as $str)
    {
        if (0 == strlen($str))
        {
            continue;
        }
        if (!isset($key_array[$str]))
        {
            $key_array[$str] = 1;
        }
        else
        {
            ++$key_array[$str];
        }
    }

    $log = "";
    foreach ($key_array as $str => $time)
    {
        if ($dst_str == "")
        {
            $dst_str .= $str;
        }
        else
        {
            $dst_str .= "," . $str;
        }
        if ($time > 1)
        {
            $log .= " [{$str} appear {$time}]";
        }
    }
    if ($log != "")
    {
        $log .= " in string[{$str_list}].";
        do_log($log, "ERROR");
    }

    return $dst_str;
}


?>
