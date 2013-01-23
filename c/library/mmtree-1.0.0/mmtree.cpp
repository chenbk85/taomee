/*
 * =====================================================================================
 *
 *       Filename:  mmtree.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/17/2010 10:54:33 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <new>
#include "mmtree.h"

using namespace std;

#define MMT_COLOR_BLACK	0
#define MMT_COLOR_RED	1

#define MMT(px) ((c_mmtree*)px)

/** 
 * @brief 初始扩充容量大小
 */
#define INITIAL_INCREMENT_SIZE 100

/**
 * @brief 扩充容量的最大值
 */

#define MAX_INCREMENT_SIZE     102300 

/** 
 * @brief 创建接口实例
 * 
 * @param pp_instance
 * 
 * @return 0success -1failed 
 */
int create_mmtree_instance(i_mmtree **pp_instance)
{
	if(NULL == pp_instance)
	{
		return -1;
	}

	c_mmtree *p_instance = new (std::nothrow)c_mmtree();
	if(NULL == p_instance)
	{
		return -1;
	}
	else
	{
		*pp_instance = dynamic_cast<i_mmtree*>(p_instance);
		return 0;
	}
}

/** 
 * @brief 构造函数
 */
c_mmtree::c_mmtree()
{
	m_inited = 0;
	m_size = 0;
	m_start = NULL;
	m_state = NULL;
	m_map = NULL;
	m_fd = -1;
	m_increment_num = INITIAL_INCREMENT_SIZE;
	m_p_mtree = NULL;
}

/** 
 * @brief 析构函数
 */
c_mmtree::~c_mmtree()
{
	uninit();
}

/** 
 * @brief 初始化函数
 * 
 * @return 0success -1failed 
 */
int c_mmtree::init(const char *file, int flags, uint32_t check_create_flag)
{
	if(m_inited)
	{
		set_err(MMT_IFACE_EINIT, "instance has been inited.");
		return -1;
	}

	if(access(file, F_OK) != 0 && (flags & O_CREAT) == 0)
	{//文件不存在，且未指定创建标志
		set_err(MMT_IFACE_EINIT,"init file %s not exist, but you can use O_CREAT to create new file.", file); 
		return -1;
	}	
	else if(access(file, F_OK) != 0)
	{//文件不存在，但指定了创建标志
		m_fd = open(file, flags, 0644);
	}
	else
	{//文件已经存在
		if(check_create_flag)
		{
			set_err(MMT_IFACE_EINIT, "I'm sorry, version%s not implemented check schema.", MMTREE_VERSION);
			return -1;
		}
		else
		{
			m_fd = open(file, flags, 0644);
		}
	}

	if(m_fd < 0)
	{
		set_err(MMT_IFACE_EINIT, "init file %s open failed(%s).", file, strerror(errno));
		return -1;
	}

	struct stat st = {0};

	if((m_p_mtree = (c_mmtree*)calloc(1, sizeof(c_mmtree))))
	{
		if(fstat(m_fd, &st) == 0)
		{
			m_size = st.st_size;	
			if(m_size == 0)
			{
				m_size += (uint64_t)sizeof(mt_state_t);
				if(mmtree_increment() != 0)
				{
					return -1;
				}
				memset(m_state->md5, 0, sizeof(m_state->md5));
				sprintf(m_state->version, "%s", MMTREE_VERSION);
				m_state->root_id = 0;
				m_state->current = 0;
				m_state->nonzero = 0;
			}
			else
			{
				if(mmtree_mmap() != 0)
				{
					return -1;
				}
			}
		}
		else
		{
			close(m_fd);
			set_err(MMT_IFACE_EINIT, "fstat file %s fail(%s).", file, strerror(errno));
			free(m_p_mtree);
			m_p_mtree = NULL;
			return -1;
		}
	}
	else
	{
		set_err(MMT_IFACE_EINIT, "m_p_mtree calloc failed(%s).", strerror(errno));
		return -1;
	}

	m_inited = 1;
	return 0;
}

