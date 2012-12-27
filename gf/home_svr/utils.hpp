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

#ifndef KFTRD_UTILS_HPP_
#define KFTRD_UTILS_HPP_

extern "C" {
#include <libtaomee/dirtyword/dirtyword.h>
#include <libtaomee/log.h>
}

#define CHECK_VAL_LE(val_, max_) \
		do { \
			if ( (val_) > (max_) ) { \
				ERROR_LOG("val %d is greater than max %d", (val_), (max_)); \
				return -1; \
			} \
		} while (0)

#define CHECK_VAL_GE(val_, min_) \
		do { \
			if ( (val_) < (min_) ) { \
				ERROR_LOG("val %d is less than min %d", (val_), (min_)); \
				return -1;\
			} \
		} while (0)

#define CHECK_VAL_EQ(val_, expect_) \
		do { \
			if ( (val_) != (expect_) ) { \
				ERROR_LOG("val %d is no equal to the expected %d", (int)(val_), (int)(expect_)); \
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
				return send_header_to_player(p_, (p_)->waitcmd, cli_err_base_dberr + (err_), 1); \
			} \
		} while (0)

#endif // KF_UTILS_HPP_

