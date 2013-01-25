#ifndef __CHANGE_THUMB_H
#define __CHANGE_THUMB_H

#include <glib.h>



void init_chg_thumb_hash();
void fini_chg_thumb_hash();
int get_chg_thumb_hash_size();
int change_picture_thumbs_cmd(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);
int change_picture_thumbs_callback(uint32_t userid, uint16_t cmd, int fd, void* pkg, int pkglen);

#endif
