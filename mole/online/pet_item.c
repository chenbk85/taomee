#include <libtaomee/bitmanip/bitmanip.h>

#include "proto.h"
#include "exclu_things.h"
#include "message.h"
#include "communicator.h"
#include "lahm_sport.h"
#include "lamu_skill.h"
#include "pet_item.h"
#include "event.h"


int is_pet_wear_cloth(pet_t* pet, uint32_t id)
{
	int i;
	for (i = 0; i < pet->suppl_info.pet_cloth_cnt; i++) {
		DEBUG_LOG("IS WEAR\t[%u %u %u]", pet->id, pet->suppl_info.pet_clothes[i], pet->suppl_info.pet_cloth_cnt);
		if (pet->suppl_info.pet_clothes[i] == id) {
			return 1;
		}
	}
	return 0;
}

int pet_wear_cloth (sprite_t *p, pet_t* pet, uint32_t itm)
{
	item_t* pitm = get_item_prop(itm);
	if (!pitm || (!PET_IS_SUPER_LAHM(pet) && IS_SUPER_LAHM_ITM(pitm)) || (PET_IS_SUPER_LAHM(pet) && IS_NOMAL_LAHM_ITM(pitm)))
		ERROR_RETURN (("item not suitable the pet\t[%u %u %u]", p->id, pet->id, itm), -1);
	if (pet->suppl_info.pet_cloth_cnt >= PET_CLOTH_MAX_CNT)
		ERROR_RETURN (("pet wear too many item, count=%d, uid=%u petid=%u", pet->suppl_info.pet_cloth_cnt, p->id, pet->id), -1);
	pet->suppl_info.pet_clothes[pet->suppl_info.pet_cloth_cnt++] = itm;
	return 0;
}

int pet_unwear_cloth (sprite_t *p, pet_t* pet, uint32_t itm)
{
	int i;
	for (i = 0; i < pet->suppl_info.pet_cloth_cnt; i++) {
		if (pet->suppl_info.pet_clothes[i] == itm) {
			pet->suppl_info.pet_clothes[i] = pet->suppl_info.pet_clothes[pet->suppl_info.pet_cloth_cnt - 1];
			pet->suppl_info.pet_clothes[pet->suppl_info.pet_cloth_cnt - 1] = 0;
			pet->suppl_info.pet_cloth_cnt--;
			return 0;
		}
	}

	ERROR_RETURN (("pet not wear item=%u, uid=%u petid=%u", itm, p->id, pet->id), -1);
}

int is_pet_wear_honor(pet_t* pet, uint32_t id)
{
	int i;
	for (i = 0; i < pet->suppl_info.pet_honor_cnt; i++) {
		DEBUG_LOG("IS WEAR\t[%u %u %u]", pet->id, pet->suppl_info.pet_honor[i], pet->suppl_info.pet_honor_cnt);
		if (pet->suppl_info.pet_honor[i] == id) {
			return 1;
		}
	}
	return 0;
}

int pet_wear_honor (sprite_t *p, pet_t* pet, uint32_t itm)
{
	if (pet->suppl_info.pet_honor_cnt >= PET_CLOTH_MAX_CNT)
		ERROR_RETURN (("pet wear too many honor, count=%d, uid=%u petid=%u", pet->suppl_info.pet_honor_cnt, p->id, pet->id), -1);
	pet->suppl_info.pet_honor[pet->suppl_info.pet_honor_cnt++] = itm;
	return 0;
}

int pet_unwear_honor (sprite_t *p, pet_t* pet, uint32_t itm)
{
	int i;
	for (i = 0; i < pet->suppl_info.pet_honor_cnt; i++) {
		if (pet->suppl_info.pet_honor[i] == itm) {
			pet->suppl_info.pet_honor[i] = pet->suppl_info.pet_honor[pet->suppl_info.pet_honor_cnt - 1];
			pet->suppl_info.pet_honor[pet->suppl_info.pet_honor_cnt - 1] = 0;
			pet->suppl_info.pet_honor_cnt--;
			return 0;
		}
	}

	ERROR_RETURN (("pet not wear honor=%u, uid=%u petid=%u", itm, p->id, pet->id), -1);
}


