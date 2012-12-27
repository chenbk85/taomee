#ifndef CARD_MOD_H
#define CARD_MOD_H
#define CARD_TYPE_MAX	3
#define CARD_ID_MAX		60
#define CARD_VALUE_MAX	20
#define CARD_COLOUR_MAX	4
#define CARD_EVENT_MAX	16
#define CARD_STAR_MAX	6
//at the beginning, everybody can has 12 basic cards;

enum card_proto
{
	SVR_PROTO_CARD_DO_INIT			= 0x1179,
	SVR_PROTO_CARD_GET_INFO 		= 0x1075,
	SVR_PROTO_CARD_ADD_BASIC 		= 0x1178,
	SVR_PROTO_CARD_ADD_AVAIL	 	= 0x107B,
};

typedef struct cards_info
{
    uint32_t flag;
	uint32_t win_cnt;
	uint32_t lost_cnt;
	uint32_t basic_cnt;
	uint32_t senior_cnt;
	uint32_t total_cnt;
	uint32_t id[];
} __attribute__((packed)) cards_info_t;

typedef struct card
{
	uint32_t id;
	uint8_t type;
	uint8_t colour;
	uint8_t value;
	uint8_t event;
	uint8_t star;
} __attribute__((packed)) card_t;

typedef struct init_info
{
	uint32_t cnt;
	uint32_t id[];
}init_info_t;

int load_cards(const char *file);

int load_new_cards(const char *file);

int card_do_init(sprite_t* p, const uint8_t *body, int bodylen);

//get player's cards info
int card_get_info(sprite_t* p, const uint8_t *body, int bodylen);

//player can add 2 new basic cards when his rank go up;
int add_basic_card(sprite_t* p, const uint8_t *body, int bodylen);

int add_basic_card_avail(sprite_t* p, const uint8_t *body, int bodylen);


//callback function
int card_do_init_callback(sprite_t *p, uint32_t id, char *buf, int len);
int card_get_info_callback(sprite_t *p, uint32_t id, char *buf, int len);
int add_basic_card_callback(sprite_t *p, uint32_t id, char *buf, int len);
int add_basic_card_avail_callback(sprite_t *p, uint32_t id, char *buf, int len);

int get_new_card_exp_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_new_card_exp_callback(sprite_t *p, uint32_t id, char *buf, int len);

int new_card_do_init(sprite_t *p, uint32_t id);
int new_card_do_init_callback(sprite_t *p, uint32_t id, char *buf, int len);
int add_new_card_by_npc_talk_cmd(sprite_t *p, const uint8_t *body, int bodylen);
void random_array(uint32_t array[], int len);
int get_new_card_callback(sprite_t *p, uint32_t id, char *buf, int bodylen);
int check_red_clothe_callback(sprite_t *p, uint32_t id, char *buf, int bodylen);
int check_if_have_basic_card_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int check_if_have_basic_card_callback(sprite_t *p, uint32_t id, char *buf, int bodylen);
int get_add_card_id(uint32_t all_array_id[], int count);
int add_random_card_callback(sprite_t *p, uint32_t id, char *buf, int bodylen);
int old_new_card_info_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_new_card_exp_flag_cmd(sprite_t *p, const uint8_t *body, int bodylen);

#endif