int c_mmtree::insert(int key, int data)
{
	if(!m_inited)
	{
		set_err(MMT_IFACE_EINIT, "instance has not been init.");
		return -1;
	}

	if(m_state && m_map)
	{
		if(m_state->root_id == 0)
		{//第一次插入节点
			insert_root_node(key, data);
		}
		else
		{//插入新的节点
			int node_id = 0;
			mt_node_t *p_node = NULL;	
			if(m_state->root_id < m_state->total)
			{
				node_id = m_state->root_id;
				while(node_id > 0 && node_id < m_state->total)
				{
					p_node = &m_map[node_id];
					if(key == p_node->key)
					{
						set_err(MMT_IFACE_EINSERT, "key(%d) has been exist.", key);
						return -1;
					}
					else if(key > p_node->key)
					{
						if(p_node->right == 0)
						{
							break;
						}
						node_id = p_node->right;
					}
					else
					{
						if(p_node->left == 0)
						{
							break;
						}
						node_id = p_node->left;
					}
				}
					
				if(insert_new_node(node_id, key, data) != 0)
				{
					return -1;
				}
			}
			else
			{
				set_err(MMT_IFACE_EINSERT, "root_id(%d) > total(%d).", m_state->root_id, m_state->total);
				return -1;
			}
		}
	}	
	else
	{
		set_err(MMT_IFACE_EINSERT, "m_state or m_map is null.");
		return -1;
	}

	return 0;
}

int c_mmtree::update(int key, int data, int op)
{
	if(!m_inited)
	{
		set_err(MMT_IFACE_EINIT, "instance has not been init.");
		return -1;
	}
	
	if(m_state && m_map)
	{
		if(m_state->root_id == 0 || m_state->root_id >= m_state->total)
		{
			set_err(MMT_IFACE_EINSERT, "not found key(%d) in tree, for tree is null(root_id = %d) or root_id(%d) > total(%d)", key, m_state->root_id, m_state->root_id, m_state->total);
			return -1;
		}

		int node_id = m_state->root_id;
		mt_node_t *p_node = NULL;
		while(node_id > 0 && node_id < m_state->total)
		{
			p_node = &m_map[node_id];
			if(key == p_node->key)
			{
				return update_node_value(p_node, data, op);
			}
			else if(key > p_node->key)
			{
				if(p_node->right == 0)
				{
					break;
				}
				node_id = p_node->right;
			}
			else
			{
				if(p_node->left == 0)
				{
					break;
				}
				node_id = p_node->left;
			}
		}
	}
	else
	{
		set_err(MMT_IFACE_EINSERT, "m_state or m_map is null.");
		return -1;
	}
	
	set_err(MMT_IFACE_EINSERT, "not found key(%d) in tree.", key);
	return -1;
}

int c_mmtree::set(int key, int data, int op)
{
	if(!m_inited)
	{
		set_err(MMT_IFACE_EINIT, "instance has not been init.");
		return -1;
	}

	if(m_state && m_map)
	{
		if(m_state->root_id == 0)
		{//第一次插入节点
			insert_root_node(key, data);
		}
		else
		{
			int node_id = m_state->root_id;
			mt_node_t *p_node = NULL;
			while(node_id > 0 && node_id < m_state->total)
			{
				p_node = &m_map[node_id];
				if(key == p_node->key)
				{
					return update_node_value(p_node, data, op);
				}
				else if(key > p_node->key)
				{
					if(p_node->right == 0)
					{
						break;
					}
					node_id = p_node->right;
				}
				else
				{
					if(p_node->left == 0)
					{
						break;
					}
					node_id = p_node->left;
				}
			}

			return insert_new_node(node_id, key, data);	
		}
	}
	else
	{
		set_err(MMT_IFACE_EINSERT, "m_state or m_map is null.");
		return -1;
	}

	return 0;
}

int c_mmtree::get(int key, int *p_data)
{
	if(!m_inited)
	{
		set_err(MMT_IFACE_EINIT, "instance has not been init.");
		return -1;
	}
	if(!p_data)
	{
		set_err(MMT_IFACE_EFIND, "para error, p_data is null.");
		return -1;
	}

	if(m_state && m_map)
	{
			int node_id = m_state->root_id;
			mt_node_t *p_node = NULL;
			while(node_id > 0 && node_id < m_state->total)
			{
				p_node = &m_map[node_id];
				if(key == p_node->key)
				{
					*p_data = p_node->data;
					return 0;
				}
				else if(key > p_node->key)
				{
					if(p_node->right == 0)
					{
						break;
					}
					node_id = p_node->right;
				}
				else
				{
					if(p_node->left == 0)
					{
						break;
					}
					node_id = p_node->left;
				}
			}

	}
	else
	{
		set_err(MMT_IFACE_EINSERT, "m_state or m_map is null.");
		return -1;
	}

	set_err(MMT_IFACE_EFIND, "not found key(%d) in tree.", key);
	return -1;
}

/** 
 * @brief 查找指定的key是否存在
 * 
 * @param key
 * 
 * @return 0存在 -1其他错误 1不存在
 */
