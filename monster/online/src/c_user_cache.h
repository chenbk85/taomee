
#ifndef H_USER_CACHE_H_20110711
#define H_USER_CACHE_H_20110711

#include <map>
#include <set>
#include <vector>

#include "data_structure.h"

#include "online_structure.h"
#include "online_constant.h"

class c_restrict_cmp
{
public:
    bool operator()(const restrict_key_t &key_a, const restrict_key_t &key_b) const
    {
        if(key_a.time < key_b.time)
        {
            return true;
        }

        if(key_a.time == key_b.time)
        {
            if(key_a.type < key_b.type)
            {
                return true;
            }
        }

        return false;
    }
};

typedef uint32_t task_id_t;//任务id
typedef uint32_t last_reward_time_t;//上次任务奖励时间


class c_user_cache
{
    public:
        c_user_cache();
        ~c_user_cache();

        int init();
        int uninit();

        /*
         * 下面的函数只缓存登录用户的个人信息
         */
        //增加用户角色信息的缓存
        int add_role(const role_cache_t *p_role);
        //获得缓存的用户角色信息,NULL表示失败
        role_cache_t *get_role();

        //缓存用户默认房间
        int add_default_room(room_value_t *p_room_value);
        //获得用户的房间数据
        room_value_t *get_default_room();
        //缓存用户默认房间
        int add_cur_room(uint32_t room_id, room_value_t *p_room_value);
        //获得用户的最后拉取的非默认房间数据
        room_value_t *get_cur_room();
        //获得用户最后拉取的非默认房间id,0表示没有拉取过除默认房间外的其他房间
        uint32_t get_cur_room_id();
        //更新用户的房间缓存
        int update_room(room_update_t *p_room);

        //缓存用户的房间id
        int add_room_id(uint32_t *p_room_id, int p_room_num);
        //获得用户的所有房间id
        int get_all_room_id(uint32_t *p_room_id, int *p_room_num);

        //获得用户的背包数据
        int get_bag(db_msg_get_bag_rsp_t *p_bag);
        //缓存用户的背包物品
        int add_bag(const db_msg_get_bag_rsp_t *p_bag);
        //获得物品可使用的数量
        int get_stuff_num(uint32_t stuff_id);
        //增加用户背包里物品的数量
        void add_stuff(uint32_t stuff_id, uint16_t stuff_num);
        //增加用户背包里物品的使用数量
        int add_stuff_used(uint32_t stuff_id, uint16_t used_num);
        //减少用户背包里物品的数量
        int desc_stuff(uint32_t stuff_id, uint16_t stuff_num);
        //减少用户背包里物品的使用数量
        int desc_stuff_used(uint32_t stuff_id, uint16_t used_num);
        //用户是否有该物品,-1失败，0不在背包，1在背包
        bool is_stuff_exist(uint32_t stuff_id);

        //保存用户拉取留言板信息的数据
        int add_pinboard(int page, pinboard_t *p_pinboard);
        //获得用户的留言板信息
        pinboard_t *get_pinboard(int page);
        //通过留言id获得用户的留言板信息
        pinboard_message_t *get_pinboard_by_id(int id);
        //删除用户的留言板信息
        void del_pinboard();

        //获得用户所有益智游戏的信息
        int get_all_puzzle(db_msg_puzzle_rsp_t *p_puzzle);
        //增加用户的益智游戏信息
        int add_puzzle(const db_msg_puzzle_rsp_t *p_puzzle);
        //增加用户的一类益智游戏信息
        int add_puzzle_type(int type, puzzle_info_t *p_puzzle);
        //获得一类益智游戏的信息
        puzzle_info_t *get_puzzle(int type);

