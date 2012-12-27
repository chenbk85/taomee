#include <libtaomee/dataformatter/bin_str.h>

#include "proto.h"
#include "exclu_things.h"
#include "message.h"
#include "communicator.h"
#include "small_require.h"
#include "central_online.h"
#include "angry_lahm.h"
#include "central_online.h"

static al_all_task_info_t	all_task_info;


int al_get_task_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID( p->id );
	return send_request_to_db( SVR_PROTO_AL_GET_TASK_INFO, p, 0, NULL, p->id );
}

int al_get_task_info_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	uint32_t passed_counter;
	CHECK_BODY_LEN_GE( len, sizeof(uint32_t) );
	int j = 0;
	UNPKG_H_UINT32( buf, passed_counter, j );
	CHECK_BODY_LEN( len, sizeof(uint32_t) + passed_counter * sizeof(al_task_back_info_t) );
	int i;
	int k = sizeof(protocol_t);
	PKG_UINT32( msg, passed_counter, k );
	al_task_back_info_t back_info;
	for( i = 0; i < passed_counter; ++i )
	{
		UNPKG_H_UINT32( buf, back_info.best_score, j );
		UNPKG_H_UINT32( buf, back_info.has_got_award, j );
		UNPKG_H_UINT32( buf, back_info.is_pass, j );
		int level;
		al_task_info_t* task_info = &all_task_info.task_info[i];

		if( back_info.best_score >= task_info->level3 )
		{
			level = 3;
		}
		else if( back_info.best_score >= task_info->level2 )
		{
			level = 2;
		}
		else if( back_info.best_score >= task_info->level1 )
		{
			level = 1;
		}
		else
		{
			level = 0;
		}
		PKG_UINT32( msg, back_info.best_score, k );
		PKG_UINT32( msg, level, k );
		PKG_UINT32( msg, back_info.has_got_award, k );
		PKG_UINT32( msg, back_info.is_pass, k );
	}
	init_proto_head( msg, p->waitcmd, k );
	return send_to_self( p, msg, k, 1 );

}


int al_update_score_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID( p->id );
	uint32_t task_id;
	uint32_t score;
	uint32_t is_pass;
	CHECK_BODY_LEN( len, sizeof(uint32_t) * 3 );
	int j = 0;
	UNPKG_UINT32( body, task_id, j );
	UNPKG_UINT32( body, score, j );
	UNPKG_UINT32( body, is_pass, j );
	if( 0 == task_id || task_id > all_task_info.task_count )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_al_invalid_task_id, 1 );
	}
	uint32_t dbBuf[]={task_id, score, is_pass, all_task_info.task_info[task_id - 1].award_score};
	return send_request_to_db(SVR_PROTO_AL_UPDATE_SCORE, p, sizeof(dbBuf), dbBuf, p->id);
}

int al_update_score_callback(sprite_t * p,uint32_t id,char * buf,int len)
{
	al_task_result_back_t* back_info;
	CHECK_BODY_LEN( len, sizeof(al_task_result_back_t) );
	back_info = (al_task_result_back_t*)buf;
	if( back_info->task_id > back_info->passed_counter + 1 || back_info->task_id > all_task_info.task_count )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_al_invalid_task_id, 1 );
	}

	{
		//统计玩的次数
		uint32_t msgbuff[2]= {p->id, 1};
		uint32_t count_id = 0x0408B34D;
		count_id += back_info->task_id - 1;
		msglog(statistic_logfile, count_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}

	int j = sizeof(protocol_t);
	PKG_UINT32( msg, back_info->task_id, j );
	PKG_UINT32( msg, back_info->bast_score, j );
	PKG_UINT32( msg, back_info->cur_score, j );

	int level;
	al_task_info_t* task_info = &all_task_info.task_info[back_info->task_id - 1];

	if( back_info->cur_score >= task_info->level3 )
	{
		level = 3;
	}
	else if( back_info->cur_score >= task_info->level2 )
	{
		level = 2;
	}
	else if( back_info->cur_score >= task_info->level1 )
	{
		level = 1;
	}
	else
	{
		level = 0;
	}
	PKG_UINT32( msg, level, j );

	if( back_info->is_pass && back_info->cur_score >= task_info->award_score && !back_info->has_got_award )
	{
		PKG_UINT32( msg, task_info->award_count, j );
		int i;
		for( i = 0; i < task_info->award_count; ++i )
		{
			db_exchange_single_item_op(p, 202, task_info->award_info[i].item_id, task_info->award_info[i].item_count, 0);
			PKG_UINT32( msg, task_info->award_info[i].item_id, j );
			PKG_UINT32( msg, task_info->award_info[i].item_count, j );
		}
	}
	else
	{
		PKG_UINT32( msg, 0, j );
	}
	init_proto_head( msg, p->waitcmd, j );
	{
		//统计玩的次数
		uint32_t msgbuff[2]= {p->id, 1};
		msglog(statistic_logfile, 0x0408B34B, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}
	return send_to_self( p, msg, j, 1 );

}

int al_load_conf_file()
{
	char file[]="./conf/angry_lahm_task_info.xml";
	xmlDocPtr doc;
	xmlNodePtr cur;
	int err = -1;

	memset(&all_task_info, 0, sizeof(all_task_info));

	doc = xmlParseFile (file);
	if (!doc)
	{
		ERROR_RETURN (("load angry lahm task info failed"), -1);
	}

	cur = xmlDocGetRootElement(doc);
	if (!cur)
	{
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}
	xmlNodePtr chl_task = cur->children;
	uint32_t* task_count = &all_task_info.task_count;
	*task_count = 0;
	while( chl_task )
	{
		if( 0 == xmlStrcmp( chl_task->name, (const xmlChar *)"Task" ) )
		{
			if( *task_count >= AL_MAX_TASK_SIZE )
			{
				ERROR_RETURN (("al_load_conf_file Task Count Out Of Max Range"), -1);
			}
			al_task_info_t* task_info = &(all_task_info.task_info[*task_count]);

			DECODE_XML_PROP_UINT32(task_info->level1, chl_task, "Level1");
			DECODE_XML_PROP_UINT32(task_info->level2, chl_task, "Level2");
			DECODE_XML_PROP_UINT32(task_info->level3, chl_task, "Level3");
			DECODE_XML_PROP_UINT32(task_info->award_score, chl_task, "AwardScore");

			xmlNodePtr chl_award = chl_task->children;
			task_info->award_count = 0;
			while( chl_award )
			{
				if( 0 == xmlStrcmp( chl_award->name, (const xmlChar *)"Award" ) )
				{
					if( task_info->award_count >= AL_MAX_AWARD_KIND )
					{
						ERROR_RETURN ( ( "al_load_conf_file Award Count Out Of Max Range Task_ID is[%d]", *task_count ), -1 );
					}
					al_award_info_t* award_info = &task_info->award_info[task_info->award_count];
					DECODE_XML_PROP_UINT32(award_info->item_id, chl_award, "ItemID");
					DECODE_XML_PROP_UINT32(award_info->item_count, chl_award, "ItemCount");
					task_info->award_count++;
				}
				chl_award = chl_award->next;
			}
			(*task_count)++;
		}
		chl_task = chl_task->next;
	}
	err = 0;
	exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load angry lahm task file %s", file);
}


