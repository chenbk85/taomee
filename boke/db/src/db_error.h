/*
 * =====================================================================================
 * 
 *       Filename:  common.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月02日 18时33分18秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  db_error_INC
#define  db_error_INC

#include <db_error_base.h>

//物品已经存在
#define  ITEM_IS_EXISTED_ERR  5120
//物品不存在
#define  ITEM_NOFIND_ERR  5121

//物品不足
#define  ITEM_NOENOUGH_ERR  5122

//物品太多
#define  ITEM_MAX_ERR  5123


//任务已经存在
#define  TASK_IS_EXISTED_ERR  5131
//任务不存在
#define  TASK_NOFIND_ERR  5132

//mapid已经存在
#define  MAPID_IS_EXISTED_ERR  5135
//maid不存在
#define  MAPID_NOFIND_ERR  5136

//加入足迹失败
#define  USER_LOG_ADD_ERR 5137

#define  GAME_NOFIND_ERR 5138
#define GAME_IS_EXISTED_ERR 5139



#endif   /* ----- #ifndef COMMON_INC  ----- */

