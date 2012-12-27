/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  j
 *
 *        Version:  1.0
 *        Created:  06/09/2010 08:23:25 PM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include "login_impl.hpp"
#include "time_stamp_impl.hpp"
#include "switch_impl.hpp"


MoleLogin login(new SwitchService(), new TimeStampService());
