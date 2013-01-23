<?php
/**
 * socket类
 */
class socket_handler
{
    /**
     * socket实例
     * @access private
     * @var object
     */
    private $socket = NULL;

    /**
     * 连接
     * @access public
     * @param string $host
     * @param string $port
     * @return boolean
     */
    public function connect($host, $port)
    {
        /// 创建socket
        $this->socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
        if (false == $this->socket)
        {
            return false;
        }
        /// 链接主机
        if (!socket_connect($this->socket, $host, $port))
        {
            socket_close($this->socket);
            return false;
        }
        return true;
    }

    function __destruct()
    {
        $this->close();
    }

    /**
     * 关闭socket链接
     * @access public
     * @return NULL
     */
    public function close()
    {
        if (NULL != $this->socket)
        {
            socket_close($this->socket);
            $this->socket = NULL;
        }
    }

    /**
     * 发送数据
     * @access public
     * @param string $data
     * @return boolean
     */
    public function send($data)
    {
        $result = socket_write($this->socket, $data, strlen($data));
        if (strlen($data) != $result)
        {
            return false;
        }
        return true;
    }

    /**
     * 接收数据
     * @access public
     * @return false|string
     */
    public function receive()
    {
        $data = socket_read($this->socket, 1024 * 4, PHP_BINARY_READ);
        if (false == $data || strlen($data) <= 0)
        {
            return false;
        }
        return $data;
    }

    /**
     * 一步直接发送数据
     * @access public
     * @string $host
     * @string $port
     * @string $data
     * @return false|string
     */
    public function send2($host, $port, $data)
    {
        $result = $this->connect($host, $port);
        if (false === $result)
        {
            return false;
        }
        $result = $this->send($data);
        if (false === $result)
        {
            return false;
        }
        $result = $this->receive();
        if (false === $result)
        {
            return false;
        }
        $this->close();
        return $result;
    }
}
