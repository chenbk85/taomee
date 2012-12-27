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
#include <libtaomee/log.h>
}

#define CHECK_VAL_LE(val_, max_) \
		do { \
			if ( (val_) > (max_) ) { \
				ERROR_LOG("val %lu is greater than max %lu", (unsigned long)(val_), (unsigned long)(max_)); \
				return -1; \
			} \
		} while (0)

#define CHECK_VAL_GE(val_, min_) \
		do { \
			if ( (val_) < (min_) ) { \
				ERROR_LOG("val %lu is less than min %lu", (unsigned long)(val_), (unsigned long)(min_)); \
				return -1;\
			} \
		} while (0)

#define CHECK_VAL_EQ(val_, expect_) \
		do { \
			if ( (val_) != (expect_) ) { \
				ERROR_LOG("val %lu is no equal to the expected %lu", (unsigned long)(val_), (unsigned long)(expect_)); \
				return -1; \
			} \
		} while (0)

#define CHECK_VAL_GE_VOID(val_, min_) \
		do { \
			if ( (val_) < (min_) ) { \
				ERROR_LOG("val %d is less than min %d"); \
				return; \
			} \
		} while (0)

#define CHECK_DBERR(p_, err_) \
		do { \
			if ( (err_) ) { \
				if ( (err_) == 3201 ) {\
					return send_header_to_player(p_, (p_)->waitcmd, cli_err_base_dberr + 3122, 1);\
				} else {\
					return send_header_to_player(p_, (p_)->waitcmd, cli_err_base_dberr + (err_), 1); \
				}\
			} \
		} while (0)

#define CHECK_MC_SVR_ERR(p_, ret_) \
		do { \
			if ((ret_)) { \
				return send_header_to_player(p_, (p_)->waitcmd, cli_err_base_mcerr + (ret_), 1); \
			} \
		} while (0) 

#define CHECK_DIRTYWORD(p_, msg_) \
		do { \
			char check_dirtyword_retmsg[8472] = "";\
			int r_ = tm_dirty_check_where(7, reinterpret_cast<char*>(msg_), check_dirtyword_retmsg); \
			if(r_>0) { \
				TRACE_LOG("check_dirty_word: NG[%s] ",msg_);\
				return notify_dirty_words(p, check_dirtyword_retmsg, strlen(check_dirtyword_retmsg));\
			} \
		} while (0)
		
#define REPLACE_DIRTYWORD(p_, msg_) \
		do { \
			int r_ = tm_dirty_replace(reinterpret_cast<char*>(msg_)); \
			if(r_==-1) { \
				ERROR_LOG("check_dirty_word: NG[%s] ",msg_);\
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

#endif // KF_UTILS_HPP_