int c_mmtree::find(int key)
{
	if(!m_inited)
	{
		set_err(MMT_IFACE_EINIT, "instance has not been init.");
		return -1;
	}
	
	int node_id = 0;
	return find(key,&node_id);
}

int c_mmtree::remove(int key)
{
	if(!m_inited)
	{
		set_err(MMT_IFACE_EINIT, "instance has not been init.");
		return -1;
	}
	int node_id = 0;
	if(find(key, &node_id) != 0)
	{
		return -1;
	}
	int root_id = m_state->root_id;
	if(root_id > 0)
	{
		if(remove_node(&root_id, node_id) != 0)
		{
			return -1;
		}
		m_state->root_id = root_id;
	}	
	else
	{
		set_err(MMT_IFACE_EINSERT, "no node in tree.");
		return -1;
	}	
	return 0;
}

int c_mmtree::get_nonzero_num(int *p_nonzero_num)
{
	if(p_nonzero_num == NULL)
	{
		return -1;
	}
	*p_nonzero_num = m_state->nonzero;
	return 0;
}

int c_mmtree::get_node_num(int *p_node_num)
{
	if(p_node_num == NULL)
	{
		return -1;
	}
	*p_node_num =  m_state->current - m_state->qleft;
	return 0;
}

int c_mmtree::traversal(mmtree_callback_t cb, void *p_param)
{
	if(!m_inited)
	{
		set_err(MMT_IFACE_EINIT, "instance has not been init.");
		return -1;
	}

	if(m_state && m_map)
	{
		int id = m_state->root_id;
		if(id <= 0)
		{
			return 0;
		}

		while(m_map[id].left > 0)
		{
			id = m_map[id].left;
		}
			
		cb(m_map[id].key, m_map[id].data, p_param);
		int next_id = next(id);
		while(next_id > 0)
		{
			cb(m_map[next_id].key, m_map[next_id].data, p_param);
			next_id = next(next_id);
		}
		return 0;
	}
	else
	{
		set_err(MMT_IFACE_EINSERT, "m_state or m_map is null.");
		return -1;
	}
	return 0;
}

int c_mmtree::combine_tree(const char *file, int flag)
{
	if(!m_inited)
	{
		set_err(MMT_IFACE_EINIT, "instance has not been init.");
		return -1;
	}
	
	int op = 0;
	
	if(flag & MMT_DATA_ADD)
	{
		op = MMT_OP_ADD; 
	}
	else if(flag & MMT_DATA_SUB)
	{
		op = MMT_OP_SUB;
	}
	else if(flag & MMT_DATA_MUL)
	{
		op = MMT_OP_MUL;
	}
	else if(flag & MMT_DATA_DIV)
	{
		op = MMT_OP_DIV;
	}
	else
	{
		set_err(MMT_IFACE_EOP, "flags(%d) is invalid.", flag);
	}

	int (*func)(int, int, void*);
	if(flag & MMT_KEY_ADD)
	{
		func = set_cb;
	}	
	else
	{
		func = update_cb;
	}
	

	i_mmtree *p_mtree = NULL;
	if(create_mmtree_instance(&p_mtree) != 0)
	{
		set_err(MMT_IFACE_EOP, "create mmtree instance failed.");
		return -1;
	}
	
	if(p_mtree->init(file, O_RDWR, 0) != 0)
	{
		set_err(MMT_IFACE_EOP, "file %s init failed.", file);
		p_mtree->release();
		return -1;
	}

	param_t param;
	param.op = op;
	param.p_obj = this;
	
	if(p_mtree->traversal(func, &param) != 0)
	{
		p_mtree->uninit();
		p_mtree->release();
		return -1;
	}

	p_mtree->uninit();
	p_mtree->release();

	return 0;
}

int c_mmtree::serialize(const char *file, const int flags)
{
	int fd = open(file, flags, 0644);
	if(fd < 0)
	{
		set_err(MMT_IFACE_EOP, "file %s open failed(%s).", strerror(errno));
		return -1;
	}

	param_t param;
	param.op = fd;
	param.p_obj = this;

	traversal(write_to_file, &param);
 	close(fd);	
	return 0;
}


int c_mmtree::flush()
{
	msync(m_start, m_size, MS_SYNC);
	return 0;
}

int c_mmtree::get_last_errno()
{
	return m_errno;
}

const char* c_mmtree::get_last_errstr()
{
	return m_strerr;
}

int c_mmtree::uninit()
{
	if(!m_inited)
	{
		return -1;
	}
	if(m_p_mtree)
	{
		mmtree_munmap();
		if(m_fd > 0)
		{
			close(m_fd);
			m_fd = 0;
		}
		free(m_p_mtree);
		m_p_mtree = NULL;
	}

	m_inited = 0;

	return 0;
}

