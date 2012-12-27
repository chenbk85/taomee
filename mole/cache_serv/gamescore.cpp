

/**
 * ============================================================================
 * @file    gamecore.cpp
 *
 * @brief   实现提交分数和获取分数的函数。
 *
 * ompiler gcc version 4.1.2
 *
 * platfomr Debian 4.1.1-12
 *
 * copyright TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 * ============================================================================
 */



#include <cstring>
#include <cstdlib>

extern "C"{
#include <arpa/inet.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include <libtaomee/interprocess/filelock.h>
#include <libtaomee/crypt/qdes.h>
#include <serverbench/benchapi.h>
}

#include "./pkg/Cclientproto.h"

#include "globalvar.h"
#include "gamescore.h"
#include "packet.h"
#include "base.h"
#include "cmdprocess.h"
#include "user_time.h"



/**
 * @brief  读取文件，初始化游戏id和游戏flag
 * @parm   gmid_array 存储游戏id和游戏flag的数组
 * @return 0, 成功；-1, 失败
 */
int get_game_id(gmid_ptr_t gmid_array, char *gmid_file)
{
	FILE *fp;
	char buf[50];
	uint32_t i = 0;
	
	if ((fp = fopen(gmid_file, "r")) == NULL) {
		return -1;
	}

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		/*make sure the begin and end don't contian ' ' '\t' '\n'*/
		trim(buf);
		/*the first word is not '#'*/
		if (strlen(buf) == 0 || buf[0] == '#') {
			continue;
		}
		sscanf(buf, "%u %u", &(gmid_array[i].game_id), &(gmid_array[i].flag));

		if (gmid_array[i].game_id != i + 1) {
			BOOT_LOG(-1, "game_id_file sort is wrong, %d", gmid_array[i].game_id);
		}
		/*if bigger than the max game number, quit*/
		if (++i >= max_game) {
			break;
		}
	}
	return 0;
}



/**
 * @brief   初始化游戏的分数
 * @param   cpo 包含对数据库的操作 
 * @param   gmid_array  存放游戏id和flag的数组
 * @return  0, 成功；-1, 失败
 */

int init_game_score(Cclientproto *cpo, gmid_ptr_t gmid_array)
{
	uint32_t i;
	int ret = -1;
	stru_count playernum;
	score_list_item *p_list = NULL;

	for (i = 0; i < max_game; i++) {
		/*get the pointer which point to game score according the game id*/
		if ((ret = cpo->game_get_score_list(gmid_array[i].game_id, &playernum, &p_list))) {
			continue;
		}

		gmsc_adr[i].flag = gmid_array[i].flag;
		gmsc_adr[i].game_id = gmid_array[i].game_id;		
		gmsc_adr[i].playernum = playernum.count;
		/*copy game score*/ 
		memcpy(gmsc_adr[i].game_score, p_list, (playernum.count) * sizeof(gmsc_t));
		
		/*free the memory*/
		free(p_list);
	}

	if (ret) {
		return -1;
	}

	return 0;
}




/**
 * @brief   对提交分数命令的响应函数
 * @param   recvbuf 接收到的字符串指针
 * @param   rcvlen  接收的字符串的长度
 * @param   sendbuf 发送的字符串的指针的指针
 * @param   sndlen  保存发送的字符串长度的指针
 * @return  0，执行成功;-1，执行失败
 */

