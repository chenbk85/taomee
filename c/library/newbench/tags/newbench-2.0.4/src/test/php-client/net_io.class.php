<?
class net_io
{
public function __construct()
{
    $this->sock = NULL;
    $this->addr = NULL;
    $this->port = NULL;

    $this->total_snd_time = 0;
    $this->total_rcv_time = 0;
}

public function __destruct()
{
    if ($this->sock) {
        error_log('net_io: uninited while destruct');
    }
}

////////////////////////////// 公开 //////////////////////////////
public function init()
{
    return true;
}

public function uninit()
{
    return true;
}

public function connect($addr, $port)
{
    if ((strlen($addr) == 0) || ($port == 0)) {
        error_log('net_io: error parameter, init failed.');
        return false;
    }

    if (! $this->sock) {
        $this->sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
        if (! $this->sock) {
            error_log('net_io: create socket fail');
            return false;
        }
    }

    if(! socket_connect($this->sock, $addr, $port)) {
        error_log('net_io: connect to '.$addr.'('.$port.') fail');
        return false;
    }

    $this->addr = $addr;
    $this->port = $port;

    return true;
}

public function close()
{
    if ($this->sock) {
        socket_close($this->sock);
        $this->sock = NULL;
    }

    return true;
}

public function send($req)
{
    if (! $this->sock) {
        error_log('net_io: not inited');
        return false;
    }

    $time_s = $this->microtime_float();
    $rst = socket_write($this->sock, $req, strlen($req));
    if($rst === false) {
        error_log('net_io: socket write to '.$this->addr.'('.$this->port.') fail');
        return false;
    }
    $this->total_snd_time += $this->microtime_float() - $time_s;

    if($rst != strlen($req)) {
        error_log('net_io: socket writed len '.$rst.', not equal to '.strlen($req));
        return false;
    }

    //error_log('net_io: socket send success, sended '.$rst.' bytes');
    return $rst;
}

public function recv($ack_len)
{
    if (! $this->sock) {
        error_log('net_io: not inited');
        return false;
    }

    $byte_rcved = 0;
    $ack = NULL;
    while (true) {
        $time_s = $this->microtime_float();
        $rst = socket_read($this->sock, $ack_len - $byte_rcved, PHP_BINARY_READ);
        $this->total_rcv_time += $this->microtime_float() - $time_s;
        if(($rst === false) || (strlen($rst) <= 0)) {
            error_log('net_io: socket read from '.$this->addr.'('.$this->port.') fail');
            return false;
        }
        else {
            $ack .= $rst;
        }

        $byte_rcved += strlen($rst);
        if ($byte_rcved == $ack_len) {
            //error_log('net_io: rcv '.$ack_len.' succ, rcved '.$byte_rcved);
            break;
        }
    }

    return $ack;
}

public function get_total_snd_time()
{
    return $this->total_snd_time;
}

public function get_total_rcv_time()
{
    return $this->total_rcv_time;
}

////////////////////////////// 保护 ///////////////////////////
protected function net_cmd($req_bin, $ack_head_len, $ack_head_unpack_str, $pkg_len_name, $ack_body_len, $ack_body_unpack_str)
{
    /// 发送请求包
    $rst = $this->send($req_bin);
    if (! $rst) {
        return false;
    }

    /// 接收包头
    $ack = $this->recv_and_unpack($ack_head_len, $ack_head_unpack_str);
    if (! $ack) {
        error_log('net_io: recv head fail.');
        return false;
    }

    $body_len = $ack[$pkg_len_name] - $ack_head_len;
    if ($body_len === 0) {
        return $ack;
    }

    if ($body_len < $ack_body_len) {
        error_log('net_io: error body_len '.$body_len.' less than to '.$ack_body_len);
        return false;
    }

    /// 接收包体
    $body = $this->recv_and_unpack($ack_body_len, $ack_body_unpack_str);
    if (! $body) {
        error_log('net_io: recv body fail');
        return false;
    }

    return array_merge($ack, $body);
}

protected function microtime_float()
{
    list($usec, $sec) = explode(" ", microtime());
    return ((float)$usec + (float)$sec);
}

////////////////////////////// 私有 ///////////////////////////
protected function recv_and_unpack($recv_len, $unpack_str)
{
    if ($recv_len == 0) {
        return false;
    }

    /// 接收包头
    $bin = $this->recv($recv_len);
    if (strlen($bin) < $recv_len) {
        error_log('net_io: rcved '.strlen($bin).' len less than to '.$recv_len);
        return false;
    }

    /// 接收包体
    return unpack($unpack_str, $bin);
}

private $sock;
private $addr;
private $port;

private $total_snd_time;
private $total_rcv_time;
}

?>
