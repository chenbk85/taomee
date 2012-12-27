/*
 * =====================================================================================
 *
 *       Filename:  Cdate.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/10/2009 10:42:32 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
extern "C" {
#include <stdint.h>
}
#include <ctime>
class Date {
	public:
		Date();

		Date(time_t time);

		uint32_t get_yymmdd();

		uint32_t get_yymm();

	private:

		time_t now;
};