        //获得种植园种的植物
        int get_all_plants(all_plant_t *p_all_plant);
        //增加一个种植园的植物
        int update_plant_in_hole(db_msg_add_plant_req_t* p_plant);
        //删除一个种植园的植物
        void del_plant(uint8_t hole_id);
        //删除所有的植物
        void del_all_plant();
        //获得一个坑的植物信息
        hole_info_t *get_hole_plant(uint8_t hole_id);
        int update_plant_info(plant_growth_t plant_growth);

        //增加吸引到的小怪兽
        int add_attract_pet(uint32_t pet_id);
        //获得吸引到的小怪兽,失败返回0，成功返回小怪兽id
        uint32_t get_attract_pet();
        //删除吸引到的小怪兽
        void del_attract_pet();

        //增加小怪兽
        int add_pet(pet_info_t *p_pet);
        //获得跟随的小怪兽
        void get_following_pet(following_pet_t *p_pet);
        //获得精灵园里所有的怪兽
        int get_all_pet(all_pet_t *p_pet);

        //增加小游戏的天限制
        void add_day_restrict(restrict_key_t key, uint32_t value);
        //获得小游戏的天限制
        int get_day_restrict(restrict_key_t key, uint32_t *p_value);
        void update_day_restrict(restrict_key_t key, uint32_t value);

        //获得所有的成就项(0:sucess  1:未做缓存 )
        int get_all_badge(all_badge_info_t *p_badge);
        //将所有未查看的成就项置为未领奖的状态
        int set_all_badge_to_no_reward();

        //将成就项加入缓存
        int add_all_badge(all_badge_info_t *p_badge);
        //将缓存中的未读成就数置未unread_badge_num
        int set_unread_badge(uint16_t unread_badge_num);
        //更改缓存中的成就状态
        int update_badge_status(badge_info_t *p_badge);
        //判断是否已经获取到该成就项
        int is_badge_acquired(badge_info_t *p_badge, uint8_t *p_need_update, uint8_t badge_type);

	int update_game_day_restrict(restrict_key_t key_coin, uint32_t restrict_coins, restrict_key_t key_item, uint32_t restrict_value);
        int add_game_day_restrict(restrict_key_t key_coin, uint32_t restrict_coins, restrict_key_t key_item, uint32_t restrict_value);
        int get_game_day_restrict(restrict_key_t key_coin, restrict_key_t key_item, uint32_t *p_restrict_coins, uint32_t *p_restrict_value);

        int update_game_level(game_t *p_game);
        int add_game_level(uint32_t game_id, uint32_t total_lvl_num, game_level_info_t *p_game_level);
        int get_game_level(uint32_t game_id, uint32_t total_level_num,  game_level_info_t *p_game_level);

        int level_unlocked(uint32_t game_id, uint32_t game_key, uint32_t level_id, uint32_t score, uint32_t unlock_cond,  uint8_t *p_need_update, uint32_t *p_badge_add);
        int unlock_next_level(uint32_t game_id, uint32_t level_id);



	int get_all_hole(all_hole_info_t *p_all_hole);
	int cached_all_hole(all_hole_info_t *p_all_hole);
    int update_hole_info_after_maintain(db_msg_maintain_req_t *p_req);
    int del_hole_reward_i(uint8_t hole_id, uint32_t reward_id);
	int get_hole_reward(uint8_t hole_id, single_hole_reward_t *p_hole_reward);
	int cache_all_hole_reward(db_all_hole_reward_rsp_t *p_all_reward);
	int is_hole_reward_cached();
    int del_plant_at_hole(uint8_t hole_id);
	int get_hole_reward_i(uint8_t hole_id, uint32_t reward_id, hole_sun_reward_t *sun_reward);

    int is_pet_exist(uint32_t pet_id);
    int del_a_pet(uint32_t pet_id);

    int get_npc_score(npc_score_info_t *p_npc);
    int set_npc_score(npc_score_info_t *npc);

    int get_game_changed(db_msg_changed_stuff_t *p_changed_stuff);
    int set_game_changed(db_msg_changed_stuff_t *p_changed_stuff);
    int is_stuff_changed(uint32_t game_id, uint32_t stuff_id);
    int update_game_changed(db_msg_get_changed_stuff_t *p_changed);

