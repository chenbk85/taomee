#ifndef UTIL_H
#define UTIL_H
/** @brief 检验包长 */
#define CHECK_VAL_LE(val_, max_) \
		if ( (val_) > (max_) ) ERROR_RETURN(("val %d is greater than max %d", (val_), (max_)), -1)
/** @brief 检验包长 */
#define CHECK_VAL_GE(val_, min_) \
		if ( (val_) < (min_) ) ERROR_RETURN(("val %d is less than min %d", (int)(val_), (int)(min_)), -1)
/** @brief 检验包长 */
#define CHECK_VAL(val_, expect_) if ( (val_) != (expect_) ) ERROR_RETURN(("val %d is no equal to the expected %u", (val_), (uint32_t)(expect_)), -1)

#endif
