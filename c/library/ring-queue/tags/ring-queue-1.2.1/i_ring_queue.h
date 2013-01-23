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
	 * @brief 把数据压入缓冲区
	 * @param p_data 要压入的数据
	 * @param data_len 要压入的数据的长度
	 * @param is_atomic 压入操作是否是原子的
	                    如果is_atomic等于1，则数据要么全部压入，要么全部不压入
			  		    如果is_atomic等于0，则压入尽可能多的数据
	 * @return 成功返回实际压入的数据的长度，出错返回-1
	 */
    virtual int push_data(const char *p_data, int data_len, int is_atomic) = 0;
	
	/**
	 * @brief 从缓冲区中取数据
	 * @param p_recv_buffer 接收数据的缓存，如果p_recv_buffer等于NULL，则只移动读指针，不拷贝数据
	 * @param buffer_len 接收数据缓存的长度
	 * @param timeout 超时值，如果timeout等于0，则不等待立即返回；
	 *                        如果timeout等于-1，则无限等待，直到有数据为止；
	 *                        如果timeout大于0，则最长等待相应的毫秒数
	 * @return 成功返回从缓冲区中取到的数据的长度，失败返回-1(0是有效的返回值，表示现在缓冲区中无数据)
	 * @note timeout在ring_queue的可等待的实例中才有效，否则必须为0
	 */
    virtual int pop_data(char *p_recv_buffer, int buffer_len, int timeout = 0) = 0;

	/**
	 * @brief 从缓冲区中取数据(不移动读指针)
	 * @param p_recv_buffer 接收数据的缓存
	 * @param buffer_len 接收数据缓存的长度
	 * @param timeout 超时值，如果timeout等于0，则不等待立即返回；
	 *                        如果timeout等于-1，则无限等待，直到有数据为止；
	 *                        如果timeout大于0，则最长等待相应的毫秒数
	 * @return 成功返回从缓冲区中取到的数据的长度，失败返回-1(0是有效的返回值，表示现在缓冲区中无数据)
	 * @note timeout在ring_queue的可等待的实例中才有效，否则必须为0
	 */
    virtual int pop_data_dummy(char *p_recv_buffer, int buffer_len, int timeout = 0) = 0;
  
	/**
	 * @brief 获取缓冲区的长度
	 * @return 成功返回ring_queue的长度，失败返回-1
	 */
	virtual int get_buffer_len() = 0;
	
	/**
	 * @brief 获取ring_queue中数据的长度
	 * @return 成功返回ring_queue中数据的长度，失败返回-1
	 */
    virtual int get_data_len() = 0;
   
	/**
	 * @brief 获取ring_queue中空闲存储的长度
	 * @return 成功返回ring_queue中空闲存储的长度，失败返回-1
	 */
	virtual int get_empty_buffer_len() = 0;
   
	/**
	 * @brief 获取最后一次错误的错误码
	 * @return 返回最后一次错误的错误码，当没有错误时返回0
	 */
	virtual int get_last_errno() = 0;
   
	/**
	 * @brief 获取最后一次错误的错误描述
	 * @return 返回最后一次错误的错误描述，当没有错误时返回NULL
	 */
	virtual const char * get_last_errstr() = 0;

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
 * @brief 为ring_queue添加等待功能
 * @param p_ring_queue ring_queue的实例
 * @return 成功返回具有等待功能的新的ring_queue的实例，失败返回NULL 
 */
i_ring_queue * create_waitable_queue_instance(i_ring_queue *p_ring_queue);

/**
 * @brief 为ring_queue添加多进程读取功能
 * @param p_ring_queue ring_queue的实例
 * @return 成功返回具有多进程读取功能的新的ring_queue的实例，失败返回NULL 
 */
i_ring_queue * create_multipopable_queue_instance(i_ring_queue *p_ring_queue);

#endif //I_RING_QUEUE_H_2010_03_22