int c_mmtree::release()
{
	delete this;
	return 0;
}

int c_mmtree::mmtree_mmap()
{
	if(m_p_mtree)
	{
		mmtree_munmap();
		if((m_start = mmap(NULL, m_size, PROT_READ|PROT_WRITE, MAP_SHARED, m_fd, 0)) != (void*)-1)
		{
			m_state = (mt_state_t*)m_start;
			m_map = (mt_node_t*)((char*)m_start + sizeof(mt_state_t));
		}
		else
		{
			set_err(MMT_IFACE_EMMAP, "mmap failed(%s).", strerror(errno));
			return -1;
		}
	}
	else
	{
		set_err(MMT_IFACE_EINIT, "m_p_mtree is null.");
		return -1;
	}

	return 0;
}

/** 
 * @brief 扩充节点数
 * 
 * @return 0success -1failed
 */
int c_mmtree::mmtree_increment()
{
	if(m_p_mtree)
	{
		if(m_start && m_size > 0 )
		{
			msync(m_start, m_size, MS_SYNC);
			munmap(m_start, m_size);
			m_start = NULL;
			m_state = NULL;
			m_map = NULL;
		}

		m_size += (uint64_t)m_increment_num * (uint64_t)sizeof(mt_node_t);
		ftruncate(m_fd, m_size);
		if((m_start = mmap(NULL, m_size, PROT_READ|PROT_WRITE, MAP_SHARED, m_fd, 0)) != MAP_FAILED)
		{
			m_state = (mt_state_t*)m_start;
			m_map = (mt_node_t*)((char*)m_start + sizeof(mt_state_t));
			m_state->left += m_increment_num;
			if(m_state->total == 0)
			{
				m_state->left--;
			}	
			m_state->total += m_increment_num;
		}
		else
		{
			set_err(MMT_IFACE_EMMAP, "mmap failed(%s).", strerror(errno));
			return -1;
		}
	}
	else
	{
		set_err(MMT_IFACE_EINIT, "m_p_mtree is null.");
		return -1;
	}

	m_increment_num *= 2; //每次已两倍的速度扩充节点数
	if(m_increment_num > MAX_INCREMENT_SIZE)//当扩充树超过最大限制时，从初始大小开始扩充节点
	{
		m_increment_num = INITIAL_INCREMENT_SIZE;
	}
	return 0;
}

int c_mmtree::mmtree_munmap()
{
	if(m_p_mtree)
	{
		if(m_size > 0)
		{
			if(m_start)
			{
				msync(m_start, m_size, MS_SYNC);
				munmap(m_start, m_size);
				m_start = NULL;
				m_state = NULL;
				m_map = NULL;
			}
		}
	}
	else
	{
		set_err(MMT_IFACE_EINIT, "m_p_mtree is null.");
		return -1;
	}

	return 0;
}


int c_mmtree::next(int node_id)
{
	int id = -1, parent_id = 0;
	if(node_id > 0 && node_id < m_state->total)
	{
		id = node_id;
		if(m_map[id].right > 0)
		{
			id = m_map[id].right;
			while(m_map[id].left > 0)
			{
				id = m_map[id].left;
			}
		}
		else
		{
			while(id > 0)
			{
				parent_id = m_map[id].parent;
				if(m_map[id].key < m_map[parent_id].key)
				{
					id = parent_id;
					return id;
				}
				else
				{
					id = parent_id;
				}
			}
		}
	}
	return id;
}

