#ifndef MOLE_CANDY_H
#define MOLE_CANDY_H
void clear_array(uint32_t uid);
int load_candy(const char *file);
int process_ok(void* owner, void* data);
int occupy_position_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int query_item_cnt_callback(sprite_t *p, uint32_t id, char *buf, int len);
int make_candy_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_candy_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int fetch_candy_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_candy_from_recy_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_candy_callback(sprite_t *p, uint32_t id, char *buf, int len);
int do_add_make_timer(sprite_t * p);
int do_db_add_candy(sprite_t *p, uint32_t type);
int do_pkg_candy_and_exch(sprite_t* p, uint32_t type);
int get_position_info_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int get_candy_status_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int update_candy_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_candy_status_callback(sprite_t *p, uint32_t id, char *buf, int len);
int do_fetch_from_recy(sprite_t *p);
#endif

