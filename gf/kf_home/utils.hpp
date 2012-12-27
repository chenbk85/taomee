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
#include <libtaomee/dirtyword/dirtyword.h>
#include <libtaomee/log.h>
}

#define MAX_NAME_LEN  16

#define CHECK_VAL_LE(p_, val_, max_) \
		do { \
			if ( (val_) > (max_) ) { \
				ERROR_LOG("val %d is greater than max %d", (val_), (max_)); \
				p_->send_header_to_player(p_->base_.waitcmd, cli_err_pkglen_error, 1); \
				return 0; \
			} \
		} while (0)

#define CHECK_VAL_GE(p_, val_, min_) \
		do { \
			if ( (val_) < (min_) ) { \
				ERROR_LOG("val %d is less than min %d", (val_), (min_)); \
				p_->send_header_to_player(p_->base_.waitcmd, cli_err_pkglen_error, 1); \
				return 0; \
			} \
		} while (0)

#define CHECK_VAL_EQ(p_, val_, expect_) \
		do { \
			if ( (val_) != (expect_) ) { \
				ERROR_LOG("val %d is no equal to the expected %d", (val_), (expect_)); \
				p_->send_header_to_player(p_->base_.waitcmd, cli_err_pkglen_error, 1); \
				return 0; \
			} \
		} while (0)

#define SAFE_DELETE(p_) \
		do { \
			if (p_) { \
				delete (p_); \
				(p_) = 0; \
			} \
		} while (0)

#define SAFE_DELETE_VEC(p_) \
		do { \
			if (p_) { \
				delete[] (p_); \
				(p_) = 0; \
			} \
		} while (0)

#endif // KF_UTILS_HPP_