/*
 * flag:
 * 	del_unused_itm	0x00
 * 	add_itm			0x01
 * 	unused_itm		0x02
 *	used_itm			0x03
 * 	del_used_itm		0x04
 */
int db_single_pet_item_op(sprite_t* p, uint32_t petid, uint32_t itmid, int count, int flag, uint32_t uid)
{
	int j;
	char buffer[20];

	const item_t* itm = get_item_prop(itmid);
	if (!itm) {
		ERROR_RETURN(("Item %u not found: uid=%u petid=%u", itmid, (p ? p->id : uid), petid), -1);
	}

	j = 0;
	PKG_H_UINT32(buffer, petid, j);
	PKG_H_UINT32(buffer, itmid, j);
	PKG_H_UINT32(buffer, flag, j);
	PKG_H_UINT32(buffer, count, j);
	if (!flag || (flag == 4)) {
		PKG_H_UINT32(buffer, 99999, j);
	} else {
		PKG_H_UINT32(buffer, itm->max, j);
	}

	return send_request_to_db(SVR_PROTO_SET_PET_ITEM, p, 20, buffer, uid);
}

int db_buy_pet_items(sprite_t* p, uint32_t petid, uint32_t itmid, int count, int free, uint32_t uid)
{
	uint32_t buf[5];

	const item_t* itm = get_item_prop(itmid);
	if (!itm) {
		ERROR_RETURN(("Item %u not found: uid=%u petid=%u", itmid, (p ? p->id : uid), petid), -1);
	}

    item_kind_t* kind = find_kind_of_item(itmid);
    if (!kind) {
		ERROR_RETURN(("Item %u not found: uid=%u petid=%u", itmid, (p ? p->id : uid), petid), -1);
	}

    uint32_t money = (free ? 0 : (itm->price * count));

	// judge if discount day
	const event_t* ev = get_event(5);
	if (ev->status) {
		if (kind->kind == PET_ITEM_CLOTH_KIND) {
			money *= 0.5;
		}
	}

	buf[0] = petid;
	buf[1] = itmid;
	buf[2] = count;
	buf[3] = itm->max;
	buf[4] = money;

	return send_request_to_db(SVR_PROTO_BUY_PET_ITEM, p, 20, buf, uid);
}

int db_get_pet_items_cnt(sprite_t* p, uint32_t petid, uint32_t start_idx, uint32_t end_idx, uint32_t flag, uint32_t uid)
{
	uint32_t buf[4];

	buf[0] = petid;
	buf[1] = start_idx;
	buf[2] = end_idx;
	buf[3] = flag;

	return send_request_to_db(SVR_PROTO_GET_PET_ITEM_LIST, p, 16, buf, uid);

}


/*---------------------------------------------------
  *  func:  get_pet_item_cnt_cmd
  *  abstr: get the count of a given user's given item
  *---------------------------------------------------*/
int get_pet_items_cnt_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 17);
	CHECK_VALID_ID(p->id);

	uint32_t uid, petid, itmid_start, itmid_end;
	uint8_t  flag;

	int i = 0;
	UNPKG_UINT32(body, uid, i);
	UNPKG_UINT32(body, petid, i);
	UNPKG_UINT32(body, itmid_start, i);
	UNPKG_UINT32(body, itmid_end, i);
	UNPKG_UINT8(body, flag, i);  	//0 not use, 1 used, 2, not use and used

	CHECK_VALID_ID(uid);
	if (flag > 2) {
		ERROR_RETURN( ("Wrong Flag=%d From Uid=%u", flag, p->id), -1 );
	}

	item_kind_t* itmk_start = find_kind_of_item(itmid_start);
	item_kind_t* itmk_end = find_kind_of_item(itmid_end);
	if (!itmk_start || !itmk_end || itmid_start >= itmid_end
		|| (itmk_start->kind != PET_ITEM_CLOTH_KIND
			&& itmk_start->kind != PET_ITEM_CERTIF_KIND
			&& itmk_start->kind != PET_ITEM_HONOR_KIND)
		|| (itmk_end->kind != PET_ITEM_CLOTH_KIND
			&& itmk_end->kind != PET_ITEM_CERTIF_KIND
			&& itmk_end->kind != PET_ITEM_HONOR_KIND)) {
		ERROR_RETURN( ("Invalid ItmID_start=%u ItmID_end=%u Chkee=%u Petid=%u Chker=%u", itmid_start, itmid_end, uid, petid, p->id), -1);
	}

	DEBUG_LOG("GET ITEMS CNT LIST\t[checker=%u checkee=%u petid=%u start=%u end=%u flag=%d]", p->id, uid, petid, itmid_start, itmid_end, flag);
	return db_get_pet_items_cnt(p, petid, itmid_start, itmid_end, flag, uid);
}