    int get_finished_task_list(as_msg_finished_task_rsp_t *p_finished_task);
    int add_finished_task(db_msg_finished_task_rsp_t *p_finished_task);
    int task_need_reward(uint32_t task_id, uint16_t lvl, uint32_t pre_mission, uint16_t reward_cycle);
    int update_task_list(uint32_t task_id, uint32_t reward_time);

	museum_info_t* get_museum_item(uint32_t museum_id);
	int add_museum_item(museum_info_t *museum_item);
    private:
        int m_inited;

        role_cache_t m_role;

        room_value_t *m_p_default_room;      //保存用户的默认房间信息
        int m_cur_room_id;                      //用户最后一次获取的非默认房间的id
        room_value_t *m_p_cur_room;          //保存最后一次获得的非默认房间的信息
        std::set<uint32_t> m_room_set;       //保存用户对应的所有房间id

        std::map<uint32_t, stuff_num_t> m_bag_map;
        std::map<int, puzzle_info_t> m_puzzle_map;
        std::map<uint8_t, plant_info_t> m_plant_map;
        std::map<int, pinboard_t *> m_pinboard_map;
        std::map<int, pinboard_message_t *> m_pinboard_id_map;      //留言id对应的留言信息

        std::map<uint32_t, pet_num_t> m_pet_map;    //保存用户跟随的小怪兽

        std::map<uint32_t, uint32_t> m_elf_map; //保存用户拥有的小精灵
        uint32_t m_attract_pet;           //保存用户吸引到的但是还未处理的小怪兽

        std::map<uint32_t, level_info_t> m_game_level_map;//小游戏的缓存 key为game_id和level_id的组合

        std::map<restrict_key_t, uint32_t, c_restrict_cmp> m_day_restrict_map;          //保存小游戏的每日限制

        std::map<uint32_t, badge_info_t> m_badge_map; //保存所有的成就项
        uint8_t m_badge_cached;//判断成就项是否有做缓存(0:未做缓存 1：做了缓存)

        hole_info_t m_hole[PLANTATION_NUM];//坑的信息
        std::map<uint32_t, hole_sun_reward_t> m_hole1_reward_map;//第一个坑上的奖励信息, key为奖励id
        std::map<uint32_t, hole_sun_reward_t> m_hole2_reward_map;//第二个坑上的奖励信息
        std::map<uint32_t, hole_sun_reward_t> m_hole3_reward_map;//第三个坑上的奖励信息

        uint8_t m_hole_cached;//种植园的坑是否有缓存(0:未做缓存 1:做了缓存)
        uint8_t m_hole_reward_cached;//种植园的阳光奖励是否缓存(0:未做缓存 1：做了缓存)

        npc_score_info_t m_npc_score;

        std::map<uint32_t, std::vector<uint32_t> > m_game_change_stuff_map; //记录用户已经兑换过的游戏当前期物品列表, key为游戏id

        std::map<task_id_t, last_reward_time_t> m_finished_task_map;//已完成任务列表
        uint8_t m_task_cached;//任务列表是否缓存

		std::map<uint32_t, museum_info_t> m_museum_info_map;//记录完成博物馆任务情况

    public:
        int get_cached_open_act(single_activity_t *p_nv);
        int set_cached_open_act(single_activity_t *p_nv);
        int update_cached_open_act(uint32_t reward_id);
        int can_get_open_reward(uint32_t reward_id);
    private:
        std::map<uint32_t, single_reward_t> m_open_act_reward_map;//存放公测活动的奖励
        uint8_t m_open_act_cached;

    public:
        int get_peer_real(uint32_t peer_id);
        int set_peer_real(peer_real_msg_num_t *p_peer_msg);
    private:
        peer_real_msg_num_t m_peer_msg;

};

#endif //H_USER_CACHE_H_20110711
