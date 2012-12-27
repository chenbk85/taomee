/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file ucount.cpp
 * @author richard <richard@taomee.com>
 * @date 2010-05-10
 */

#include <cstdlib>                                         // for abort()
#include <new>                                             // for std::nothrow
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

extern "C"
{
#include <libtaomee/log.h>
}

#include "ucount.h"

using namespace std;

int create_ucount_instance(i_ucount **pp_instance)
{
	if (pp_instance == NULL) {
		return -1;
	}

	c_ucount *p_instance = new(nothrow)c_ucount();
	if (p_instance == NULL) {
		return -1;
	} else {
		*pp_instance = dynamic_cast<i_ucount *>(p_instance);
		return 0;
	}
}

c_ucount::c_ucount() : m_inited(0), m_fd(-1)
{

}

c_ucount::~c_ucount()
{
	if (m_inited) {
		uninit();
	}
}

int c_ucount::init(const char *p_ucount_path_name, uint32_t flags, mode_t mode)
{
	if (m_inited) {
		return -1;
	}

	int create = flags & CREATE;
	int excl = flags & EXCL;

	if (access(p_ucount_path_name, R_OK | W_OK) != 0) {    // 文件不存在
		if (!create) {
			return -1;
		}

		if ((m_fd = open(p_ucount_path_name, O_RDWR | O_CREAT | O_EXCL, mode)) == -1) {
			return -1;
		}
		if (ftruncate(m_fd, UFILE_SIZE) != 0) {
			return -1;
		}
	} else {                                               // 文件存在
		if (create && excl) {
			return -1;
		}
		if ((m_fd = open(p_ucount_path_name, O_RDWR)) == -1) {
			return -1;
		}
		struct stat stat = {0};
		if (fstat(m_fd, &stat) != 0) {
			return -1;
		}
		if (stat.st_size != UFILE_SIZE) {
			return -1;
		}
	}

	if (m_fd == -1) {
		return -1;
	}

	m_inited = 1;

	return 0;
}

int c_ucount::set(uint32_t ucount, value_opcode_t value_opcode)
{
	if (!m_inited) {
		return -1;
	}

	if (ucount >= UFILE_SIZE * 8) {
		ERROR_LOG("ERROR: ucount: %u >= %u", ucount, UFILE_SIZE * 8);
		return -1;
	}

	uint32_t block_index = ucount / (BLOCK_SIZE * 8);
	uint32_t offset = ucount % (BLOCK_SIZE * 8);

	void *p_block_addr = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
			m_fd, BLOCK_SIZE * block_index);
	if (p_block_addr == MAP_FAILED) {
		return -1;
	}

	set_block(p_block_addr, offset, value_opcode);

	if (munmap(p_block_addr, BLOCK_SIZE) != 0) {
		return -1;
	}

	return 0;
}

int c_ucount::get(uint32_t ucount)
{
	if (!m_inited) {
		return -1;
	}

	uint32_t block_index = ucount / (BLOCK_SIZE * 8);
	uint32_t offset = ucount % (BLOCK_SIZE * 8);

	void *p_block_addr = mmap(NULL, BLOCK_SIZE, PROT_READ, MAP_SHARED,
			m_fd, BLOCK_SIZE * block_index);
	if (p_block_addr == MAP_FAILED) {
		return -1;
	}

	int rv = get_block(p_block_addr, offset);

	if (munmap(p_block_addr, BLOCK_SIZE) != 0) {
		return -1;
	}

	return rv;
}


/**
 * @brief set_on_noucount
 *
 * @param ucount
 *
 * @return 1:已经存在 0:不存在 -1：出错
 */
int c_ucount::set_on_noucount(uint32_t ucount, value_opcode_t value_opcode)
{
	if (!m_inited) {
		return -1;
	}

	if (ucount >= UFILE_SIZE * 8) {
		ERROR_LOG("ERROR: ucount: %u >= %u", ucount, UFILE_SIZE * 8);
		return -1;
	}

	uint32_t block_index = ucount / (BLOCK_SIZE * 8);
	uint32_t offset = ucount % (BLOCK_SIZE * 8);

	void *p_block_addr = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
			m_fd, BLOCK_SIZE * block_index);
	if (p_block_addr == MAP_FAILED) {
		return -1;
	}

    if(get_block(p_block_addr, offset))
    {//已经存在
	    if (munmap(p_block_addr, BLOCK_SIZE) != 0)
        {
            ERROR_LOG("1:unmap  failed(%s).",  strerror(errno));
		   // return -1;
	    }
        return 1;
    }
    else
    {//还不存在
	    set_block(p_block_addr, offset, value_opcode);
    	if (munmap(p_block_addr, BLOCK_SIZE) != 0)
        {
            ERROR_LOG("2:unmap  failed(%s).", strerror(errno));
		   // return -1;
	    }
        return 0;
    }


	return 0;
}




int c_ucount::get()
{
	if (!m_inited) {
		return -1;
	}

	static const unsigned int byte_count_table[] = {
		0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
	};

	uint32_t count = 0;
	for (uint32_t i = 0; i != UFILE_SIZE / BLOCK_SIZE; ++i) {
		void *p_block_addr = mmap(NULL, BLOCK_SIZE, PROT_READ, MAP_SHARED, m_fd, i * BLOCK_SIZE);
		if (p_block_addr == MAP_FAILED) {
			return -1;
		}
		for (uint32_t j = 0; j != BLOCK_SIZE; ++j) {
			count += byte_count_table[*((unsigned char *)p_block_addr + j)];
		}

		if (munmap(p_block_addr, BLOCK_SIZE) != 0) {
			return -1;
		}
	}

	return count;
}

