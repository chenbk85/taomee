<?
require_once('net_io.class.php');

class stubsys extends net_io
{
protected function do_cmd($cmd_id, $user_id, $body_bin, $ack_body_len, $ack_body_unpack_str)
{
    $pkg_head_len = 18;
    $pkg_len = $pkg_head_len + strlen($body_bin);
    $req_bin = pack('LLSLL', $pkg_len, 0, $cmd_id, 0, $user_id).$body_bin;

    return $this->net_cmd($req_bin, $pkg_head_len, 'Lpkg_len/Lseq_num/Scmd_id/Lresult/Luser_id', 'pkg_len', $ack_body_len, $ack_body_unpack_str);;
}

protected function do_cmd_no_recv($cmd_id, $user_id, $body_bin)
{
    $pkg_head_len = 18;
    $pkg_len = $pkg_head_len + strlen($body_bin);
    $req_bin = pack('LLSLL', $pkg_len, 0, $cmd_id, 0, $user_id).$body_bin;

    return $this->send($req_bin);;
}

public function do_echo($user_id, $val)
{
    $body_bin = pack('a256', $val);
    return $this->do_cmd(0x1001, $user_id, $body_bin, 256, 'a256val');
}

public function do_filter($user_id, $need_filter, $sleep_cnt, $val)
{
    $body_bin = pack('LLa32', $need_filter, $sleep_cnt, $val);
    return $this->do_cmd(0x1002, $user_id, $body_bin, 32, 'a32val');
}

public function do_filter_no_recv($user_id, $need_filter, $sleep_cnt, $val)
{
    $body_bin = pack('LLa32', $need_filter, $sleep_cnt, $val);
    return $this->do_cmd_no_recv(0x1002, $user_id, $body_bin);
}

public function do_filter_recv()
{
    return $this->recv_and_unpack(50, 'Lpkg_len/Lseq_num/Scmd_id/Lresult/Luser_id/a32val');
}
}

?>