int c_mmtree::remove_node(int *p_rootid, int node_id)
{
	if(node_id < m_state->total)
	{
		int id = 0, pid = 0, parent = 0, child = 0, rid = 0, uid = 0, ppid = 0, lid = 0, z = 0, color = 0;
		if(m_map[node_id].left == 0)
		{
			child = m_map[node_id].right;
		}
		else if(m_map[node_id].right == 0)
		{
			child = m_map[node_id].left;
		}
		else
		{
			id = m_map[node_id].right;
			while(m_map[id].left > 0)
			{
				id = m_map[id].left; 
			}
			parent = m_map[id].parent;
			color = m_map[id].color;
			if((child = m_map[id].right) > 0)
			{
				m_map[child].parent = parent;
			}

			if((pid = m_map[id].parent) > 0)
			{
				if(m_map[pid].left == id)
				{
					m_map[pid].left = child;
				}
				else
				{
					m_map[pid].right = child;
				}
			}
			else
			{
				*p_rootid = child;
			}
			
			if(m_map[id].parent == node_id)
			{
				parent = id;
			}

			m_map[id].color = m_map[node_id].color;
			m_map[id].parent = m_map[node_id].parent;
			m_map[id].left = m_map[node_id].left;
			m_map[id].right = m_map[node_id].right;
			if((pid = m_map[node_id].parent) > 0)
			{
				if(m_map[pid].left == node_id)
				{
					m_map[pid].left = id;
				}
				else
				{
					m_map[pid].right = id;
				}
			}
			else
			{
				*p_rootid = id;
			}

			lid = m_map[node_id].left;
			m_map[lid].parent = id;
			if((rid = m_map[node_id].right) > 0)
			{
				m_map[rid].parent = id;
			}
			goto color_move;
		}
		
		parent = m_map[node_id].parent;
		color = m_map[node_id].color;
		if((id = child) > 0)
		{
			m_map[child].parent = parent;
		}

		if((pid = m_map[node_id].parent) > 0)
		{
			if(node_id == m_map[pid].left)
			{
				m_map[pid].left = child;
			}
			else
			{
				m_map[pid].right = child;
			}
		}
		else
		{
			*p_rootid = child;
		}

color_move:
		if(color == MMT_COLOR_BLACK)
            	{
                	mmt_remove_color(p_rootid, child, parent, rid, rid, uid, ppid);
            	}
            	//add to qleft
            	memset(&(m_map[node_id]), 0, sizeof(mt_node_t));
            	if(m_state->qleft == 0)
            	{
                	m_state->qfirst = m_state->qlast = node_id;
            	}
            	else
            	{
                	z = m_state->qlast;
                	m_map[z].parent = node_id;
                	m_state->qlast = node_id;
            	}
            	m_state->qleft++;
            	m_state->left++;
	}	
	else
	{
		set_err(MMT_IFACE_EINSERT, "node_id(%d) > total(%d).", node_id, m_state->total);
		return -1;
	}

	return 0;
}


void c_mmtree::mmt_remove_color(int *p_rootid, int id, int pid, int lid, int rid, int uid, int ppid)
{
	while((id == 0 || m_map[id].color == MMT_COLOR_BLACK) && id != *p_rootid)                                                  
	{                                                                           
		if(m_map[pid].left == id)                                         
		{                                                                       
			uid = m_map[pid].right;                                       
			if(m_map[uid].color == MMT_COLOR_RED)                         
			{                                                                   
				m_map[uid].color = MMT_COLOR_BLACK;                       
				m_map[pid].color = MMT_COLOR_RED;             
				mmtree_rotate_left(p_rootid, pid, rid, lid, ppid);   
				uid = m_map[pid].right;                       
			}                                                                   
			if((m_map[uid].left == 0 || m_map[uid].color == MMT_COLOR_BLACK) &&
				 (m_map[uid].right == 0 || m_map[uid].color == MMT_COLOR_BLACK))         
			{                                                                   
				m_map[uid].color = MMT_COLOR_RED;                         
				id = pid;                                                       
				pid = m_map[id].parent;                                   
			}                                                                   
			else                                                               
			{                                                                  
				rid = m_map[uid].right;                     
				lid = m_map[uid].left;                     
				if(rid == 0 || m_map[rid].color == MMT_COLOR_BLACK)   
				{                                            
					m_map[lid].color = MMT_COLOR_BLACK;                  
					m_map[uid].color = MMT_COLOR_RED;                    
					mmtree_rotate_right(p_rootid, uid, lid, rid, ppid); 
					uid = m_map[pid].right;                             
				}                                                              
				m_map[uid].color = m_map[pid].color;               
				m_map[pid].color = MMT_COLOR_BLACK;                      
				if((rid = m_map[uid].right) > 0)                         
				{
					m_map[rid].color = MMT_COLOR_BLACK;   
				}                    
				mmtree_rotate_left(p_rootid, pid, rid, lid, ppid);               
				id = *p_rootid;                                                  
				break;                                                         
			}                                                                   
		}                                                                      
		else                                                                  
		{                                                                    
			uid = m_map[pid].left;                                        
			if(m_map[uid].color == MMT_COLOR_RED)                         
			{                                                                   
				m_map[uid].color = MMT_COLOR_BLACK;                       
				m_map[pid].color = MMT_COLOR_RED;                         
				mmtree_rotate_right(p_rootid, pid, lid, rid, ppid);              
				uid = m_map[pid].left;                                    
			}                                                                   
			if((m_map[uid].left == 0 || m_map[uid].color == MMT_COLOR_BLACK) &&
				 (m_map[uid].right == 0 || m_map[uid].color == MMT_COLOR_BLACK))          
			{                                                                   
				m_map[uid].color = MMT_COLOR_RED;                         
				id = pid;                                                       
				pid = m_map[id].parent;                                   
			}                                                                   
			else                                                                
			{                                                                   
				rid = m_map[uid].right;                                  
				lid = m_map[uid].left;                                    
				if(lid == 0 || m_map[lid].color == MMT_COLOR_BLACK)       
				{                                                               
					m_map[rid].color = MMT_COLOR_BLACK;                   
					m_map[uid].color = MMT_COLOR_RED;                     
					mmtree_rotate_left(p_rootid, uid, rid, lid, ppid);    
					uid = m_map[pid].left;                                
				}                                                             
				m_map[uid].color = m_map[pid].color;              
				m_map[pid].color = MMT_COLOR_BLACK;             
				if((lid = m_map[uid].left) > 0)                 
				{
					m_map[lid].color = MMT_COLOR_BLACK;     
				}				
				mmtree_rotate_right(p_rootid, pid, lid, rid, ppid);              
				id = *p_rootid;                                                  
				break;                                                          
			}                                   
		}                                           
	}                                                   
	if(id > 0)
	{
 		m_map[id].color = MMT_COLOR_BLACK; 
	}
}