int gmsc_submit(char *recvbuf, int rcvlen, char *sendbuf, int *sndlen)
{
	int ret;
	uint32_t user_id;
	uint32_t game_id;
	userinfo_get_nick_out user_nick;
	proto_head_ptr_t proto_data;
	gmsc_submit_req_ptr_t sub_data;
	stru_gmsc_key_t stu_key;
	gmsc_t gmsc;
	
	/*get packet header*/
	proto_data = (proto_head_ptr_t)recvbuf; 
	/*get submit request structure*/
	sub_data = (gmsc_submit_req_ptr_t)(recvbuf + PROTO_HEAD_SIZE);
	user_id = ntohl(proto_data->user_id);


	game_id = ntohl(sub_data->game_id);
	gmsc.user_id = user_id;
	gmsc.score = ntohl(sub_data->score);

	/*check the received string*/
	if (ntohl(*(uint32_t*)recvbuf) != (PROTO_HEAD_SIZE + GMSC_SUBMIT_REQ_BODY_SIZE)) {
		ERROR_LOG("packet length, length %u gameid %u, userid %u, ip %x",
				ntohl(*(uint32_t*)recvbuf), game_id, user_id, client_ip);
		return REG_SIZE_ERR;
	}

	/*decode the session*/
	des_decrypt_n(GS_KEY, sub_data->session, (char *)(&stu_key), sizeof(stru_gmsc_key_t)/8);

	/*if (stu_key.ip != client_ip) {
		ERROR_LOG("invalid ip: %x  %x", stu_key.ip, client_ip);
		return GS_CLIENT_IP_ERR;
	}*/

	if (stu_key.user_id != ntohl(proto_data->user_id)) {
		ERROR_LOG("use_id is invalid, user id %u, session id %u, game_id %u, ip %x",
				 ntohl(proto_data->user_id), stu_key.user_id, game_id, client_ip);
		return USER_ID_ERR;
	}

	if (GAME_ID(stu_key.game_id) != game_id) {
		ERROR_LOG("gameid gameid %u, session game_id %u, userid %u, ip %u",
				  game_id, stu_key.game_id, user_id, client_ip);
		return GAME_ID_ERR;
	}

	if (stu_key.score != gmsc.score) {
		ERROR_LOG("invalid score, user_id  %u, game_id %u, session score %u, game score %u",
				  user_id, game_id, stu_key.score, gmsc.score);
		return GAME_SCORE_ERR;
	}

	if (ntohl(sub_data->score) > GMID_MAX_SCOR) {
		ERROR_LOG("score too large, score %u, gameid %u, userid %u, ip %x",
				ntohl(sub_data->score), game_id, user_id, client_ip);
		return GAME_SCORE_ERR;
	}

	/*allocate memory for the sendbuf and initialize variable*/
	if (set_sndbuf_head(sendbuf, sndlen, (proto_head_ptr_t)recvbuf, 
				        GMSC_SUBMIT_RESP_BODY_SIZE, SUCCESS) != SUCCESS) {
		ERROR_LOG("gmsc_submit : malloc memory for sendbuf is wrong!");
		return REG_SYS_ERR;
	}

	/*get nickname from DB according user id*/
	if ((ret = cpo->f_USERINFO_GET_NICK(user_id, &user_nick)) != SUCCESS) {
		ERROR_LOG("get user nick name is wrong, userid %u, ret %d, gameid %u, ip %x",
				user_id, ret, game_id, client_ip);
		return GET_USER_NICK_ERR;
	}	

	memcpy(gmsc.nick, user_nick.nick, NICK_LEN);
	gmsc.nick[NICK_LEN-1] = '\0';


	if (game_id < 1 || game_id > max_game) {
		ERROR_LOG("the game is not exist, userid %u, gameid %u", user_id, game_id);
		return USER_ID_ERR;
	}

	if ((ret = insert_game_score(game_id, &gmsc)) != SUCCESS) {
		return 0;
	}


	/*update the DB*/
	ret = cpo->game_update_game_score(game_id, user_id, gmsc.score, gmsc.nick);
	if (ret != 0) {
		if (ret == GAME_SCORE_NONEED_SET_ERR) {
			INFO_LOG("insert score : %u, game_id %u, user_id %u", gmsc.score, game_id, user_id);
			score_list_item *temp_list = NULL;
			stru_count temp_num;
			cpo->game_get_score_list(game_id, &temp_num, &temp_list);
			for (uint32_t k = 0; k < temp_num.count; k++) {
				INFO_LOG("the user %u, the DB %u", (gmsc_adr + game_id - 1)->game_score[k].score, 
						 temp_list[k].score);
			}

		} else if (ret == GAME_SCORE_NOFIND_ERR) {
			INFO_LOG("the game score is not found in db");
		} else {
			INFO_LOG("submit the game score error, %d", ret);
		}	
	}


	return 0;
}



