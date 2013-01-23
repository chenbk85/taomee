#ifndef H_PLAYER_EXTRA_INFO_H_2012_05_21
#define H_PLAYER_EXTRA_INFO_H_2012_05_21

#include "pea_common.hpp"
#include "proto.hpp"

class player_t;

enum
{
    EXTRA_INFO_BEGIN = 10000,
    // 免费的天赋随机次数
    EXTRA_INFO_FREE_RAND_GIFT = 10001,


    EXTRA_INFO_END
};

struct extra_info_t
{
    extra_info_t()
    {

    }

    uint32_t * id2p(uint32_t attr_id)
    {
        if (!(EXTRA_INFO_BEGIN < attr_id && attr_id < EXTRA_INFO_END))
        {
            return NULL;
        }

        return extra_info_arr + attr_id - EXTRA_INFO_BEGIN;
    }

    uint32_t id2value(uint32_t attr_id)
    {
        if (!(EXTRA_INFO_BEGIN < attr_id && attr_id < EXTRA_INFO_END))
        {
            return 0;
        }

        return extra_info_arr[attr_id - EXTRA_INFO_BEGIN];
    }


    // 免费的天赋随机次数
    uint32_t extra_info_arr[EXTRA_INFO_END];
};

int notify_extra_info(player_t * p, std::vector<uint32_t> info_id);

int notify_extra_info(player_t * p, uint32_t info_id);

int notify_extra_info(player_t * p, std::vector<db_extra_info_t> extra_info);

#endif
