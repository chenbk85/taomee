#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "dll.h"
#include "config.h"
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include "shmq.h"
#include "daemon.h"
#include "service.h"
#include "proto.h"

#include "sprite.h"

#define HASH_SLOT_NUM	100
static sprite_t     all_sprites[MAXFDS];
static list_head_t  idslots[HASH_SLOT_NUM];

void traverse_sprites (int (*action)(sprite_t *))
{
	int i;
	for (i = 0; i < fds.fdmax + 1; i++) {
		if (all_sprites[i].id != 0)
			action (&all_sprites[i]);
	}
}

void init_sprites()
{
	int i;

	memset (all_sprites, 0, sizeof (all_sprites));
	for (i = 0; i < HASH_SLOT_NUM; i++)
		INIT_LIST_HEAD(&idslots[i]);
}

void fini_sprites ()
{
}

int sprite_fd (const sprite_t *p)
{
	int fd = p - all_sprites;

	if (fd < 0 || fd > fds.fdmax)
		ERROR_RETURN (("error sprite fd=%d", fd), -1);

	return fd;
}

sprite_t *get_sprite (uint32_t id)
{
	sprite_t *p;

	list_for_each_entry(p, &idslots[id % HASH_SLOT_NUM], hash_list) {
		if (p->id == id)
			return p;
	}

	return NULL;
}

sprite_t* get_sprite_from_gamegrp(uint32_t id, const struct game_group* grp)
{
	uint8_t i = 0;
	for (; i != grp->count; ++i) {
		if (grp->players[i]->id == id) {
			return grp->players[i];
		}
	}
	return 0;
}

sprite_t *get_sprite_by_fd(int fd)
{
	if (fd >= MAXFDS || fd < 0)
		return NULL;
	if (all_sprites[fd].id == 0)
		return NULL;
	return &all_sprites[fd];
}

void free_sprite (sprite_t *p)
{
	memset (p, 0, sizeof (sprite_t));
}

sprite_t *alloc_sprite (int fd)
{
	assert (fd <= fds.fdmax && all_sprites[fd].id == 0);
	return &all_sprites[fd];
}

int add_sprite(const sprite_t* v)
{
	if ( (v->id == 0) || IS_GUEST_ID(v->id) ) {
		ERROR_RETURN( ("Guest not allowed!"), -1 );
	}

	sprite_t* p = alloc_sprite(v->priv);
	*p = *v;
	p->priv = 0;
	INIT_LIST_HEAD(&p->timer_list);

	list_add_tail(&p->hash_list, &idslots[p->id % HASH_SLOT_NUM]);
	return 0;
}

int del_sprite(sprite_t *p, int update)
{
	list_del(&p->hash_list);
	free_sprite(p);

	return 0;
}

int del_sprite_by_fd (int fd, int update)
{
	sprite_t *p;

	if (!(p = get_sprite_by_fd(fd)))
		return -1;

	return del_sprite(p, update);
}

void del_sprite_conn(const sprite_t *p)
{
//	DEBUG_LOG("DELETE PLAYER %d", p->id);
	int fd = sprite_fd(p);
	if (fd >= 0) {
		if ( !IS_SPRITE_OFFLINE(p) ) {
			shm_ctl_block_push(&sendq, fd, FIN_BLOCK);
		} else {
			handle_close(fd, 1);
		}
	}
}

inline void ASC2HEX_3(char * dst, char * src,int len)
{
	int hex;
	int i;
	int di;
	for(i=0;i<len;i++){
		hex=((unsigned char)src[i])>>4;
		di=i*3;
		dst[di]=hex<10?'0'+hex:'A'-10 +hex ;
		hex=((unsigned char)src[i])&0x0F;
		dst[di+1]=hex<10?'0'+hex:'A'-10 +hex ;
		dst[di+2]=' ';
	}
	dst[len*3]=0;
}



int send_to_self(sprite_t* p, void* buffer, int len, int completed)
{
//	char buf_o[13000];
//	ASC2HEX_3(buf_o,buffer,len );
//	DEBUG_LOG("send[%s]",buf_o);

	int fd;
	protocol_t *proto;

	struct shm_block mb;


	fd = sprite_fd(p);
	if (fd == -1) return -1;

	proto = (protocol_t *) buffer;
	proto->id = htonl (p->id);

	mb.id = fds.cn[fd].id;
	mb.fd = fd;
	mb.type = DATA_BLOCK;
	mb.length = len + sizeof (shm_block_t);

	if (shmq_push(&sendq, &mb, (uint8_t*)buffer) == -1)
		ERROR_RETURN (("message is lost: id=%u", p->id), -1);

	if (completed)
		p->waitcmd = 0;
	return 0;
}

int send_to_self_error (sprite_t *p, int cmd, int err, int completed)
{
	protocol_t pkg;
	init_proto_head(&pkg, cmd, sizeof pkg);
	pkg.ret = htonl(err);

	return send_to_self(p, (uint8_t *)&pkg, sizeof pkg, completed);
}

int response_test_alive(int fd)
{
	static uint8_t alive_buffer[4096];
	protocol_t *proto;

	init_proto_head(alive_buffer, proto_test_alive, sizeof (protocol_t));
	struct shm_block mb;

	proto = (protocol_t *) alive_buffer;
	proto->id = 0;

	mb.id = fds.cn[fd].id;
	mb.fd = fd;
	mb.type = DATA_BLOCK;
	int len = sizeof(protocol_t);
	mb.length = len + sizeof (shm_block_t);

	if (shmq_push(&sendq, &mb, (uint8_t*)alive_buffer) == -1)
		ERROR_RETURN (("test alive message is lost"), -1);
	return 0;
}
