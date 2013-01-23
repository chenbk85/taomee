/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file i_ring_queue.h
 * @brief 环形缓冲区接口,及其相关创建API
 * @author xml <xml@taomee.com>
 * @author richard <richard@taomee.com>
 * @date 2010-03-22
 */

#ifndef I_RING_QUEUE_H_2010_03_22
#define I_RING_QUEUE_H_2010_03_22

/**
 * @struct i_ring_queue
 * @brief 环状缓冲区接口类
 * @note 本接口不是多进程(线程)安全的，除非只有一个读进程(线程)、一个写进程(线程)
 */
struct i_ring_queue
{
public:
	/**
	 * @brief 初始化
	 * @param buffer_len 缓冲区的长度
	 * @return 成功返回0，失败返回－1
	 */
    virtual int init(int buffer_len) = 0;
	
	/**
	 * @brief 把数据压入到环状缓冲区中
	 * @param p_data 指向要压入的数据
	 * @param data_len 要压入的数据的长度
	 * @param is_atomic 压入操作是否是原子的
	                    如果is_atomic等于1，则数据要么全部压入，要么全部不压入
			  		    如果is_atomic等于0，则压入尽可能多的数据
	 * @return 成功时返回实际压入的数据的长度，出错返回-1
	 */
    virtual int push_data(const char *p_data, int data_len, int is_atomic) = 0;
	
	/**
	 * @brief 尽可能的从缓冲去取出数据
	 * @param p_recv_buffer 接收取出的数据的缓冲区
	 * @param buffer_len 接收数据缓冲区的长度
	 * @param timeout microseconds 等待超时时限, 单位毫秒
	 * @return 实际取出数据的长度，0是正常值，－1发生错误了。
	 */
    virtual int pop_data(char *p_recv_buffer, int buffer_len, int timeout = 0) = 0;

	/**
	 * @brief 和pop_data()相同，只不过这个函数不会删除本环形缓冲对象里的数据
	 * @param p_recv_buffer 接收取出的数据的缓冲区
	 * @param buffer_len 接收数据缓冲区的长度
	 * @param timeout microseconds 等待超时时限, 单位毫秒
	 * @return 实际取出数据的长度，0是正常值，－1发生错误了。
	 */
    virtual int pop_data_dummy(char *p_recv_buffer, int buffer_len, int timeout = 0) = 0;
  
	/**
	 * @brief 获取换新缓冲区的长度
	 */
	virtual int get_buffer_len() = 0;
	
	/**
	 * @brief 获取环形缓冲区内当前数据长度(快照，并非确切值)
	 * @return 数据区域长度值
	 */
    virtual int get_data_len() = 0;
   
	/**
	 * @brief 获取环形缓冲区内当前空区域长度(快照，并非确切值)
	 * @return 空区域长度值
	 */
	virtual int get_empty_buffer_len() = 0;
   
	/**
	 * @brief 获取最近一次错误信息， 非线程安全
	 * @return 错误码
	 */
	virtual int get_last_error() = 0;
    
	/**
	 * @brief 反初始化
	 * @return 成功返回0，失败返回－1
	 */
	virtual int uninit() = 0;
    
	/**
	 * @brief 释放自己
	 * @return 成功返回0，失败返回－1
	 */
	virtual int release() = 0;
};

/**
 * @brief 创建环状缓冲区实例
 * @param pp_instaance 输出参数，接收创建的环状缓冲的地址
 * @return 成功返回0，失败返回-1
 */
int create_ring_queue_instance(i_ring_queue **pp_instance);

/**
 * @brief 创建固定长度的环状缓冲区的实例
 * @param pp_instance 输出参数，接收创建的环状缓冲的地址
 * @param len 环状缓冲区中每个消息的长度
 * @return 成功返回0，失败返回-1
 */
int create_fixed_queue_instance(i_ring_queue **pp_instance, int len);

/**
 * @brief 创建可变长度的环状缓冲区的实例
 * @param pp_instance 输出参数，接收创建的环状缓冲区的地址
 * @param len 环状缓冲区中消息头部中表示消息长度的字节数(只能为2或者4)
 * @return 成功返回0，失败返回-1
 */
int create_variable_queue_instance(i_ring_queue **pp_instance, int len);

#endif //I_RING_QUEUE_H_2010_03_22

