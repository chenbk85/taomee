<?
function list_dir($dir)
{
    $dfd = opendir($dir);
    if (! $dfd) {
        return false;
    }

    $idx = 0;
    $dir_ary = array();
    while($file = readdir($dfd)) {
        if(($file != '.') && ($file != '..')) {
            $dir_ary[$idx] = $file;
            $idx ++;
        }
    }

    return $dir_ary;
}

function get_proc_state($proc_name)
{
    if (! file_exists('./proc_state.sh')) {
        return false;
    }

    $lines = explode("\n", shell_exec('./proc_state.sh '.$proc_name));

    foreach ($lines as $idx => $line) {
        $data[$idx] = explode("\t", $line);
    }

    return $data;
}

function chnl_hash32($chnl_id, $key, $data)
{
    return md5('channelId='.$chnl_id.'&securityCode='.$key.'&data='.$data);
}

?>
