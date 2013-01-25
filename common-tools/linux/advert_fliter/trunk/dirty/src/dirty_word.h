/*
 * =====================================================================================
 *
 *       Filename:  dirty_word.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/30/2011 03:17:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ian (Ian), ian@taomee
 *        Company:  taomee co.
 *
 * =====================================================================================
 */


#ifndef CDIRTY_WORD_H_
#define CDIRTY_WORD_H_
class mysql_interface;
class Cdirty_word {
private:
	char sqlstr[512];
	mysql_interface* db;

	int save_bak_file();
	int load_bak_file();
public:
	bool is_inited;
	int reload_time;
	enum {
		dirty_file_cache_size	= 1024 * 1024,
		dirty_file_md5_len		= 16,
	};

	char file_cache[dirty_file_cache_size];
	unsigned char file_md5[dirty_file_md5_len];
	uint32_t total_len;
	
	Cdirty_word(int reload_time = 120);
	int reload();

};
#endif