int get_pet_item_count_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t petid;
	uint32_t id_start;
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 8);

	int i = 0;
	UNPKG_UINT32(body, petid, i);
	UNPKG_UINT32(body, id_start, i);
	return db_get_pet_items_cnt(p, petid, id_start, id_start + 1, 2, p->id);
}

int get_pet_items_cnt_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 8);

	int petid = *(uint32_t*)buf;
	int count = *(uint32_t*)(buf + 4);
	CHECK_BODY_LEN(len - 8, count * 8);

	int i, j, k = 8;
	uint32_t itm, cnt;

	if (count < 0){
		ERROR_RETURN(("fault pet item num\t[%u %u %u]", p->id, petid, count), -1);
	}

	j = sizeof (protocol_t);
	PKG_UINT32 (msg, id, j);
	PKG_UINT32 (msg, petid, j);
	PKG_UINT32 (msg, count, j);
	for (i = 0; i < count; i++) {
		UNPKG_H_UINT32 (buf, itm, k);
		UNPKG_H_UINT32 (buf, cnt, k);
		PKG_UINT32 (msg, itm, j);
		PKG_UINT32 (msg, cnt, j);
	}

	init_proto_head (msg, p->waitcmd, j);

	DEBUG_LOG ("LIST PET ITEM\t[%u %u %d]", p->id, petid, count);
	return send_to_self (p, msg, j, 1);
}


int buy_pet_item_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 12);
	CHECK_VALID_ID(p->id);
	/*
	if (!p->followed) {
		ERROR_RETURN(("no pet follow uid=%u", p->id), -1);
	}
	*/
	uint32_t itemid, count, petid;

	int i = 0;
	UNPKG_UINT32(body, petid, i);
	UNPKG_UINT32(body, itemid, i);
	UNPKG_UINT32(body, count, i);

	if(!get_pet(p->id, petid)){
		ERROR_RETURN(("error petid, uid=%u petid=%u", p->id, petid), -1);
	}

	if (count > 100 || count <= 0)
		ERROR_RETURN(("error count=%d, uid=%u petid=%u", count, p->id, petid), -1);

	item_t* itm_prop = get_item_prop(itemid);
	if (!itm_prop || !itm_buyable(p, itm_prop)) {
		ERROR_RETURN( ("Item %u not found or Unbuyable: uid=%u itmflag=0x%X",
						itemid, p->id, (itm_prop ? itm_prop->tradability : 0)), -1 );
	}

	item_kind_t* kind = find_kind_of_item(itemid);
	if (!kind || (kind->kind != PET_ITEM_CLOTH_KIND && kind->kind != PET_ITEM_HONOR_KIND))
		ERROR_RETURN(("error item kind, item=%u, uid=%u petid=%u", itemid, p->id, petid), -1);

	DEBUG_LOG("BUY PET ITEM\t[buyer=%u petid=%u itemid=%u count=%u]", p->id, petid, itemid, count);
	return db_buy_pet_items(p, petid, itemid, count, 0, p->id);
}

int buy_pet_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 12);

	p->yxb = *(uint32_t*)buf;

	switch (p->waitcmd) {
	case PROTO_BUY_PET_ITEM:
		response_proto_uint32(p, p->waitcmd, p->yxb, 0);
		break;
	case PROTO_GET_VIP_LEVEL_LAHM_GIFTS:
		response_proto_head(p, p->waitcmd, 0);
		break;
	case PROTO_EXCHANGE_RAND_ITEM:
		response_proto_uint32(p, p->waitcmd, *(uint32_t*)p->session, 0);
		break;
	case PROTO_GET_LAHM_TEAM_CLOTH:
		DEBUG_LOG("PROTO_GET_LAHM_TEAM_CLOTH uid=%u item=%u",p->id, *(uint32_t*)p->session);
		response_proto_uint32(p, p->waitcmd, *(uint32_t*)p->session, 0);
		break;
	}
	return 0;
}