/**
 * @brief  得到游戏分排行榜的分数 
 * @param  recvbuf 接收到的字符串的指针
 * @param  rcvlen  接收的字符串长度
 * @param  sendbuf 发送的字符串指针的指针
 * @param  sndlen  保存发送的字符串长度
 * @return -1, 失败;SUCCESS，成功
 */
int gmsc_obtain(char *recvbuf, int rcvlen, char *sendbuf, int *sndlen)
{
	uint32_t i;
	uint32_t game_id;
	uint32_t num;
	gmsc_obtain_req_ptr_t obtain_req;
	gs_mem_t gmsc_list;
	gmsc_obtain_resp_t gmsc_list_resp;

	obtain_req = (gmsc_obtain_req_ptr_t)(recvbuf + PROTO_HEAD_SIZE);
	if (ntohl(*(uint32_t *)recvbuf) != (PROTO_HEAD_SIZE + GMSC_OBTAIN_REQ_SIZE)) {
		ERROR_LOG("the length is wrong!");
		return REG_SIZE_ERR;
	}

	game_id = ntohl(obtain_req->game_id);

	if (game_id < 0 || game_id > max_game) {
		ERROR_LOG("game is not exist");
		return GAME_NOT_EXIST;
	}
	
	/*get score information, store in gmsc_list*/
	get_game_score_list(game_id, &gmsc_list);
	/*allocate memory for the response*/
	if (set_sndbuf_head(sendbuf, sndlen, (proto_head_ptr_t)recvbuf,
						GMSC_OBTAIN_RESP_SIZE, SUCCESS) !=SUCCESS) {
		ERROR_LOG(" allocate memory is wrong!");
		return CSYS_ERR;
	}

	memcpy(&gmsc_list_resp, (char *)&gmsc_list + 4, GMSC_OBTAIN_RESP_SIZE);
	num = gmsc_list_resp.playernum;

	gmsc_list_resp.game_id = htonl(gmsc_list_resp.game_id);
	gmsc_list_resp.playernum = htonl(gmsc_list_resp.playernum);

	for (i = 0; i < num; i++) {
		gmsc_list_resp.game_score[i].user_id = htonl(gmsc_list_resp.game_score[i].user_id);
		gmsc_list_resp.game_score[i].score = htonl(gmsc_list_resp.game_score[i].score);
	}
	
	/*copy response information to sendbuf memory*/
	memcpy((sendbuf + PROTO_HEAD_SIZE),	&gmsc_list_resp, GMSC_OBTAIN_RESP_SIZE);

	return 0;
}



/**
 * @brief   根据游戏ID号得到游戏的排行榜的分数
 * @param   game_id 游戏的ID号
 * @param   gmscptr 排行榜的分数存储在此指针变量指向的空间
 * @return  -1，失败；0，成功
 */
int get_game_score_list(uint32_t game_id, gs_mem_ptr_t gmscptr)
{
	uint32_t i;
	uint32_t index;
	gs_mem_ptr_t p;

	index = game_id - 1;
	if (index < 0 || index > max_game) {
		ERROR_LOG("fail to get the game index, %u", index);
		return GAME_NOT_EXIST;
	}

	p = gmsc_adr + index;

	if (filelock_rlock(gm_score_fd, index * sizeof(gs_mem_t), SEEK_SET, sizeof(gs_mem_t)) == -1) {
		return -1;
	}
	memcpy(gmscptr, p, GMMEM_LEN);
	if (filelock_unlock(gm_score_fd, index * sizeof(gs_mem_t), SEEK_SET, sizeof(gs_mem_t)) == -1) {
		return -1;
	}

	if (gmid_array[index].flag) {
		for (i = 0; i < GMSC_FIRST_NUM; ++i) {
			(gmscptr->game_score[i]).score = GMID_MAX_SCOR - (gmscptr->game_score[i]).score;
		}
	}
	return 0;
}



