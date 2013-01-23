/*
* =====================================================================================
*       @file  c_auto_ptr.h
*      @brief 提供相关自动管理指针类，这些类会自动释放传入的资源 
*
*  Detailed description starts here.
*
*   @internal
*     Created  01/22/2010 01:53:21 PM 
*    Revision  1.0.0.0
*    Compiler  gcc/g++
*     Company  TaoMee.Inc, ShangHai.
*   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
*
*     @author  jasonwang (王国栋), jasonwang@taomee.com
* This source code was wrote for TaoMee,Inc. ShangHai CN.
* =====================================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <pthread.h>

/** 
* @brief 将用户的MYSQL_RESULT指针交给类来处理，它会自动释放资源
*/
class c_mysql_res_auto_ptr
{
public:
	/** 
	* @brief 将用户的MYSQL_RESULT指针传给类
	* 
	* @param res MYSQL_RES结果集指针
	*/
	c_mysql_res_auto_ptr(MYSQL_RES* res);

	/** 
	* @brief 自动释放MYSQL_RESULT指针
	*/
	~c_mysql_res_auto_ptr();

	/** 
	* @brief 用户传入的指针与类脱离，不再由类来管理
	* 
	* @return 成功返回0，失败返回-1 
	*/
	int detach();

	/** 
	* @brief 释放类管理的指针
	* 
	* @return 成功返回0，失败返回-1
	*/
	int free();

	/** 
	* @brief 将类管理的指针返回给用户
	* 
	* @return 类管理的指针 
	*/
	MYSQL_RES* get_ptr();

private:

	c_mysql_res_auto_ptr(c_mysql_res_auto_ptr &c_ptr) {}

	/** 
	* @brief 保存用户传入的指针
	*/
	MYSQL_RES* m_res;
};

/** 
* @brief 将用户的文件描述符交给类来处理，它会自动释放资源
*/
class c_fd_auto_ptr
{
public:
	/**
	* @brief 将用户的文件描述符传给类
	* 
	* @param fd 文件描述符
	*/
	c_fd_auto_ptr(int);

	/**
	* @brief 自动释放文件描述符
	*/
	~c_fd_auto_ptr();

	/**
	* @brief 用户传入的文件描述符与类脱离，不再由类来管理
	*
	* @return 成功返回0，失败返回-1
	*/
	int detach();

	/**
	* @brief 关闭类管理的文件描述符
	*
	* @return 成功返回0，失败返回-1
	*/
	int free();

	/**
	* @brief 获取类管理的文件描述符
	*
	* @return 文件描述符
	*/
	int get_fd();

private:

	c_fd_auto_ptr(c_fd_auto_ptr &c_ptr) {}

	/**
	* @brief 保存用户传入的文件描述符
	*/
	int m_fd;
};

/** 
* @brief 将用户的抽象数据类型指针交给类来处理，它会自动释放资源
*/
template<typename T> 
class c_obj_auto_ptr
{
public:

	/** 
	* @brief 将用户的抽象数据类型指针传给类
	* 
	* @param ptr 抽象数据类型指针
	*/
	c_obj_auto_ptr(T* ptr)
	{
		m_ptr = ptr;
	}

	/** 
	* @brief 自动释放抽象数据类型指针
	*/
	~c_obj_auto_ptr()
	{
		free();
	}

	/** 
	* @brief 用户传入的指针与类脱离，不再由类来管理
	* 
	* @return 成功返回0，失败返回-1
	*/
	int detach()
	{
		if (m_ptr != NULL) {
			m_ptr = NULL;
			return 0;
		}
		return -1;
	}

	/** 
	* @brief 释放类管理的指针
	* 
	* @return 成功返回0，失败返回-1
	*/
	int free() 
	{
		if (m_ptr != NULL) {
			delete m_ptr;
			m_ptr = NULL;
			return 0;
		}

		return -1;
	}

	/** 
	* @brief 将类管理的指针返回给用户
	* 
	* @return 类管理的指针 
	*/
	T* get_ptr()
	{
		return m_ptr;
	}

private:

	c_obj_auto_ptr(c_obj_auto_ptr &c_ptr) {}

	/** 
	* @brief 保存用户传入的指针
	*/
	T* m_ptr;
};