int pkg_pet_item(sprite_t* p, pet_t* pet, struct user_itm_use_buf* itm, uint32_t itemid, uint8_t is_used)
{
	if (!(itm->prop = get_item_prop(itemid))) {
		ERROR_RETURN(("error item id=%u, uid=%u", itemid, p->id), -1);
	}
	itm->id = itemid;
	item_kind_t* kind = find_kind_of_item(itemid);
	if (!kind || (kind->kind != PET_ITEM_CLOTH_KIND && kind->kind != PET_ITEM_HONOR_KIND))
		ERROR_RETURN(("error item kind, item=%u, uid=%u petid=%u", itemid, p->id, pet->id), -1);

	if (!is_used) { // put off clothes
		if (!is_pet_wear_cloth(pet, itm->id) && !is_pet_wear_honor(pet, itm->id)) {
			ERROR_RETURN( ("not use suit item, id=%u, petid=%u, item=%u", p->id, pet->id, itm->id), -1 );
		}
		return (itm->mask = USED_ITEM_MASK);
	} else { // put on clothes
		if (itm_vip_only(itm->prop) && !ISVIP(p->flag)){
			ERROR_RETURN( ("vip only item, id=%u, item=%d", p->id, itm->id), -1 );
		}
		if (is_pet_wear_cloth(pet, itm->id) || is_pet_wear_honor(pet, itm->id)) {
			ERROR_RETURN( ("already wear: uid=%u, petid=%u, item=%u flag=%d", p->id, pet->id, itemid, is_used), -1 );
		}
		return (itm->mask = ADD_ITEM_MASK | USED_ITEM_MASK);
	}
}


int use_pet_cloth_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint8_t wear_tag;
	uint32_t petid;
	uint32_t itemid;

	CHECK_BODY_LEN(bodylen, 9);
	CHECK_VALID_ID(p->id);

	int i = 0;
	UNPKG_UINT32(body, petid, i);
	UNPKG_UINT32(body, itemid, i);
	UNPKG_UINT8(body, wear_tag, i);

	pet_t* pet = get_pet(p->id, petid);
	if (!pet)
		ERROR_RETURN(("user no map or pet not exist\t[%u %u]", p->id, petid), -1);

//	if (wear_tag && enter_five_stage(pet)) {
//		return send_to_self_error(p, p->waitcmd, -ERR_five_stage_cannot_clothe, 1);
//	}

	item_t* pitm = get_item_prop(itemid);
	item_kind_t* kind = find_kind_of_item(itemid);
	if (!pitm || !kind || (kind->kind != PET_ITEM_CLOTH_KIND && kind->kind != PET_ITEM_HONOR_KIND))
		ERROR_RETURN(("error item kind, item=%u, uid=%u petid=%u", itemid, p->id, pet->id), -1);

	int k = 0;
	uint8_t buff[16];
	PKG_H_UINT32(buff, petid, k);

	if (kind->kind == PET_ITEM_CLOTH_KIND) {
		PKG_H_UINT32(buff, 1, k);     // cloth
	}
	else {
		PKG_H_UINT32(buff, 0, k);		// honor
	}

	if (wear_tag) {
		PKG_H_UINT32(buff, 1, k);   // cloth num
		PKG_H_UINT32(buff, itemid, k);
		*(uint32_t*)p->session = 1;
		*(uint32_t*)(p->session + 4) = itemid;
	} else {
		PKG_H_UINT32(buff, 0, k);
		*(uint32_t*)p->session = 0;
	}

	DEBUG_LOG("PUT ON PET %s\t[%u %u %u %u]",
		kind->kind == PET_ITEM_CLOTH_KIND ? "CLOTH":"HONOR", p->id, petid, itemid, wear_tag);
	return send_request_to_db(SVR_PROTO_PUT_ON_PET_ITEM, p, k, buff, p->id);
}


