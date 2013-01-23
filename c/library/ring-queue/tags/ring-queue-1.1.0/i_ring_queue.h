/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file i_ring_queue.h
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
	 * @brief pop data from buffer as much as possible
	 * @param p_recv_buffer receive buffer
	 * @param buffer_len receive buffer length
	 * @param timeout 超时值，如果timeout等于0，则不等待立即返回；如果timeout等于-1，则无限等待
	 * @return data length actually poped(0 is valid), -1 if error occur
	 */
    virtual int pop_data(char *p_recv_buffer, int buffer_len, int timeout = 0) = 0;

	/**
	 * @brief same as pop_data(), expect the read index not moved
	 * @param p_recv_buffer
	 * @param buffer_len
	 * @param timeout 超时值，如果timeout等于0，则不等待立即返回；如果timeout等于-1，则无限等待
	 */
    virtual int pop_data_dummy(char *p_recv_buffer, int buffer_len, int timeout = 0) = 0;
  
	/**
	 * @brief get the buffer length
	 */
	virtual int get_buffer_len() = 0;
	
	/**
	 * @brief get current data length in buffer(snapshot)
	 * @return data length in buffer
	 */
    virtual int get_data_len() = 0;
   
	/**
	 * @brief get empty buffer len(snapshot)
	 * @return empty buffer length in buffer
	 */
	virtual int get_empty_buffer_len() = 0;
   
	/**
	 * @brief get last error
	 * @return the last error
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

/**
 * @brief 使用装饰者模式，对ring_queue增加功能，返回新的支持多个读进程的ring_queue
 */
i_ring_queue * create_multipopable_queue_instance(i_ring_queue *p_ring_queue);

#endif //I_RING_QUEUE_H_2010_03_22

