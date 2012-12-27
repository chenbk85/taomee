#ifndef UTIL_H
#define UTIL_H
/** @brief 检验包长 */
#define CHECK_VAL_LE(val_, max_) \
		if ( (val_) > (max_) ) ERROR_RETURN(("val %d is greater than max %d", (val_), (max_)), CLOSE_CONN)
/** @brief 检验包长 */
#define CHECK_VAL_GE(val_, min_) \
		if ( (val_) < (min_) ) ERROR_RETURN(("val %d is less than min %d", (val_), (min_)), CLOSE_CONN)
/** @brief 检验包长 */
#define CHECK_VAL(val_, expect_) if ( (val_) != (expect_) ) ERROR_RETURN(("val %d is no equal to the expected %d", (val_), (expect_)), CLOSE_CONN)

/** @brief 进行脏词检测 */

#define CHECK_DIRTYWORD(p_, msg_) \
		do { \
			int r_ = tm_dirty_check(7, reinterpret_cast<char*>(msg_)); \
			if(r_ > 0) { \
				send_to_self_error((p_), INVALID_NICK_NAME);\
				DEBUG_LOG("check_dirty_word failed:%s ",msg_);\
				return -1; \
			} \
		} while (0)




#endif
