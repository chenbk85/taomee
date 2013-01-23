#ifndef  GF_CROUTE_FUNC_H
#define  GF_CROUTE_FUNC_H

#include <libtaomee++/proto/proto_base.h>
#include "Citem_change_log.h"


#include "Cfunc_route_base.h"
#include "Ctable.h"
#include "pea_common.hpp"
#include "user.hpp"
#include "pet.hpp"
#include "pet_gift.hpp"
#include "item.hpp"
#include "mail.hpp"
#include "extra_info.hpp"
#include "friends.hpp"
#include "task.hpp"
#include "prize.hpp"


/*
 * =====================================================================================
 *        Class:  Croute_func
 *  Description:  
 * =====================================================================================
 */




class Croute_func : public Cfunc_route_base 
{
    private:


        c_cmd_map m_cmd_map;


    private:

        int do_sync_data(uint32_t user_id, uint16_t cmdid)
        {
            return SUCC;
        }


    public:

        Croute_func (mysql_interface * db); 

        ~Croute_func()
        {

        }

        /*
           virtual int deal(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen )
           {
           PRI_STRU * p_pri_stru;
           uint16_t cmdid=((PROTO_HEADER*)recvbuf)->cmd_id;
           userid_t user_id=((PROTO_HEADER*)recvbuf)->id;			
           return  this->deal_new(recvbuf, rcvlen, sendbuf, sndlen );
           }
           */

        virtual int deal_func(uint16_t cmdid, userid_t user_id, Cmessage * c_in, Cmessage * c_out, deal_func_t p_func)
        {
            //处理在线分裂数据
            if (this->sync_user_data_flag ){//是否打开同步标志
                ret=this->do_sync_data(user_id,cmdid);
                if (ret!=SUCC) return ret;
            }

            c_out->init();

            //调用相关DB处理函数
            ret = (((Croute_func *)this)->*p_func)(user_id, c_in, c_out, NULL);

            //提交数据
            if (mysql_commit(&(this->db->handle))!=DB_SUCC){
                db->show_error_log("db:COMMIT:err");
            }
            return ret;
        }



        int deal(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen )
        {
            uint16_t cmdid = RECVBUF_CMDID;
            userid_t user_id = RECVBUF_USERID;


			DEBUG_LOG("CMDID=0x%04x PRI_IN_LEN:%u",cmdid ,(uint32_t)PRI_IN_LEN );
            const bind_proto_cmd_t * p_cmd_item = m_cmd_map.find_cmd(cmdid);
            if (NULL != p_cmd_item)
            {

                //检查md5值,md5_tags 放在上送报文的result中
                uint32_t md5_value = RECVBUF_RESULT;
                if (md5_value != 0)
                {//==0,表示不用md5_tags检查
                    if (p_cmd_item->md5_tag != md5_value)
                    {
                        return CMD_MD5_TAG_ERR;
                    }
                }

                //私有报文处理
                bool unpack_ret = p_cmd_item->p_in->read_from_buf_ex(RCVBUF_PRIVATE_POS, PRI_IN_LEN);
                if (!unpack_ret)
                {
                    DEBUG_LOG("11111 解析请求包失败, recv len: %u", RECVLEN_FROM_RECVBUF);
                    return PROTO_LEN_ERR;
                }
                //请求报文还原完毕,调用相关函数。

                this->ret=this->deal_func(cmdid, user_id,
                        p_cmd_item->p_in, p_cmd_item->p_out ,
                        p_cmd_item->func);

                if (this->ret == SUCC)
                {//需要构造返回报文

                    static byte_array_t ba_out;
                    ba_out.init_postion();
                    if (!p_cmd_item->p_out->write_to_buf(ba_out))
                    {
                        DEBUG_LOG("p_cmd_item->p_out->write_to_buf :SYS_ERR" );
                        return SYS_ERR;
                    }

                    if (!set_std_return(sendbuf, sndlen, RECVBUF_HEADER, SUCC, ba_out.get_postion(), PROTO_HEADER_SIZE)) 
                    {
                        DEBUG_LOG("set_std_return:SYS_ERR: %u", ba_out.get_postion() );
                        return SYS_ERR; 
                    }
                    memcpy(SNDBUF_PRIVATE_POS, ba_out.get_buf(), ba_out.get_postion()  );
                    return SUCC;
                }
                else
                {//返回错误
                    return this->ret;
                }
            }
            else
            {
                DEBUG_LOG("cmd no define  cmdid[%04X]", cmdid);
                return CMDID_NODEFINE_ERR;
            }
        }

    public:

        pea_user handle_user;


        pea_pet handle_pet;


        pea_pet_gift handle_pet_gift;


		pea_item handle_item;


		pea_mail handle_mail;


        pea_extra_info handle_extra_info;

		pea_friends handle_friends;
		
        task handle_task;
        
        pea_prize handle_prize;


////函数定义
#ifdef PROTO_FUNC_DEF
#undef PROTO_FUNC_DEF
#endif

#define PROTO_FUNC_DEF(proto_name)  \
    int proto_name(DEAL_FUNC_ARG = NULL);


#include "proto/pea_db_func_def.h"

#undef PROTO_FUNC_DEF




}; /* -----  end of class  Croute_func  ----- */
#endif   /* ----- #ifndef CROUTE_FUNC_INC  ----- */