int c_mmtree::find(int key, int *p_nodeid)
{
	if(m_state && m_map)
	{
		int node_id = m_state->root_id;
		mt_node_t *p_node = NULL;
		while(node_id > 0 && node_id < m_state->total)
		{
			p_node = &m_map[node_id];
			if(key == p_node->key)
			{
				*p_nodeid = node_id;
				return 0;
			}
			else if(key > p_node->key)
			{
				if(p_node->right == 0)
				{
					break;
				}
				node_id = p_node->right;
			}
			else
			{
				if(p_node->left == 0)
				{
					break;
				}
				node_id = p_node->left;
			}
		}

	}
	else
	{
		set_err(MMT_IFACE_EINSERT, "m_state or m_map is null.");
		return -1;
	}

	set_err(MMT_IFACE_EFIND, "not found key(%d) in tree.", key);
	return 1;

}


void c_mmtree::mmtree_insert_color(int *p_rootid, int id, int lid, int rid, int uid, int pid, int gpid, int ppid)
{
	while((pid = m_map[id].parent)> 0 && m_map[pid].color == MMT_COLOR_RED)                         
    	{                                                                           
        	gpid = m_map[pid].parent;                                         
        	if(pid == m_map[gpid].left)                                       
        	{                                                                       
            		uid = m_map[gpid].right;                                      
            		if(uid > 0 && m_map[uid].color == MMT_COLOR_RED)              
            		{                                                                   
                		m_map[uid].color = MMT_COLOR_BLACK;                       
                		m_map[pid].color = MMT_COLOR_BLACK;                       
                		m_map[gpid].color = MMT_COLOR_RED;                        
                		id = gpid;                                                      
                		continue;                                                       
            		}                                                                   
            		if(m_map[pid].right == id)                                    
            		{                                                                   
                		mmtree_rotate_left(p_rootid, pid, rid, lid, ppid);               
                		uid = pid;
				pid = id;
			        id = uid;                                  
           		 }                                                                   
            		m_map[pid].color = MMT_COLOR_BLACK;                           
            		m_map[gpid].color = MMT_COLOR_RED;                            
            		mmtree_rotate_right(p_rootid, gpid, lid, rid, ppid);                 
        	}                                                                       
        	else                                                                    
        	{                                                                       
           		 uid = m_map[gpid].left;                                       
            		if(uid > 0 && m_map[uid].color == MMT_COLOR_RED)              
            		{                                                                   
                		m_map[uid].color = MMT_COLOR_BLACK;                       
                		m_map[pid].color = MMT_COLOR_BLACK;                       
                		m_map[gpid].color = MMT_COLOR_RED;                        
                		id = gpid;                                                      
                		continue;                                                       
            		}                                                                   
            		if(m_map[pid].left == id)                                     
            		{                                                                   
                		mmtree_rotate_right(p_rootid, pid, lid, rid, ppid);              
                		uid = pid; pid = id; id = uid;                                  
            		}                                                                   
            		m_map[pid].color = MMT_COLOR_BLACK;                           
           		m_map[gpid].color = MMT_COLOR_RED;                            
            		mmtree_rotate_left(p_rootid, gpid, rid, lid, ppid);                  
        	}                                                                       
    	}                                                                           
    	if(*p_rootid > 0)
	{
		m_map[*p_rootid].color = MMT_COLOR_BLACK;  
	}

}

