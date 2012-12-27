#ifndef ONLINE_PET_ITEM_H
#define ONLINE_PET_ITEM_H


#define PET_ITEM_KINDS_MAX_NUM		32
#define PET_ITEMS_MAX_NUM			1024

#define db_get_all_pets_weared(p_, id) \
		send_request_to_db(SVR_PROTO_GET_PET_WEARED, p_, 0, NULL, id)

int pet_wear_cloth(sprite_t * p,pet_t * pet,uint32_t itm);
int pet_unwear_cloth(sprite_t * p,pet_t * pet,uint32_t itm);
int pet_wear_honor(sprite_t * p,pet_t * pet,uint32_t itm);
int pet_unwear_honor(sprite_t * p,pet_t * pet,uint32_t itm);
int db_single_pet_item_op(sprite_t* p, uint32_t petid, uint32_t itmid, int count, int flag, uint32_t uid);
int buy_pet_item_cmd(sprite_t * p,const uint8_t * body,int bodylen);
int buy_pet_item_callback(sprite_t * p,uint32_t id,char * buf,int len);
int use_pet_cloth_cmd(sprite_t * p,const uint8_t * body,int bodylen);
int use_pet_honor_cmd(sprite_t * p,const uint8_t * body,int bodylen);
int db_single_pet_item_op_callback(sprite_t * p,uint32_t id,char * buf,int len);
int get_pet_items_cnt_cmd(sprite_t * p,const uint8_t * body,int bodylen);
int get_pet_item_count_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_pet_items_cnt_callback(sprite_t * p,uint32_t id,char * buf,int len);
int db_get_all_pet_weared_callback(sprite_t * p,uint32_t id,char * buf,int len);
int db_buy_pet_items(sprite_t* p, uint32_t petid, uint32_t itmid, int count, int free, uint32_t uid);
int use_pet_item_callback(sprite_t* p, uint32_t id, char* buf, int len);
int super_lamu_get_zhanpao_cmd(sprite_t* p, const uint8_t* body, int bodylen);


#endif

