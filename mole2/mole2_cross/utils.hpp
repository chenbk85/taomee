#ifndef SWITCH_UTILS_HPP_
#define SWITCH_UTILS_HPP_

extern "C" {
#include <libtaomee/log.h>
}

#define CHECK_VAL_LE(val_, max_) \
		if ( (val_) > (max_) ) ERROR_RETURN(("val %d is greater than max %d", (val_), (max_)), -1)
#define CHECK_VAL_GE(val_, min_) \
		if ( (val_) < (min_) ) ERROR_RETURN(("val %d is less than min %d", (val_), (min_)), -1)
#define CHECK_VAL(val_, expect_) if ( (val_) != (expect_) ) ERROR_RETURN(("val %d is no equal to the expected %d", (val_), (expect_)), -1)

#define CHECK_VAL_GE_VOID(val_, min_) \
		do { \
			if ( (val_) < (min_) ) { \
				ERROR_LOG("val %d is less than min %d"); \
				return; \
			} \
		} while (0)

#define n_make_online_dead	1
#define n_try_match_again   2

#define REGISTER_TIMER_TYPE(nbr_, cb_, rstart_) \
		do { \
			if (register_timer_callback(nbr_, cb_) == -1) \
				ERROR_RETURN(("register timer type error\t[%u]", nbr_), -1); \
		} while(0)
		
template <typename DEST_T, typename SRC_T> union Ptr_Conver{
		SRC_T  p_src;
		DEST_T p_dest;
};

template <typename SRC_T, typename DEST_T> DEST_T ConvertPtrToType(SRC_T p_src)
{
		Ptr_Conver<DEST_T, SRC_T> ptr;
		ptr.p_src = p_src;
		return ptr.p_dest;
}

#define CONVERT_PTR_TO_PTR(src, dest) dest = ConvertPtrToType<typeof(src), typeof(dest)>(src)

#endif // SWITCH_UTILS_HPP_