void c_mmtree::mmtree_rotate_left(int *p_rootid, int id, int lid, int rid, int ppid)
{
	if(m_p_mtree && (rid = m_map[id].right) > 0)
	{
		if((lid  = m_map[id].right = m_map[rid].left) > 0)
		{
			m_map[lid].parent = id;
		}

		if((ppid = m_map[rid].parent = m_map[id].parent) > 0)
		{
			if(m_map[ppid].left == id)
			{
				m_map[ppid].left = rid;
			}
			else
			{
				m_map[ppid].right = rid;
			}
		}
		else
		{
			*p_rootid = rid;
		}

		m_map[rid].left = id;
		m_map[id].parent = rid;
	}

}

void c_mmtree::mmtree_rotate_right(int *p_rootid, int id, int lid, int rid, int ppid)
{
	if(m_p_mtree && (lid = m_map[id].left) > 0)
	{
		if((rid = m_map[id].left = m_map[lid].right) > 0)
		{
			m_map[rid].parent = id;
		}

		if((ppid = m_map[lid].parent = m_map[id].parent) > 0)
		{
			if(m_map[ppid].left == id)
			{
				m_map[ppid].left = lid;
			}
			else
			{
				m_map[ppid].right = lid;
			}
		}
		else
		{
			*p_rootid = lid;
		}

		m_map[lid].right = id;
		m_map[id].parent = lid;
	}

}

/** 
 * @brief 插入树的根节点
 * 
 * @param key 键
 * @param data值
 * 
 * @return 0success -1failed
 */
int c_mmtree::insert_root_node(int key, int data)
{
	m_state->root_id = ++(m_state->current);	
	memset(&(m_map[m_state->root_id]), 0, sizeof(mt_node_t));
	m_map[m_state->root_id].key = key;
	m_map[m_state->root_id].data = data;
	m_map[m_state->root_id].color = MMT_COLOR_BLACK;
	m_state->left--;
	if(data != 0)
	{
		m_state->nonzero++;
	}
	return 0;
}

/** 
 * @brief 在树中插入一个新的节点
 * 
 * @param node_id 要插入的位置节点
 * @param key 键
 * @param data值
 * 
 * @return 0success -1failed
 */
int c_mmtree::insert_new_node(int node_id, int key, int data)
{
	int id = 0, rid = 0, lid = 0, pid = 0, uid = 0, g_pid = 0, p_pid = 0;
	if(m_state->left == 0)
	{
		if(mmtree_increment() != 0)
		{
			return -1;
		}
	}

	if(m_state->qleft > 0)
	{
		id = m_state->qfirst;
		m_state->qfirst = m_map[id].parent;
		m_state->qleft--;
	}
	else
	{
		id = ++(m_state->current);
	}

	m_state->left--;
	m_map[id].parent = node_id;
	m_map[id].key = key;
	m_map[id].data = data;

	if(key > m_map[node_id].key)
	{
		m_map[node_id].right = id;
	}
	else
	{
		m_map[node_id].left = id;
	}

	node_id = id;
	m_map[node_id].color = MMT_COLOR_RED;
	int root_id = m_state->root_id;
	mmtree_insert_color(&root_id, node_id, lid, rid, uid, pid, g_pid, p_pid);
	m_state->root_id = root_id;
	if(data != 0)
	{
		m_state->nonzero++;
	}
	return 0;
}

/** 
 * @brief 更新树中节点的值
 * 
 * @param p_node要更新的节点
 * @param data	值
 * @param op 操作
 * 
 * @return 0success -1failed
 */
int c_mmtree::update_node_value(mt_node_t *p_node, int data, int op)
{
	int prev_zero_flag = 0;
	if(p_node->data == 0)
	{
		prev_zero_flag = 1;
	}

	switch(op)
	{
		case MMT_OP_ADD:
			p_node->data += data;
			break;
		case MMT_OP_SUB:
			p_node->data -= data;
			break;
		case MMT_OP_MUL:
			p_node->data *= data;
			break;
		case MMT_OP_DIV:
			if(data == 0)
			{
				set_err(MMT_IFACE_EOP, "divisor can not be zero.");
				return -1;
			}
			p_node->data /= data;
			break;
		case MMT_OP_MAX:
			if(data > p_node->data)
			{
				p_node->data = data;
			}
			break;
		case MMT_OP_MIN:
			if(data < p_node->data)
			{
				p_node->data= data;
			}
			break;
		case MMT_OP_SET:
			p_node->data = data;
			break;
		default:
			set_err(MMT_IFACE_EOP, "unknown op %d.", op);
			return -1;
	}

	if(prev_zero_flag && p_node->data != 0)
	{
		m_state->nonzero++;
	}
	else if(!prev_zero_flag && p_node->data == 0)
	{
		m_state->nonzero--;
	}
	else
	{
		//nothing to do 
	}
	return 0;

}