void response_proto_use_pet_items(sprite_t *p, uint32_t petid)
{
	int cnt;
	int j, i = sizeof (protocol_t);

	cnt = *(int *)(p->session + 4);
	PKG_UINT32 (msg, p->id, i);
	PKG_UINT32 (msg, petid, i);
	PKG_UINT32 (msg, cnt, i);
	for (j = 0; j < cnt; j++) {
		struct user_itm_use_buf *itms = (struct user_itm_use_buf *)
			(p->session + 12 + j * sizeof (struct user_itm_use_buf));
		PKG_UINT32 (msg, itms->id, i);
		if (itms->mask & ADD_ITEM_MASK)
			PKG_UINT8 (msg, 1, i);
		else
			PKG_UINT8 (msg, 0, i);
	}

	DEBUG_LOG("USE RSP\t[%u %u %u]", p->id, cnt, i);
	init_proto_head (msg, p->waitcmd, i);
	send_to_map (p, msg, i, 1);
}

int db_single_pet_item_op_callback(sprite_t* p, uint32_t id, char* buf, int len)
{

	uint32_t itm, used, total, petid;
	int *cnt, *req;
	struct user_itm_use_buf *itms = NULL;
	pet_t* pet;
	int i;
/*
	if (!p->followed) {
		ERROR_RETURN(("no pet follow uid=%u", p->id), -1);
	}
*/
	if (len != 12 || p->id != id){
		ERROR_RETURN(("error len=%d, id=%u, %u", len, p->id, id), -1);
	}

	i = 0;
	UNPKG_H_UINT32(buf, itm, i);
	UNPKG_H_UINT32(buf, used, i);
	UNPKG_H_UINT32(buf, total, i);

	switch (p->waitcmd) {
	case PROTO_PET_CLOTHES_USE:
		petid = *(uint32_t *)p->session;
		cnt = (int *)(p->session + 4);
		req = (int *)(p->session + 8);
		assert (*cnt  > *req && *cnt > 0);

		pet = get_pet(p->id, petid);
		if (!pet){
			ERROR_RETURN(("user no map or pet\t[%u %u]", p->id, petid), -1);
		}
		//search item mask
		for (i = 0; i < *cnt; i++) {
			itms = (struct user_itm_use_buf *)(p->session + 12 + i * sizeof (struct user_itm_use_buf));
			if (itms->id == itm)
				break;
		}
		if (i == *cnt)
			ERROR_RETURN(("can't find item: cnt=%d, itm=%d, id=%u petid=%u", *cnt, itm, p->id, petid), -1);

		if ((itms->mask & ADD_ITEM_MASK) && pet_wear_cloth(p, pet, itm) == -1){
			db_single_pet_item_op(0, pet->id, itms[*req].id, 1, 0x02, id);
			return -1;
		}
		if (!(itms->mask & ADD_ITEM_MASK) && pet_unwear_cloth(p, pet, itm) == -1)
			return -1;
		(*req)++;

		DEBUG_LOG("PUT ON CLOTH CALLBACK\t[%u %u %u]", p->id, *cnt, *req);
		if (*cnt  == *req) {
			response_proto_use_pet_items(p, petid);
			broadcast_sprite_info(p);
			p->sess_len = 0;
		} else {
			db_single_pet_item_op(p, pet->id, itms[*req].id, 1, itms[*req].mask, p->id);
		}
		break;
	case PROTO_PET_HONOR_USE:
		petid = *(uint32_t *)p->session;
		cnt = (int *)(p->session + 4);
		req = (int *)(p->session + 8);
		assert (*cnt  > *req && *cnt > 0);

		pet = get_pet(p->id, petid);
		if (!pet){
			ERROR_RETURN(("user no map or pet\t[%u %u]", p->id, petid), -1);
		}

		//search item mask
		for (i = 0; i < *cnt; i++) {
			itms = (struct user_itm_use_buf *)(p->session + 12 + i * sizeof (struct user_itm_use_buf));
			if (itms->id == itm)
				break;
		}
		if (i == *cnt)
			ERROR_RETURN(("can't find item: cnt=%d, itm=%d, id=%u petid=%u", *cnt, itm, p->id, petid), -1);

		if ((itms->mask & ADD_ITEM_MASK) && pet_wear_honor(p, pet, itm) == -1){
			db_single_pet_item_op(0, pet->id, itms[*req].id, 1, 0x02, id);
			return -1;
		}
		if (!(itms->mask & ADD_ITEM_MASK) && pet_unwear_honor(p, pet, itm) == -1)
			return -1;
		(*req)++;

		DEBUG_LOG("PUT ON HONOR CALLBACK\t[%u %u %u]", p->id, *cnt, *req);
		if (*cnt  == *req) {
			response_proto_use_pet_items(p, petid);
			broadcast_sprite_info(p);
			p->sess_len = 0;
		} else {
			db_single_pet_item_op(p, pet->id, itms[*req].id, 1, itms[*req].mask, p->id);
		}
		break;
	case PROTO_EXCHANGE_RAND_ITEM:
		response_proto_uint32(p, p->waitcmd, 1200036, 0);
		break;
	default:
		ERROR_RETURN(("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
	}
	return 0;
}

int use_pet_honor_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint8_t wear_tag;
	uint32_t petid;
	uint32_t itemid;
	int i, count, k;

	CHECK_BODY_LEN_GE(bodylen, 9);
	CHECK_VALID_ID(p->id);

	i = k = 0;
	UNPKG_UINT32(body, petid, i);
	UNPKG_UINT32(body, itemid, i);
	UNPKG_UINT8(body, wear_tag, i);


	pet_t* pet = get_pet(p->id, petid);
	if (!pet) {
		ERROR_RETURN(("user no map or pet\t[%u %u]", p->id, petid), -1);
	}

	PKG_H_UINT32(p->session, petid, k);
	k += 4;
	PKG_H_UINT32(p->session, 0, k);
	count = 1;

	struct user_itm_use_buf* itms = (struct user_itm_use_buf*)&p->session[12];
	if ((itemid != pet->suppl_info.pet_honor[0] && wear_tag) && pet->suppl_info.pet_honor[0] != 0) {
		//change honor
		if (pkg_pet_item(p, pet, itms, pet->suppl_info.pet_honor[0], 0) == -1)
			return -1;
		if (pkg_pet_item(p, pet, itms + 1, itemid, wear_tag) == -1)
			return -1;
		count++;
	}  else if ((itemid == pet->suppl_info.pet_honor[0] && wear_tag) && pet->suppl_info.pet_honor[0] != 0) {
		// do nothing
		int j = sizeof(protocol_t);
		PKG_UINT32 (msg, p->id, j);
		PKG_UINT32 (msg, petid, j);
		PKG_UINT32 (msg, count, j);
		PKG_UINT32 (msg, itemid, j);
		PKG_UINT8 (msg, 1, j);
		init_proto_head (msg, p->waitcmd, j);
		send_to_map (p, msg, j, 1);

		broadcast_sprite_info(p);
		p->sess_len = 0;
		DEBUG_LOG("PUT ON PET HONOR\t[%u %u]", p->id, petid);
		return 0;
	} else {
		//put on honor or put off honor
		if (pkg_pet_item(p, pet, itms, itemid, wear_tag) == -1)
			return -1;
	}
	*(uint32_t*)(p->session + 4)= count;

	DEBUG_LOG("PUT ON PET HONOR\t[%u %u %u]", p->id, petid, count);
	return db_single_pet_item_op(p, pet->id, itms[0].id, 1, itms[0].mask, p->id);
}

int db_get_all_pet_weared_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	map_t* map = get_map(id);
	if (!map) {
		ERROR_RETURN(("not alloc map eid=%u uid=%u", id, p->id), -1);
	}

	int count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len - 4, count * 12);

	//for debug,when too many user come the same home
	int loop;
	for (loop = 0; loop < map->pet_cnt; loop++) {
		map->pets[loop].suppl_info.pet_cloth_cnt = map->pets[loop].suppl_info.pet_honor_cnt = 0;
	}
	//end for
	int i, j, k = 4;
	uint32_t petid, itmid, cnt;
	for (i = 0; i < count; i++) {
		UNPKG_H_UINT32 (buf, petid, k);
		UNPKG_H_UINT32 (buf, itmid, k);
		UNPKG_H_UINT32 (buf, cnt, k);
		for (j = 0; j < map->pet_cnt; j++) {
			if (map->pets[j].id == petid){
				item_kind_t* kind = find_kind_of_item(itmid);
				if (kind && kind->kind == PET_ITEM_CLOTH_KIND){
					pet_wear_cloth(p, &(map->pets[j]), itmid);
					DEBUG_LOG ("WEARED ID\t[%u %u %d]", p->id, petid, itmid);
				} else if (kind && kind->kind == PET_ITEM_HONOR_KIND){
					pet_wear_honor(p, &(map->pets[j]), itmid);
					DEBUG_LOG ("WEARED ID\t[%u %u %d]", p->id, petid, itmid);
				} else {
					ERROR_RETURN(("error item kind, item=%u, uid=%u petid=%u", itmid, p->id, petid), -1);
				}
				break;
			}
		}
	}

	switch (p->waitcmd) {
	case PROTO_GET_PET:
	{
		sprite_t* who = get_sprite(id);
		if (!who) {
			who = get_across_svr_sprite(id, p->tiles->id);
		}
		int follow_id = ( (who && who->followed) ? who->followed->id : 0 );
		uint32_t type = *(uint32_t*)p->session;
		return response_proto_get_pets(p, map->pets, id, map->pet_cnt, follow_id, type);
	}
	default:
		ERROR_RETURN(("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
	}

	return 0;
}

int use_pet_item_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 8);

	uint32_t petid = *(uint32_t*)buf;
	uint32_t put_off_count = *(uint32_t*)(buf + 4);
	uint32_t put_off_itmid = 0;
	if (put_off_count)
		put_off_itmid = *(uint32_t*)(buf + 8);

	uint32_t put_on_count = *(uint32_t*)p->session;
	uint32_t put_on_itmid = 0;
	if (put_on_count)
		put_on_itmid = *(uint32_t*)(p->session + 4);

	pet_t* pet = get_pet(p->id, petid);
	if (!pet)
		ERROR_RETURN(("user no map or pet not exist\t[%u %u]", p->id, petid), -1);

	switch (p->waitcmd) {
	case PROTO_PET_CLOTHES_USE:
		pet->suppl_info.pet_clothes[0] = put_on_itmid;
		break;
	case PROTO_PET_HONOR_USE:
		pet->suppl_info.pet_honor[0] = put_on_itmid;
		break;
	default:
		ERROR_RETURN(("bad cmd type\t[%u %u]", p->id, petid), -1);
	}

	DEBUG_LOG("PUT ON PET ITEM\t[%u %u %u]", p->id, petid, put_on_itmid);
	int cnt = 0;
	int j = sizeof(protocol_t);
	PKG_UINT32 (msg, p->id, j);
	PKG_UINT32 (msg, petid, j);
	int k = j;
	j += 4;
	if (put_off_count) {
		PKG_UINT32 (msg, put_off_itmid, j);
		PKG_UINT8 (msg, 0, j);
		cnt++;
	}
	if (put_on_count) {
		PKG_UINT32 (msg, put_on_itmid, j);
		PKG_UINT8 (msg, 1, j);
		cnt++;
	}
	PKG_UINT32 (msg, cnt, k);
	init_proto_head(msg, p->waitcmd, j);
	send_to_map(p, msg, j, 1);
	return 0;
}

/* @brief add zhanpao for super lamu when 177 task finish
 */
int super_lamu_get_zhanpao_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN_GE(bodylen, 0);
	CHECK_VALID_ID(p->id);
	if (!test_bit_on(p->tasks, sizeof p->tasks, 2 * 177 + 2)) {
		return send_to_self_error(p, p->waitcmd, -ERR_have_not_fini_task, 1);
	}
	map_t *map = get_map(p->id);
	if (map == NULL) {
		ERROR_RETURN(("cannot get map %u", p->id), -1);
	}
	int loop = 0;
	for (loop = 0; loop < map->pet_cnt; loop++) {
		if(PET_IS_SUPER_LAHM(&map->pets[loop])) {
			db_buy_pet_items(0, map->pets[loop].id, 1200035, 1, 1, p->id);
			break;
		}
	}
	if (loop == map->pet_cnt) {
		return send_to_self_error(p, p->waitcmd, -ERR_no_super_lahm, 1);
	}
	response_proto_uint32(p, p->waitcmd, 1200035, 0);

	return 0;
}
