/**
* =====================================================================================
*       @file  c_auto_ptr.cpp
*      @brief  提供相关自动管理指针类，这些类会自动释放传入的资源 
*
*  Detailed description starts here.
*
*   @internal
*     Created  01
*    Revision  1.0.0.0
*    Compiler  gcc
*     Company  TaoMee.Inc, ShangHai.
*   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
*
*     @author  jasonwang (王国栋), jasonwang@taomee.com
* This source code was wrote for TaoMee,Inc. ShangHai CN.
* =====================================================================================
*
*/
#include "c_auto_ptr.h"

c_mysql_res_auto_ptr::c_mysql_res_auto_ptr(MYSQL_RES* res)
{
	m_res = res;
}

c_mysql_res_auto_ptr::~c_mysql_res_auto_ptr()
{
	//free();
	puts("destruct ptr");
}

int c_mysql_res_auto_ptr::detach()
{
	if (m_res != NULL) {
		m_res = NULL;
		return 0;
	}
	return -1;
}

int c_mysql_res_auto_ptr::free()
{
	if (m_res != NULL) {
		mysql_free_result(m_res);
		m_res = NULL;
		return 0;
	}
	return -1;
}

MYSQL_RES* c_mysql_res_auto_ptr::get_ptr()
{
	return m_res;
}

c_fd_auto_ptr::c_fd_auto_ptr(int fd)
{
	m_fd = fd;
}

c_fd_auto_ptr::~c_fd_auto_ptr()
{
	free();
}

int c_fd_auto_ptr::detach()
{
	if (m_fd != -1) {
		m_fd = -1;
		return 0;
	}

	return -1;
}

int c_fd_auto_ptr::free()
{
	if (m_fd != -1) {
		close(m_fd);
		m_fd = -1;
		return 0;
	}

	return -1;
}

int c_fd_auto_ptr::get_fd()
{
	return m_fd;
}

c_malloc_auto_ptr::c_malloc_auto_ptr(void* ptr)
{
	m_ptr = ptr;
}

c_malloc_auto_ptr::~c_malloc_auto_ptr()
{
	free();
}

int c_malloc_auto_ptr::detach()
{
	if (m_ptr != NULL) {
		m_ptr = NULL;
		return 0;
	}

	return -1;
}

int c_malloc_auto_ptr::free()
{
	if (m_ptr != NULL) {
		::free(m_ptr);
		m_ptr = NULL;
		return 0;
	}

	return -1;
}

void* c_malloc_auto_ptr::get_ptr()
{
	return m_ptr;
}

c_mmap_auto_ptr::c_mmap_auto_ptr(void* map, size_t sz)
{
	m_map = map;
	m_sz = sz;
}

c_mmap_auto_ptr::~c_mmap_auto_ptr()
{
	free();
}

int c_mmap_auto_ptr::detach()
{
	if (m_map != NULL) {
		m_map = NULL;
		return 0;
	}

	return -1;
}

int c_mmap_auto_ptr::free()
{
	if (m_map != NULL) {
		munmap(m_map, m_sz);
		m_map = NULL;
		return 0;
	}

	return -1;
}

void* c_mmap_auto_ptr::get_ptr()
{
	return m_map;
}

c_pthread_mutex_auto_ptr::c_pthread_mutex_auto_ptr(pthread_mutex_t *mtx)
{
	m_mtx = mtx;
}

c_pthread_mutex_auto_ptr::~c_pthread_mutex_auto_ptr()
{
	this->free();
}

int c_pthread_mutex_auto_ptr::detach()
{
	if (m_mtx != NULL) {
		m_mtx = NULL;
		return 0;
	}
	return -1;
}

int c_pthread_mutex_auto_ptr::free()
{
	if (m_mtx != NULL && pthread_mutex_destroy(m_mtx)) {
		m_mtx = NULL;
		return 0;
	}
	return -1;
}

pthread_mutex_t* c_pthread_mutex_auto_ptr::get_ptr() 
{
	return m_mtx;
}

c_semaphore_auto_ptr::c_semaphore_auto_ptr(sem_t *sem)
{
	m_sem = sem;
}

c_semaphore_auto_ptr::~c_semaphore_auto_ptr()
{
	this->free();
}

int c_semaphore_auto_ptr::detach()
{
	if (m_sem != NULL) {
		m_sem = NULL;
		return 0;
	}
	return -1;
}

int c_semaphore_auto_ptr::free()
{
	if (m_sem != NULL && sem_close(m_sem)) {
		m_sem = NULL;
		return 0;
	}
	return -1;
}

sem_t* c_semaphore_auto_ptr::get_ptr()
{
	return m_sem;
}
