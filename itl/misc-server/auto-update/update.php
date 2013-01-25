<?php
require_once("log.php");
require_once("config.php");



function check_md5($file_info)
{
    $file_info["update"] = false;
    if (array_key_exists("prior", $file_info))
    {
        $file_full_name = $file_info["dir"] . "/" . $file_info["prior"];
        if (!file_exists($file_full_name))
        {
            $file_full_name = $file_info["dir"] . "/" . $file_info["filename"];
        }
    }
    else
    {
        $file_full_name = $file_info["dir"] . "/" . $file_info["filename"];
    }



    $fd = @fopen($file_full_name, "r");
    if (!$fd)
    {
        do_log("failed to open file[{$file_full_name}], take as no-update", "ERROR");
        return $file_info;
    }
    else
    {
        if (flock($fd, LOCK_EX))
        {
            $file_info["local_md5"] = md5_file($file_full_name); 
            flock($fd, LOCK_UN);
        }
        fclose($fd);

    }

    if ($file_info["local_md5"] != $file_info["remote_md5"])
    {
        $file_info["update"] = true;
        $file_info["download_file"] = $file_full_name;
    }

    return $file_info;
}

function parse_info($file_info, $dir)
{
    $info = explode(";", trim($file_info));
    $arr = array();
    $index = 0;
    for ($index = 0; $index < count($info); $index += 2)
    {
        $file_name = $info[$index];
        $file_md5 = $info[$index + 1];
        if (strlen($file_name) == 0 || strlen($file_md5) != 32)
        {
            $log = "wrong info({$file_name}): {$file_md5}";
            do_log($log, "ERROR");
            break;
        }

        $file_arr = array(
            "filename" => $file_name,
            "remote_md5" => $file_md5,
            "dir" => $dir,
        );

        $arr[$file_name] = $file_arr;

    }

    return $arr;
}

function check($file_array)
{

    foreach ($file_array as $i => $file)
    {
        $file = check_md5($file);
        $file_array[$i] = $file;
    }
    return $file_array;
}


function generate_xml($file_array, $doc, $root)
{
    foreach ($file_array as $i => $file)
    {
        if ($file["update"])
        {
            $file_node = $doc->createElement($file["filename"]);
            $md5_value = $doc->createAttribute("md5");
            $md5_value->appendChild($doc->createTextNode($file["local_md5"]));
            $file_node->appendChild($md5_value);
            $download_filename = $doc->createAttribute("download_filename");
            $download_filename->appendChild($doc->createTextNode($file["download_file"]));
            $file_node->appendChild($download_filename);
            $root->appendChild($file_node);

        }

    }

}


function check_update($update_info)
{
    $os_type        = $update_info["os"];
    $host_ip        = $update_info["ip"];

    $bin_info       = $update_info["bin"];
    $so_info        = $update_info["so"];
    $conf_info      = $update_info["conf"];
    $script_info    = $update_info["script"];



    if (WHITE_SWITCH == "on")
    {
        $allow_host_array = explode(",", trim(ALLOW_HOST));
        if (!in_array($host_ip, $allow_host_array))
        {
            do_log("Host[{$host_ip}] is not in the white list.", "DEBUG");
            return;
        }
    }


    $bin_arr = parse_info($bin_info, BIN_DIR . $os_type);
    $so_arr = parse_info($so_info, SO_DIR . $os_type);
    $conf_arr = parse_info($conf_info, CONF_DIR);
    $script_arr = parse_info($script_info, SCRIPT_DIR);

    if (array_key_exists("bench.conf", $conf_arr))
    {
        $conf_arr["bench.conf"]["prior"] = $host_ip;
    }


    $bin_arr = check($bin_arr);
    $so_arr =check($so_arr);
    $conf_arr = check($conf_arr);
    $script_arr = check($script_arr);

    $doc = new DomDocument('1.0');
    $doc->formatOutput = true;
    $root = $doc->appendChild($doc->createElement("node"));
    $ip_value = $doc->createAttribute("ip");
    $ip_value->appendChild($doc->createTextNode($host_ip));
    $root->appendChild($ip_value);
    $os_value = $doc->createAttribute("os");
    $os_value->appendChild($doc->createTextNode($os_type));
    $root->appendChild($os_value);

    generate_xml($bin_arr, $doc, $root);
    generate_xml($so_arr, $doc, $root);
    generate_xml($conf_arr, $doc, $root);
    generate_xml($script_arr, $doc, $root);


    $update_xml = $doc->saveXML();

    do_log($update_xml, "UPDATE");
    return $update_xml;
}

?>