int c_mmtree::write_to_file(int key, int data, void *p_param)
{
	param_t *p_para = (param_t*)p_param;
	int fd = p_para->op;
	char buffer[1024] = {0};
	sprintf(buffer, "%d:%d\n", key, data);
	lseek(fd, 0, SEEK_END);
	write(fd, buffer, strlen(buffer));
	return 0;
}


int c_mmtree::set_cb(int key, int data, void *p_param)
{
	param_t *p_para = (param_t*)p_param;
	int op = p_para->op;
	c_mmtree *p_mtree = (c_mmtree*)p_para->p_obj;
	return p_mtree->set(key, data, op);
}

int c_mmtree::update_cb(int key, int data, void *p_param)
{
	param_t *p_para = (param_t*)p_param;
	int op = p_para->op;
	c_mmtree *p_mtree = (c_mmtree*)p_para->p_obj;
	return p_mtree->update(key, data, op);
}

int c_mmtree::get_node_cb(int key, int data, void *p_param)
{
	param_output_t * p = (param_output_t *)p_param;
	if (0 == data)
	{
		return 0;
	}
	int index = p->index;
	p->p_buf[index].key = key;
	p->p_buf[index].data = data;
	p->index += 1;	

	return 0;
}


int c_mmtree::compare_key_data(mt_key_data_t * d1, mt_key_data_t * d2)
{
    if (d1->data > d2->data)
    {
        return 1;
    }
    else if (d1->data == d2->data)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

int c_mmtree::copy_key_data(mt_key_data_t * from, mt_key_data_t * to)
{
    to->key = from->key;
    to->data = from->data;
    return 0;
}

int c_mmtree::swap_key_data(mt_key_data_t *a, mt_key_data_t *b)
{
    mt_key_data_t tmp;
    copy_key_data(a, &tmp);
    copy_key_data(b, a);
    copy_key_data(&tmp, b);
    return 0;
}
 
int c_mmtree::quicksort(mt_key_data_t * p, uint32_t begin, uint32_t end, uint32_t num)
{
	if (end >= begin + 1) 
  	{
  		mt_key_data_t * piv = p + begin;
        uint32_t k = begin + 1;
        uint32_t r = end;
 
		while (k < r) 
        {
            if (1 == compare_key_data(p + k, piv))
                k++;
            else 
                swap_key_data(p + k, p + r--);
        }

		if (1 == compare_key_data(p + k, piv))
        {
 
			swap_key_data(p + k, p + begin);
 
			quicksort(p, begin, k, num);
            if (k < num - 1)
            {
                quicksort(p, r, end, num);
            }
		}
        else
        {
			if (end - begin == 1)
            {
                return 0;
            }
 
			swap_key_data(p + --k, p + begin);
			quicksort(p, begin, k, num);
            if (k < num - 1)
            {
                quicksort(p, r, end, num);
            }
		}
  	}
    return 0;
 
}


int c_mmtree::output_top_data(mt_key_data_t * p_buf, uint32_t * num)
{
    if (NULL == p_buf)
    {
        return -1;
    }

    uint32_t node_count = 0;
    if (0 != get_nonzero_num((int *)&node_count))
    {
        return -1;
    }
    if (node_count < *num)
    {
        *num = node_count;
    }
    if (0 == node_count)
    {
        return 0;
    }

    mt_key_data_t * p_key_data = NULL;
    p_key_data = (mt_key_data_t *)calloc(node_count, sizeof(mt_key_data_t));
    if (NULL == p_key_data)
    {
        return -1;
    }
    param_output_t param_output;
    param_output.index = 0;
    param_output.p_buf = p_key_data;
    if (0 != traversal(get_node_cb, &param_output))
    {
        return -1;
    }
    if (0 != quicksort(p_key_data, 0, node_count - 1, *num))
    {
        return -1;
    }
    memcpy(p_buf, p_key_data, sizeof(mt_key_data_t) * (*num));
    return 0;
}

/** 
 * @brief 设置本对象的内部错误信息
 * 
 * @param errno 错误码
 * @param msg   错误描述
 * @param ... 	变参
 */
void c_mmtree::set_err(int err_no, const char *msg, ...)
{
	m_errno = err_no;

	va_list arg;
	va_start(arg, msg);
	vsprintf(m_strerr, msg, arg);
	va_end(arg);
}
