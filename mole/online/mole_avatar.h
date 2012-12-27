/*
 * =====================================================================================
 *
 *       Filename:  mole_avatar.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  10/31/2011 02:07:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ericlee, ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

enum SAVE_AVATAR_RET
{
	SAR_OK		= 0,
	SAR_FULL,
	SAR_EMPTY,
};

enum REMOVE_AVATAR_RET
{
	RAR_OK		= 0,
	RAR_INVALID_ID,
};

int save_avatar_cmd(sprite_t* p, const uint8_t* body, int len);
int save_avatar_callback(sprite_t* p, uint32_t id, char* buf, int len);

int get_avatar_list_cmd(sprite_t* p, const uint8_t* body, int len);
int get_avatar_list_callback(sprite_t* p, uint32_t id, char* buf, int len);

int remove_avatar_cmd(sprite_t* p, const uint8_t* body, int len);
int remove_avatar_callback(sprite_t* p, uint32_t id, char* buf, int len);
