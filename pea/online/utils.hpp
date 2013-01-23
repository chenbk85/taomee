/**
 *============================================================
 *  @file      utils.hpp
 *  @brief    utilities
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_UTILS_HPP_
#define KF_UTILS_HPP_

extern "C" {
#include <libtaomee/tm_dirty/tm_dirty.h>
}

#define CHECK_VAL_LE(val_, max_) \
		do { \
			if ( (val_) > (max_) ) { \
				ERROR_TLOG("val %lu is greater than max %lu", (unsigned long)(val_), (unsigned long)(max_)); \
				return -1; \
			} \
		} while (0)

#define CHECK_VAL_GE(val_, min_) \
		do { \
			if ( (val_) < (min_) ) { \
				ERROR_TLOG("val %lu is less than min %lu", (unsigned long)(val_), (unsigned long)(min_)); \
				return -1;\
			} \
		} while (0)

#define CHECK_VAL_EQ(val_, expect_) \
		do { \
			if ( (val_) != (expect_) ) { \
				ERROR_TLOG("val %lu is no equal to the expected %lu", (unsigned long)(val_), (unsigned long)(expect_)); \
				return -1; \
			} \
		} while (0)

#define CHECK_VAL_GE_VOID(val_, min_) \
		do { \
			if ( (val_) < (min_) ) { \
				ERROR_TLOG("val %d is less than min %d"); \
				return; \
			} \
		} while (0)

#define CHECK_DBERR(p_, err_) \
		do { \
			if ( (err_) ) { \
					return send_header_to_player(p_, (p_)->waitcmd, (err_), 1); \
				}\
		} while (0)

#define CHECK_MC_SVR_ERR(p_, ret_) \
		do { \
			if ((ret_)) { \
				return send_header_to_player(p_, (p_)->waitcmd, cli_err_base_mcerr + (ret_), 1); \
			} \
		} while (0) 

#define CHECK_DIRTYWORD(p_, msg_) \
		do { \
			int r_ = tm_dirty_check(7, reinterpret_cast<char*>(msg_)); \
			if(r_>0) { \
				TRACE_TLOG("check_dirty_word: NG[%s] ",msg_);\
				return send_header_to_player(p_, (p_)->waitcmd, cli_err_dirtyword , 1); \
			} \
		} while (0)

#define CHECK_INVALID_WORD(p_, msg_) \
		do { \
			const char* bad_char = "\n\v\t\f\r"; \
			if (((msg_)[0] == '\0') || strpbrk(reinterpret_cast<char*>(msg_), bad_char)) { \
				return send_header_to_player(p_, (p_)->waitcmd, cli_err_dirtyword, 1); \
			} \
		} while (0)

#define CHECK_VIP_SVR_ERR(p_, ret_) \
		do { \
			if ((ret_)) { \
				return send_header_to_player(p_, (p_)->waitcmd, cli_err_base_storerr + (ret_), 1); \
			} \
		} while (0) 
		
#define CHECK_TRERR(p_, err_) \
		do { \
			if ( (err_) ) { \
				return send_header_to_player(p_, (p_)->waitcmd, err_, 1);\
			} \
		} while (0)


static inline uint64_t gen_key_by_uid_role(uint32_t user_id, uint32_t role_tm)
{
	uint64_t key = user_id;
	key = (key << 32) | role_tm;
	return key;
}

#endif // KF_UTILS_HPP_

