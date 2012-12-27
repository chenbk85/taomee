
#ifndef BIRTHDAY_H
#define BIRTHDAY_H

#include "benchapi.h"


int set_birthday_cmd(sprite_t * p, const uint8_t * body, int bodylen);
int set_birthday_callback(sprite_t * p, uint32_t id, char * buf,int len);
int dress_birthday_clothes_cmd (sprite_t *p, const uint8_t *body, int len);

#endif
