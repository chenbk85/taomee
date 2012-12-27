/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file ucount.h
 * @author richard <richard@taomee.com>
 * @date 2010-05-10
 */

#ifndef UCOUNT_H_2010_05_10
#define UCOUNT_H_2010_05_10

#include "../i_ucount.h"

class c_ucount : public i_ucount
{
public:
	c_ucount();

	virtual ~c_ucount();

	virtual int init(const char *p_ucount_path_name, uint32_t flags, mode_t mode);

	virtual int set(uint32_t ucount, value_opcode_t value_opcode);

    virtual int set_on_noucount(uint32_t ucount, value_opcode_t value_opcode);

	virtual int get(uint32_t ucount);

	virtual int get();

	virtual int traverse(cb_traverse_t cb_traverse, void *p_user_data, int skip_zero);

	virtual int merge(const char *p_ucount_path_name, key_opcode_t key_opcode);

	virtual int merge(i_ucount *p_ucount, key_opcode_t key_opcode);

	virtual int uninit();

	virtual int release();

private:
	void set_block(void *p_block_addr, uint32_t offset, value_opcode_t value_opcode);
	int get_block(const void *p_block_addr, uint32_t offset);
	void merge_block(void *p_dest_block_addr, const void *p_source_block_addr, key_opcode_t key_opcode);
	int m_inited;
	int m_fd;
};

#endif /* UCOUNT_H_2010_05_10 */

