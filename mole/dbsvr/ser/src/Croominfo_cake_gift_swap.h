/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_cake_gift_swap.h
 *
 *    Description:  蛋糕与礼品之间可以进行交换，此处正理蛋糕的计数
 *
 *        Version:  1.0
 *        Created:  11/26/2009 01:50:26 PM
 *       Revision:  none
 *       Compiler:  gcc -Wall -Wextra Croominfo_cake_gift_swap.h
 *
 *         Author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *        Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#ifndef  CROOMINFO_CAKE_GIFT_INCL
#define  CROOMINFO_CAKE_GIFT_INCL

#include "CtableRoute10x10.h"
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"

class Ccake_gift_swap : public CtableRoute10x10
{
public:
    Ccake_gift_swap(mysql_interface * db):CtableRoute10x10(db, "ROOMINFO","t_roominfo_cake_gift","user_id"){};

    int cake_add(const userid_t uid, const uint32_t n);
    int cake_subtract(const userid_t uid, const uint32_t n);
    int cake_get(const userid_t uid, uint32_t& cake_num_out)
    {
    	return sql_get(uid, cake_num_out);
    }

private:

    int sql_insert(const userid_t uid);
    int sql_update(const userid_t uid, const uint32_t cake_num);
    int sql_get(const userid_t uid, uint32_t& cake_num);
};

#endif //CROOMINFO_CAKE_GIFT_INCL