/** 
 * @brief  把游戏分数插入到排行榜中
 * @param  game_id   游戏的id号
 * @param  gmsc_ptr  存放用户信息和分数结构体指针
 * @return 0，插入成功;-1，失败
 */
int insert_game_score(uint32_t game_id, gmsc_ptr_t gmsc_ptr)
{
	int index, pos;
	int opy_pos;
	int i;
	gs_mem_ptr_t p;
	gmsc_ptr_t gmidadr;

	index = game_id - 1;

	/*get game score address*/
	p = gmsc_adr + index;
	gmidadr = p->game_score;

	/*judge the sort, 0--high, 1--low*/
	if (gmid_array[index].flag) {
		gmsc_ptr->score = GMID_MAX_SCOR - gmsc_ptr->score; 
	}


	if (filelock_wlock(gm_score_fd, index * sizeof(gs_mem_t), SEEK_SET, sizeof(gs_mem_t)) == -1) {
		ERROR_LOG("fail to lock write file");
		return -1;
	}

	/*if the user exist, insert the score into list*/
	if (is_user_exist(game_id, gmsc_ptr->user_id, gmsc_ptr->score, &opy_pos)) {
		/*if the new score less then old score, do nothing*/
		if (opy_pos <= -1) {
			filelock_unlock(gm_score_fd, index * sizeof(gs_mem_t), SEEK_SET, sizeof(gs_mem_t));
			return -1;
		}

		/*if bigger than old score, insert it*/
		for (i = opy_pos - 1; i >= 0; --i) {
			if (gmsc_ptr->score > gmidadr[i].score) {
				memcpy((char *)&(gmidadr[i+1]), (char *)&(gmidadr[i]), GMSC_LEN);			
			} else {
				break;						
			} 				
		}
		memcpy((char *)&(gmidadr[i + 1]), (char *)gmsc_ptr, GMSC_LEN);
		filelock_unlock(gm_score_fd, index*sizeof(gs_mem_t), SEEK_SET, sizeof(gs_mem_t));
		return 0;
	}
	/*if the user not exist in the list, do here*/
	if (get_insert_pos(game_id, gmsc_ptr->score, &pos) != SUCCESS) {
		filelock_unlock(gm_score_fd, index*sizeof(gs_mem_t), SEEK_SET, sizeof(gs_mem_t));
		return -1;
	}

	/*insert the score*/
	i = ((int)((p->playernum < GMSC_FIRST_NUM) ? (p->playernum++) : (p->playernum - 1))); 
	for (; i >= pos; i--) {
		memcpy((char *)&gmidadr[i], (char *)&gmidadr[i - 1], GMSC_LEN);
	}
	memcpy((char *)&gmidadr[i], (char *)gmsc_ptr, GMSC_LEN);

	filelock_unlock(gm_score_fd, index*sizeof(gs_mem_t), SEEK_SET, sizeof(gs_mem_t));

	return 0;			
}



/**
 * @brief  得到用户应该在的位置
 * @param  game_id, 游戏的ID号
 * @param  score, 用户游戏的分数
 * @param  pos, 返回位置存储在此变量中
 * @return SUCCESS，得到位置，可进行排序
 * @return -1，没有找到位置，无需排序
 */

int get_insert_pos(uint32_t game_id, uint32_t score, int *pos)
{
	int j;
	int index;
	gmsc_ptr_t gmsc_p;

	index = game_id - 1;
	/*get the score*/
	gmsc_p = (gmsc_adr + index)->game_score;
	/*judge the score can insert into this list or not*/
	for (j = 0; j < GMSC_FIRST_NUM && gmsc_p[j].score >= score; j++) {
		;
	}

	if ((*pos = ((j >= GMSC_FIRST_NUM) ? 0 : ++j)) == 0) {
		return -1;
	}

	return 0;
}
