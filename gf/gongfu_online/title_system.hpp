#ifndef __TITLE_SYSTEM_HPP__
#define __TITLE_SYSTEM_HPP__

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
}

//#include "player.hpp"
#include "fwd_decl.hpp"
#include <map>
#include <vector>
/*
typedef int (*callback_fun)(uint32_t );
struct callback_entry{  
    int key;    
    callback_fun function;
};
*/
#define max_title_len 20
#define max_title_stage_num 50

//struct player_t;

#pragma pack(1)

struct title_pve_condition_t {
    //uint32_t id;
    uint8_t world;
    uint32_t achieve_id;
    char title_name[max_title_len];
    std::vector<uint32_t> stageid;
    std::vector<uint8_t> difficulty;
    std::vector<uint8_t> score;
};

struct title_achieve_t {
    uint8_t world;
    char title_name[max_title_len];
    std::vector<uint32_t> achieveid;
};

struct title_other_t {
	uint32_t title_id;
    uint8_t world;
    char title_name[max_title_len];
};

struct title_info_t {
    uint8_t type;
    uint32_t subid;
    uint32_t gettime;
    title_info_t(uint8_t t, uint32_t id, uint32_t intime = time(NULL)){
        type = t;
        subid = id;
        gettime = intime;
    }
};

#pragma pack()

typedef std::map<uint32_t,title_pve_condition_t> title_pve_condition_map;
typedef std::map<uint32_t,title_achieve_t> title_achieve_map;
typedef std::map<uint32_t,title_other_t> title_other_map;


struct title_condition_t {
	char pve_note1[1028];
	char pve_note2[1028];
    title_pve_condition_map pve_map;
    title_achieve_map achieve_map;
	title_other_map other_map;
};
//global v
//title_condition_t g_title_condition = {0};

enum title_type {
    pve_title = 1,
    pvp_title = 2,
    enum_achieve_title = 3,
    other_title = 4,
};

typedef std::map<uint32_t, title_info_t> TitleMap;

class CTitle 
{
public:
    CTitle(){}
    ~CTitle()
    {
        last_title.clear();
        title_map.clear();
    }
    /**
     * @brief save one title 
     */
    int save_title(uint8_t type, uint32_t id, uint32_t gettime)
    {
        TitleMap::iterator it = title_map.find(id);
        if ( it != title_map.end() ) {
            TRACE_LOG("title [%u] exist", id);
            return 0;
        }
        title_info_t title(type, id, gettime);
        title_map.insert(TitleMap::value_type(id, title));
        TRACE_LOG("save title [%u %u %u]", type, id, gettime);
        return 0;
    }
    /**
     * @brief save title in last_title 
     */
    int save_last_title(uint8_t type, uint32_t gettime)
    {
        for (uint32_t i = 0; i < last_title.size(); i++) {
            save_title(type, last_title[i], gettime);
        }
        //last_title.clear();
        return 0;
    }
    /**
     * @brief fatch one title by titleid
     */
    title_info_t* get_title(uint32_t id)
    {
        TitleMap::iterator it = title_map.find(id);
        if ( it != title_map.end() ) {
            return &(it->second);
        }
        return NULL;
    }
    bool is_get_title( uint32_t id)
    {
        return title_map.find(id) != title_map.end();
    }
    /**
     * @brief delete one title by titleid
     */
    int del_title(uint32_t id)
    {
        TitleMap::iterator it = title_map.find(id);
        if ( it != title_map.end() ) {
            TRACE_LOG("erase:[%u %u %u]", it->second.type, it->second.subid, it->second.gettime);
            title_map.erase(it);
        }
        return 0;
    }
    /**
     * @return the count of get title
     */
    int calc_title(player_t* p, uint8_t type);
private:
    /**
     * @brief traverse g_title_condition and save last_title
     * @return the count of get title
     */
    int is_achieve_pve_title(player_t* p, uint8_t type);
    int is_finish_achieve_title(player_t *p, uint8_t type);
private:
    TitleMap title_map;
public:
    std::vector<uint32_t> last_title;
};


inline bool is_digit(char c) 
{
    if (c > 0x39 || c < 0x30) {
        return false;
    }
    return true;
}

/**
 * @brief  db set achieve title
 * @param p the requester
 * @param type :1 pve; 2 pvp
 */
int db_set_achieve_title(player_t* p, uint8_t type, uint32_t tm);

/**
 * @brief get achieve title list
 */
int db_get_achieve_title_list(player_t* p);

/**
  * @brief player performs periodical action : get achieve title
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_achieve_title_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief player performs periodical action : set player achieve title
  * @param p the player who launches the request
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int set_player_achieve_title_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief callback for handling adding periodical action returned from dbproxy
  * @param p the requester
  * @param uid id of the requester
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */ 
int db_get_achieve_title_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for handling adding periodical action returned from dbproxy
  * @param p the requester
  * @param uid id of the requester
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 on error
  */ 
int db_set_player_achieve_title_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 * @brief notify player get achieve title
 */
int notify_player_get_achieve_title(player_t* p, uint8_t type, uint32_t gettime);

/**
 * @brief get title name from g_condition_title
 */
bool get_title_name(uint32_t titleid, uint8_t title_type, char* name);

int do_special_title_logic(player_t* p, uint32_t id, bool is_bcast = true);
/**
 * @brief Interface of add titile to player
 */
int add_title_interface(player_t* p, uint8_t type);


/**
 * @brief send achieve title to self
 */
int send_achieve_title_to_player(player_t* p);

/**
 * @brief load achieve title info from conf/title.xml file
 * @param cur xml file point
 * @return 0 on success, -1 on error
 */
int load_title_config(xmlNodePtr cur);

#endif //__TITLE_SYSTEM_HPP__