/** 
* @brief 将用户的动态分配内存指针交给类来处理，它会自动释放资源
*/
class c_malloc_auto_ptr
{
public:
	/** 
	* @brief 将用户的动态分配内存指针传给类
	* 
	* @param ptr 用户动态分配内存指针
	*/
	c_malloc_auto_ptr(void* ptr);

	/** 
	* @brief 自动释放用户动态分配内存指针
	*/
	~c_malloc_auto_ptr();

	/** 
	* @brief 用户传入的指针与类脱离，不再由类来管理
	* 
	* @return 成功返回0，失败返回-1 
	*/
	int detach();

	/** 
	* @brief 释放类管理的指针
	* 
	* @return 成功返回0，失败返回-1
	*/
	int free();

	/** 
	* @brief 将类管理的指针返回给用户
	* 
	* @return 类管理的指针 
	*/
	void* get_ptr();

private:

	c_malloc_auto_ptr(c_malloc_auto_ptr &c_ptr) {}

	/** 
	* @brief 保存用户传入的指针
	*/
	void* m_ptr;
};

/** 
* @brief 将用户的内存映射指针交给类来处理，它会自动释放资源
*/
class c_mmap_auto_ptr
{
public:
	/** 
	* @brief 将用户的内存映射指针传给类
	* 
	* @param map 内存映射指针
	*
	* @param sz 内存映射的字节数
	*/
	c_mmap_auto_ptr(void* map, size_t sz);

	/** 
	* @brief 自动释放内存映射指针
	*/
	~c_mmap_auto_ptr();

	/** 
	* @brief 用户传入的指针与类脱离，不再由类来管理
	* 
	* @return 成功返回0，失败返回-1 
	*/
	int detach();

	/** 
	* @brief 释放类管理的指针
	* 
	* @return 成功返回0，失败返回-1
	*/
	int free();

	/** 
	* @brief 将类管理的指针返回给用户
	* 
	* @return 类管理的指针 
	*/
	void* get_ptr();

private:

	c_mmap_auto_ptr(c_mmap_auto_ptr &c_ptr) {}

	/** 
	* @brief 保存用户传入的指针
	*/
	void* m_map;

	/** 
	* @brief 保存内存映射的字节数
	*/
	size_t m_sz;
};

/** 
* @brief 将用户的线程互斥锁指针交给类来处理，它会自动释放资源
*/
class c_pthread_mutex_auto_ptr
{
public:
	/** 
	* @brief 将用户的线程互斥锁指针传给类
	* 
	* @param mtx 线程互斥锁指针
	*/
	c_pthread_mutex_auto_ptr(pthread_mutex_t *mtx);

	/** 
	* @brief 自动释放线程互斥锁指针
	*/
	~c_pthread_mutex_auto_ptr();

	/** 
	* @brief 用户传入的指针与类脱离，不再由类来管理
	* 
	* @return 成功返回0，失败返回-1 
	*/
	int detach();

	/** 
	* @brief 释放类管理的指针
	* 
	* @return 成功返回0，失败返回-1
	*/
	int free();

	/** 
	* @brief 将类管理的指针返回给用户
	* 
	* @return 类管理的指针 
	*/
	pthread_mutex_t *get_ptr();

private:

	c_pthread_mutex_auto_ptr(c_pthread_mutex_auto_ptr &c_ptr) {}

	/** 
	* @brief 保存用户传入的指针
	*/
	pthread_mutex_t *m_mtx;
};

/** 
* @brief 将用户的信号量描述符指针交给类来处理，它会自动释放资源
*/
class c_semaphore_auto_ptr
{
public:
	/** 
	* @brief 将用户的信号量描述指针传给类
	* 
	* @param sem 信号量描述符指针
	*/
	c_semaphore_auto_ptr(sem_t *sem);

	/** 
	* @brief 自动释放信号量
	*/
	~c_semaphore_auto_ptr();

	/** 
	* @brief 用户传入的指针与类脱离，不再由类来管理
	* 
	* @return 成功返回0，失败返回-1 
	*/
	int detach();

	/** 
	* @brief 释放类管理的指针
	* 
	* @return 成功返回0，失败返回-1
	*/
	int free();

	/** 
	* @brief 将类管理的指针返回给用户
	* 
	* @return 类管理的指针 
	*/
	sem_t *get_ptr();

private:

	c_semaphore_auto_ptr(c_semaphore_auto_ptr &c_ptr) {}

	/** 
	* @brief 保存用户传入的指针
	*/
	sem_t *m_sem;
};

