#ifndef ONLINE_HALLOWMAS_H_
#define ONLINE_HALLOWMAS_H_

#define PUMPKIN  10002

int poison_gas(void * owner,void * data);
int become_pumpkin_expeired(void * owner,void * data);
int get_candy_count_cmd(sprite_t* p, uint8_t* body, int len);
int get_candy_count_callback(sprite_t* p, uint32_t id, char* buf, int len);
int sub_candy_count_cmd(sprite_t* p, uint8_t* body, int len);
int do_sub_candy_count(sprite_t* p);
int exchange_prop_cmd(sprite_t* p, uint8_t* body, int len);
int change_candy_prop_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_black_car_status_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int blackcat_car(void* owner, void* data);
int do_send_car_and_prop(sprite_t * p);
int get_candy_from_other_home_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int sub_vip_candy_callback(sprite_t* p, uint32_t id, char* buf, int len);
int vip_work_patch_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int do_real_send_gift(sprite_t * p);

#endif // ONLINE_HALLOWMAS_H_
