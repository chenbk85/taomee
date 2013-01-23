#ifndef _PRIZE_HPP_
#define _PRIZE_HPP_

#include "proto.hpp"

enum {
    /*! 奖励来源开始*/
    prize_source_begin     = 0,
    /*! 来自于任务的奖励*/
    prize_source_task      = 1,
    /*!TODO(singku) 其他来源的奖励 add below*/

    /*! 奖励来源结束*/
    prize_source_end
};

enum {
    /*! 奖励类型开始*/
    prize_type_begin        = 0,
    /*! 奖励经验值*/
    prize_type_exp          = 1,
    /*! 奖励金币*/
    prize_type_gold         = 2,
    /*! 奖励道具*/
    prize_type_item         = 3,
    /*! 奖励装备*/
    prize_type_equip        = 4,
    /*!TODO(singku) 其他奖励add below*/

    prize_type_end
};

enum {
    /*! 奖励项开始*/
    prize_seq_start         = 1,

    /*! 固定奖励*/
    prize_fixed             = 1,
    /*! 可选奖励*/
    prize_optional          = 0,
};

struct prize_t {
    /*! 奖励的属性 true表示固定奖励 fasle表示可选奖励*/
    bool fixed;     
    /*! 奖励类型*/
    uint32_t type;
    /*! 对应的ID(有的类型没有ID,比如金钱,经验,有的类型有ID,比如物品装备)*/
    uint32_t id;
    /*! 一次奖励的基数(实际奖励可能根据不同的用户经验值做一些调整)*/
    uint32_t num;
};

typedef std::vector<prize_t> prize_vec_t;

struct prize_info_t {
    /*! 奖励ID编号*/
    uint32_t prize_id;
    /*! 该奖励对应哪些具体的奖励项*/
    prize_vec_t *prizes;
};

extern std::map<uint32_t, prize_info_t*> prize_config_map;

/**
 * @brief 读取奖励配置
 * @para prize_node load_xmlconf解析出来的XML头结点
 * @return 解析成功返回0 解析失败返回-1，失败原因见error_log
 */
int init_prize_data(xmlNodePtr prize_node);

/**
 * @brief 释放奖励配置文件占用的内存
 */
void final_prize_data();

/**
 * @brief 给用户p以pirze_id对应的奖励 先调用db写到DB后再写内存
 * @para p user, prize_id奖励ID seq 处理到第一个奖励项
 * @return 0 成功调用send_to_db -1失败
 * @note 关于用户能否增加该奖励 需要事先做判断
 */
int get_prize(player_t *p, uint32_t prize_id, uint32_t seq = prize_seq_start);

/**
 * @brief 给用户p增加一个奖励为prize_id的奖励,所有发放奖励的地方都可以调用本函数
 * @para p 角色，prize_id 奖励ID
 * @notice 调用时注意,如果调用该函数时 没有waitcmd,但在DB完成之前 又有客户端协议过来
 * 而又没来得及给客户端响应，此时该函数的DB回包,保存的waitcmd会不匹配。可能需要修改handle_db_return
 * @return 0 成功 其他失败
 */
int add_prize(player_t *p, uint32_t prize_id);


/**
 * @brief 玩家登录时拉取未领取的奖励
 */
int db_get_prize_list(player_t *p);

int db_add_prize(player_t *p, uint32_t prize_id);
int db_del_prize(player_t *p, uint32_t prize_id);

#endif /* _PRIZE_HPP_ END*/
