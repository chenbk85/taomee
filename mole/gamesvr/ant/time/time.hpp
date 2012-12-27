#ifndef ANT_TIME_HPP_
#define ANT_TIME_HPP_

extern "C" {
#include <sys/time.h>
}

#include <cmath>

namespace ant {

/**
 * timecmp - compare if @tv1 equals @tv2
 *
 * return: 0 if equal, positive value if tv1 > tv2, otherwise tv2 > tv1
 */
inline suseconds_t timecmp(const timeval& tv1, const timeval& tv2)
{
	if (tv1.tv_sec > tv2.tv_sec) {
		return 1;
	} else if (tv1.tv_sec == tv2.tv_sec) {
		return (tv1.tv_usec - tv2.tv_usec);
	} else {
		return -1;
	}
}

/**
 * timeadd - add @tmplus in sec to @tv
 *
 */
inline void timeadd(timeval& tv, double tmplus)
{
	if ( tmplus > 1.0 ) {
		tv.tv_sec  += static_cast<time_t>(tmplus);
		tmplus     -= static_cast<time_t>(tmplus);
	}
	tv.tv_usec += lround(tmplus * 1000000.0);
	if (tv.tv_usec > 999999) {
		tv.tv_sec  += tv.tv_usec / 1000000;
		tv.tv_usec  = tv.tv_usec % 1000000;
	}
}

/**
 * timediff - time difference of @tv1 and @tv2
 *
 * return: time diff in sec
 */
inline double timediff(const timeval& tv1, const timeval& tv2)
{
	return tv1.tv_sec - tv2.tv_sec + (tv1.tv_usec - tv2.tv_usec)/1000000.0;
}

} // namespace ant

#endif // ANT_TIME_HPP_