int c_ucount::traverse(cb_traverse_t cb_traverse, void *p_user_data, int skip_zero)
{
	if (!m_inited) {
		return -1;
	}

	void *p_block_addr = NULL;
	for (unsigned int i = 0; i != UFILE_SIZE * 8; ++i) {
		uint32_t block_index = i / (BLOCK_SIZE * 8);
		uint32_t offset = i % (BLOCK_SIZE * 8);

		if (offset == 0) {                                 // 切换block
			if (p_block_addr != NULL) {
				if (munmap(p_block_addr, BLOCK_SIZE) != 0) {
					return -1;
				}
			}

			p_block_addr = mmap(NULL, BLOCK_SIZE, PROT_READ, MAP_SHARED,
					m_fd, BLOCK_SIZE * block_index);
			if (p_block_addr == MAP_FAILED) {
				return -1;
			}
		}

		if (offset % 8 == 0 && skip_zero) {
			if (((char *)p_block_addr)[offset / 8] == 0) {
				i += 7;
				continue;
			}
		}

		int isset = get_block(p_block_addr, offset);
		if (!isset && skip_zero) {
			continue;
		}
		if (cb_traverse(i, isset, p_user_data) == -1) {
			return -1;
		}
	}

	if (p_block_addr != NULL) {
		if (munmap(p_block_addr, BLOCK_SIZE) != 0) {
			return -1;
		}
	}

	return 0;
}

int c_ucount::merge(const char *p_ucount_path_name, key_opcode_t key_opcode)
{
	i_ucount *p_ucount = NULL;
	if (create_ucount_instance(&p_ucount) != 0) {
		return -1;
	}

	if (p_ucount->init(p_ucount_path_name, 0, 0) != 0) {
		p_ucount->release();
		return -1;
	}

	if (merge(p_ucount, key_opcode) != 0) {
		p_ucount->uninit();
		p_ucount->release();
		return -1;
	}

	if (p_ucount->uninit() != 0) {
		p_ucount->release();
		return -1;
	}
	if (p_ucount->release() != 0) {
		return -1;
	}

	return 0;
}

int c_ucount::merge(i_ucount *p_ucount, key_opcode_t key_opcode)
{
	c_ucount *p_ucount_source = dynamic_cast<c_ucount *>(p_ucount);
	if (p_ucount_source == NULL) {
		return -1;
	}

	for (uint32_t i = 0; i != UFILE_SIZE / BLOCK_SIZE; ++i) {
		void *p_dest_block_addr = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
				m_fd, i * BLOCK_SIZE);
		if (p_dest_block_addr == MAP_FAILED) {
			return -1;
		}
		void *p_source_block_addr = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
				p_ucount_source->m_fd, i * BLOCK_SIZE);
		if (p_source_block_addr == MAP_FAILED) {
			return -1;
		}

		merge_block(p_dest_block_addr, p_source_block_addr, key_opcode);

		if (munmap(p_dest_block_addr, BLOCK_SIZE) != 0) {
			return -1;
		}
		if (munmap(p_source_block_addr, BLOCK_SIZE) != 0) {
			return -1;
		}
	}

	return 0;
}

int c_ucount::uninit()
{
	if (!m_inited) {
		return -1;
	}

	if (m_fd != -1) {
		close(m_fd);
	}

	m_inited = 0;

	return 0;
}

int c_ucount::release()
{
	delete this;

	return 0;
}

void c_ucount::set_block(void *p_block_addr, uint32_t offset, value_opcode_t value_opcode)
{
	switch (value_opcode) {
		case SET:
			((char *)(p_block_addr))[offset / 8] |= (1 << (offset % 8));
			break;
		case UNSET:
			((char *)(p_block_addr))[offset / 8] &= ~(1 << (offset % 8));
			break;
		case NOT:
			((char *)(p_block_addr))[offset / 8] ^= (1 << (offset % 8));
			break;
		default:
			abort();
			break;
	}
}

int c_ucount::get_block(const void *p_block_addr, uint32_t offset)
{
	return (((char *)(p_block_addr))[offset / 8] & (1 << (offset % 8))) ? 1 : 0;
}

void c_ucount::merge_block(void *p_dest_block_addr, const void *p_source_block_addr, key_opcode_t key_opcode)
{
	switch (key_opcode) {
		case INTERSECT:
			for (uint32_t i = 0; i != BLOCK_SIZE; ++i) {
				if (((char *)p_dest_block_addr)[i] ==
						(((char *)(p_dest_block_addr))[i] & ((char *)(p_source_block_addr))[i])) {
					continue;
				}
				((char *)p_dest_block_addr)[i] =
					((char *)(p_dest_block_addr))[i] & ((char *)(p_source_block_addr))[i];
			}
			break;
		case EXCEPT:
			for (uint32_t i = 0; i != BLOCK_SIZE; ++i) {
				if (((char *)p_dest_block_addr)[i] ==
						(((char *)(p_dest_block_addr))[i] & ~((char *)(p_source_block_addr))[i])) {
					continue;
				}
				((char *)p_dest_block_addr)[i] =
					((char *)(p_dest_block_addr))[i] & ~((char *)(p_source_block_addr))[i];
			}
			break;
		case UNION:
			for (uint32_t i = 0; i != BLOCK_SIZE; ++i) {
				if (((char *)p_dest_block_addr)[i] ==
						(((char *)(p_dest_block_addr))[i] | ((char *)(p_source_block_addr))[i])) {
					continue;
				}
				((char *)p_dest_block_addr)[i] =
					((char *)(p_dest_block_addr))[i] | ((char *)(p_source_block_addr))[i];
			}
			break;
		default:
			abort();
			break;
	}
}
