#ifndef MOLE_CAR_H
#define MOLE_CAR_H
#include "benchapi.h"
#include "dbproxy.h"

static inline int db_add_car(sprite_t* p, const uint32_t card_id, int price, uint32_t uid)
{
    uint32_t car_buf[2];
    car_buf[0] = card_id;
    car_buf[1] = price;
    return send_request_to_db(SVR_PROTO_BUY_CAR, p, sizeof(car_buf), car_buf, uid);
}

static inline int db_set_car_show(sprite_t* p, const uint32_t card_id, uint32_t show_flag, uint32_t uid)
{
    uint32_t car_buf[2];
    car_buf[0] = card_id;
    car_buf[1] = show_flag;
    return send_request_to_db(SVR_PROTO_SET_SHOW_CAR, p, sizeof(car_buf), car_buf, uid);
}

#define db_set_drive_time(p_, uid) \
        send_request_to_db(SVR_PROTO_SET_DRIVE_TIME, p_, 0, NULL, uid)

void pack_car_info(uint8_t* buf, const car_t* mycar, int* index);

int drive_out_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int buy_car_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int buy_car_callback(sprite_t *p, uint32_t id, char *buf, int len);

int car_info_callback(sprite_t *p, uint32_t id, char *buf, int len);
int drive_back_cmd(sprite_t* p, const uint8_t *body, int bodylen);

int list_cars_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int list_cars_callback(sprite_t *p, uint32_t id, char *buf, int len);

int set_show_car_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int set_show_car_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_show_car_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_show_car_callback(sprite_t *p, uint32_t id, char *buf, int len);


int check_car_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int check_car_callback(sprite_t *p, uint32_t id, char *buf, int len);
int refuel_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int refuel_callback(sprite_t *p, uint32_t id, char *buf, int len);

int get_jiazhao_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int set_drive_time_callback(sprite_t *p, uint32_t id, char *buf, int len);
int rent_zhaijisong_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int on_off_nangua_cmd(sprite_t* p, const uint8_t *body, int bodylen);

//int change_car_attr_cmd(sprite_t* p, const uint8_t *body, int bodylen);
#endif
