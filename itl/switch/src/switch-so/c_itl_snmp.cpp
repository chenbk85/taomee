/** 
 * ========================================================================
 * @file c_itl_snmp.cpp
 * @brief 
 * @author tonyliu
 * @version 1.0.0
 * @date 2012-08-09
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/utilities.h>
#include <net-snmp/net-snmp-includes.h>

extern "C" {
#include <libtaomee/log.h>
}
#include "c_itl_snmp.h"

using namespace std;

//==========================================宏定义==========================================
#define ITL_FREE(resource, free_func) \
    do {\
        if (NULL != resource) {\
            free_func(resource);\
            resource = NULL;\
        }\
    } while (false)


#define ITL_SNMP_ASSIGN_STR(snmp_value, buf, buf_len, var_list, func, func_name) \
    do {\
        snmp_value.val_type = VAL_TYPE_STRING;\
        snmp_value.val.str_val = NULL;\
        if (buf_len <= 0) {\
            ERROR_LOG("No enough buffer space to store string info, assign it to NULL.");\
            break;\
        }\
        \
        int real_len = func(buf, buf_len, var_list, NULL, NULL, NULL);\
        if (real_len < 0) {\
            ERROR_LOG("%s failed", func_name);\
        } else {\
            snmp_value.val.str_val = buf;\
            buf[real_len] = '\0';\
            /*留一个位置给'\0'*/\
            real_len++;\
            buf_len -= real_len;\
            buf += real_len;\
        }\
    } while (false)

#define ITL_MAX_TIMEOUT_COUNT 10
#define ITL_MAX_TIMEOUT_INTERVAL 86400
#define ITL_SNMP_CHECK_TIMEOUT \
    do {\
        time_t now = time(NULL);\
        int time_interval = now - m_last_timeout;\
        if (m_timeout_count >= ITL_MAX_TIMEOUT_COUNT) {\
            if (time_interval < ITL_MAX_TIMEOUT_INTERVAL) {\
                return -1;\
            }\
            m_timeout_count = 0;\
            ERROR_LOG(">>>reset[%s] m_timeout_count to zero", m_snmp_sess.peername);\
        }\
    } while (false)


//==========================================================================================




c_itl_snmp::c_itl_snmp(): m_inited(false), m_timeout_count(0), m_last_timeout(0), m_p_sess_handle(NULL)
{
}

c_itl_snmp::~c_itl_snmp()
{
}

int c_itl_snmp::init(const char *p_ip, const char *p_community, int timeout, int retry_times)
{
    assert(NULL != p_ip && NULL != p_community);

    //init the session
    snmp_sess_init(&m_snmp_sess);
    m_snmp_sess.peername = (char *)p_ip;
    m_snmp_sess.version = SNMP_VERSION_2c;
    m_snmp_sess.community = (unsigned char *)p_community;
    m_snmp_sess.community_len = strlen(p_community);
    m_snmp_sess.retries = (retry_times > 0) ? retry_times : 0;
    m_snmp_sess.timeout = (timeout >= 0) ? timeout * 1000000L : ITL_SNMP_DEFAULT_TIMEOUT * 1000000L;//缺省10秒

    m_p_sess_handle = snmp_sess_open(&m_snmp_sess);
    if(NULL == m_p_sess_handle)
    {
        ERROR_LOG("snmp_sess_open(%s) failed.", p_ip);
        return -1;
    }
    m_buff[sizeof(m_buff) -1] = '\0';
    m_inited = true;

    return 0;
}

int c_itl_snmp::uninit()
{
    if (!m_inited)
    {
        return -1;
    }

    ITL_FREE(m_p_sess_handle, snmp_sess_close);
    m_inited = false;

    return 0;
}


int c_itl_snmp::simple_snmp_get(
        const char **oids,
        unsigned int oid_num,
        vector<snmp_value_t> &value_vector)
{
    if (!m_inited)
    {
        ERROR_LOG("c_itl_snmp has not been inited!");
        return -1;
    }
    //ITL_SNMP_CHECK_TIMEOUT;

    assert(NULL != oids && oid_num > 0);

    if(oid_num > SNMP_MAX_CMDLINE_OIDS)
    {
        ERROR_LOG("Too many[%u] object identifiers, only %d allowed in one request.",
                oid_num, SNMP_MAX_CMDLINE_OIDS);
        return -1;
    }

    netsnmp_pdu *pdu = snmp_pdu_create(SNMP_MSG_GET);
    if (NULL == pdu)
    {
        ERROR_LOG("snmp_pdu_create(SNMP_MSG_GET) for ip[%s] failed: %u",
                m_snmp_sess.peername, snmp_errno);
        return -1;
    }

    unsigned int index = 0;
    unsigned int name_num = 0;
    char *names[SNMP_MAX_CMDLINE_OIDS];
    for(index = 0; index < oid_num; index++)
    {
        names[name_num++] = (char *)oids[index];
    }

    int fail_num = 0;
    size_t name_len = 0;
    oid name[MAX_OID_LEN];
    for(index = 0; index < name_num; index++)
    {
        name_len = MAX_OID_LEN;
        if(!snmp_parse_oid(names[index], name, &name_len))
        {
            ERROR_LOG("snmp_parse_oid(%s) failed.", names[index]);
            fail_num++;
        }
        else
        {
            snmp_add_null_var(pdu, name, name_len);
        }
    }
    if(fail_num > 0)
    {
        ERROR_LOG("%d wrong oids in oid list.", fail_num);
        return -1;
    }

    //发送请求
    //snmp_synch_response_cb;
    int ret_val = 0;
    int remain_len = sizeof(m_buff) - 1;
    char *write_pos = m_buff;
    netsnmp_variable_list *vars = NULL;
    netsnmp_pdu *response = NULL;
    int status = snmp_sess_synch_response(m_p_sess_handle, pdu, &response);
    do
    {
        if(STAT_SUCCESS != status)
        {
            if(STAT_TIMEOUT == status)
            {
                ++m_timeout_count;
                m_last_timeout = time(NULL);
                ERROR_LOG("Timeout[%uth]: no response from [%s].", m_timeout_count, m_snmp_sess.peername);
            }
            else
            {        
                ERROR_LOG("Critical: unknown error from session[%s].", m_snmp_sess.peername);
            }
            ret_val = -1;
            break;
        }
        m_timeout_count = 0;

        if(SNMP_ERR_NOERROR != response->errstat)
        {
            ERROR_LOG("Packet ERROR: %s", snmp_errstring(response->errstat));
            if(0 != response->errindex)
            {
                ERROR_LOG("Failed object:");
                for(index = 1, vars = response->variables;
                        vars && index != response->errindex;
                        vars = vars->next_variable, index++)
                {
                    char oid_buf[MAX_OID_LEN] = {0};
                    snprint_objid(oid_buf, sizeof(oid_buf), vars->name, vars->name_length);
                    ERROR_LOG("    |__%s", oid_buf);
                }
            }
            ret_val = -1;
            break;
        } 

        value_vector.clear();
        snmp_value_t snmp_value;
        for(vars = response->variables; vars; vars = vars->next_variable)
        {
            bool valid_type = true;
            switch(vars->type)
            {
                case ASN_INTEGER:
                    {
                        snmp_value.val_type = VAL_TYPE_INT32;
                        snmp_value.val.int32_val = (int)(*vars->val.integer);
                        break;
                    }
                case ASN_GAUGE:
                case ASN_COUNTER:
                case ASN_TIMETICKS:
                case ASN_UINTEGER:
                    {
                        snmp_value.val_type = VAL_TYPE_UINT32;
                        snmp_value.val.uint32_val = (unsigned int)(*vars->val.integer & 0xffffffff);
                        break;
                    }
                case  ASN_COUNTER64:
                    {
                        snmp_value.val_type = VAL_TYPE_UINT64;
                        snmp_value.val.uint64_val = vars->val.counter64->high;
                        snmp_value.val.uint64_val <<= 32;
                        snmp_value.val.uint64_val += vars->val.counter64->low;
                        break;
                    }
                case ASN_IPADDRESS:
                    ITL_SNMP_ASSIGN_STR(snmp_value, write_pos, remain_len, vars,
                            snprint_ipaddress, "snprint_ipaddress");
                    break;
                case  ASN_BIT_STR:
                    ITL_SNMP_ASSIGN_STR(snmp_value, write_pos, remain_len, vars,
                            snprint_bitstring, "snprint_bitstring");
                    break;
                case  ASN_NULL:
                    ITL_SNMP_ASSIGN_STR(snmp_value, write_pos, remain_len, vars,
                            snprint_null, "snprint_null");
                    break;
                case  ASN_OBJECT_ID:
                    ITL_SNMP_ASSIGN_STR(snmp_value, write_pos, remain_len, vars,
                            snprint_object_identifier, "snprint_object_identifier");
                    break;
                case  ASN_OCTET_STR:
                    ITL_SNMP_ASSIGN_STR(snmp_value, write_pos, remain_len, vars,
                            snprint_octet_string, "snprint_octet_string");
                    break;
                case  ASN_OPAQUE:
                    ITL_SNMP_ASSIGN_STR(snmp_value, write_pos, remain_len, vars,
                            snprint_opaque, "snprint_opaque");
                    break;
                default :
                    ERROR_LOG("Bad value type[0X%X], not supported.", vars->type);
                    valid_type = false;
                    break;
            }
            if (valid_type)
            {
                value_vector.push_back(snmp_value);
            }
        }
    } while (false);

    ITL_FREE(response, snmp_free_pdu);

    return ret_val;
}

/** 
 * @brief 获取root_oid_name下面的所有字oid信息
 * 
 * @param root_oid_name: 跟oid名称
 * @param value_vector: 待返回值的vector
 * 
 * @return 0:succ, -1:failed
 */
int c_itl_snmp::simple_snmp_walk(const char *root_oid_name, vector<snmp_value_t> &value_vector)
{
    if (!m_inited)
    {
        ERROR_LOG("c_itl_snmp has not been inited!");
        return -1;
    }
    //ITL_SNMP_CHECK_TIMEOUT;
    assert(NULL != root_oid_name);

    int ret_val = 0;

    oid root_oid[MAX_OID_LEN] = {0};
    size_t root_len = sizeof(root_oid) / sizeof(oid);
    if(NULL == snmp_parse_oid(root_oid_name, root_oid, &root_len))
    {
        ERROR_LOG("snmp_parse_oid() for root_oid[%s] failed.", root_oid_name);
        return -1;
    }
    size_t name_len = root_len;
    oid name[MAX_OID_LEN] = {0};
    memcpy(name, root_oid, root_len*sizeof(oid));

    int status = 0;
    char *write_pos = m_buff;
    int remain_len = sizeof(m_buff) - 1;//给结尾'\0'留一个空间
    netsnmp_pdu *pdu = NULL;
    netsnmp_pdu *response = NULL;
    netsnmp_variable_list *vars = NULL;
    snmp_value_t snmp_value;
    value_vector.clear();
    bool goon = true;
    do
    {
        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
        snmp_add_null_var(pdu, name, name_len);

        //发送请求
        status = snmp_sess_synch_response(m_p_sess_handle, pdu, &response);
        if(STAT_SUCCESS != status)
        {
            if(STAT_TIMEOUT == status)
            {
                ++m_timeout_count;
                m_last_timeout = time(NULL);
                ERROR_LOG("Timeout[%uth]: no response from [%s].", m_timeout_count, m_snmp_sess.peername);
                ret_val = -1;
            }
            else
            { 
                ERROR_LOG("Critical:Unknown error from session [%s].", m_snmp_sess.peername);
                ret_val = -1;
            }
            break;
        }
        if(SNMP_ERR_NOERROR != response->errstat)
        {
            if(SNMP_ERR_NOSUCHNAME == response->errstat)
            {
                ERROR_LOG("End of MIB[SNMP_ERR_NOSUCHNAME].");
            }
            else
            {
                ERROR_LOG("Packet ERROR: %s.", snmp_errstring(response->errstat));
                if(0 != response->errindex)
                {
                    ERROR_LOG("Failed object:");
                    int index = 1;
                    for(vars = response->variables;
                            vars && index != response->errindex;
                            vars = vars->next_variable, index++)
                    {
                        char oid_buf[MAX_OID_LEN] = {0};
                        snprint_objid(oid_buf, sizeof(oid_buf), vars->name, vars->name_length);
                        ERROR_LOG("    |__%s", oid_buf);
                    }
                }
                ret_val = -1;
            }
            break;
        }

        for(vars = response->variables; vars; vars = vars->next_variable)
        {
            //不是root_oid的子树
            if(vars->name_length < root_len || 0 != memcmp(root_oid, vars->name, root_len * sizeof(oid)))
            {
                goon = false;
                char oid_buf[MAX_OID_LEN] = {0};
                snprint_objid(oid_buf, sizeof(oid_buf), vars->name, vars->name_length);
                ERROR_LOG("response oid[%s] is not subtree of root oid[%s]", oid_buf, root_oid_name);
                continue;
            }

            bool valid_type = true;
            switch(vars->type)
            {
                case  ASN_INTEGER:
                    {
                        snmp_value.val_type = VAL_TYPE_INT32;
                        snmp_value.val.int32_val = *vars->val.integer;
                        break;
                    }
                case  ASN_GAUGE:
                case  ASN_COUNTER:
                case  ASN_TIMETICKS:
                case  ASN_UINTEGER:
                    {
                        snmp_value.val_type = VAL_TYPE_UINT32;
                        snmp_value.val.uint32_val = (unsigned int)(*vars->val.integer & 0xffffffff);
                        break;
                    }
                case  ASN_COUNTER64:
                    {
                        snmp_value.val_type = VAL_TYPE_UINT64;
                        snmp_value.val.uint64_val = vars->val.counter64->high;
                        snmp_value.val.uint64_val <<= 32;
                        snmp_value.val.uint64_val += vars->val.counter64->low;
                        break;
                    }
                case  ASN_IPADDRESS:
                    ITL_SNMP_ASSIGN_STR(snmp_value, write_pos, remain_len, vars,
                            snprint_ipaddress, "snprint_ipaddress");
                    break;
                case  ASN_BIT_STR:
                    ITL_SNMP_ASSIGN_STR(snmp_value, write_pos, remain_len, vars,
                            snprint_bitstring, "snprint_bitstring");
                    break;
                case  ASN_NULL:
                    ITL_SNMP_ASSIGN_STR(snmp_value, write_pos, remain_len, vars,
                            snprint_null, "snprint_null");
                    break;
                case  ASN_OBJECT_ID:
                    ITL_SNMP_ASSIGN_STR(snmp_value, write_pos, remain_len, vars,
                            snprint_object_identifier, "snprint_object_identifier");
                    break;
                case  ASN_OCTET_STR:
                    ITL_SNMP_ASSIGN_STR(snmp_value, write_pos, remain_len, vars,
                            snprint_octet_string, "snprint_octet_string");
                case  ASN_OPAQUE:
                    ITL_SNMP_ASSIGN_STR(snmp_value, write_pos, remain_len, vars,
                            snprint_opaque, "snprint_opaque");
                default :
                    ERROR_LOG("Bad value type[%d], do not supported.", vars->type);
                    valid_type = false;
                    break;
            }
            if (valid_type)
            {
                value_vector.push_back(snmp_value);
            }

            if(vars->type != SNMP_ENDOFMIBVIEW &&
                    vars->type != SNMP_NOSUCHOBJECT && 
                    vars->type != SNMP_NOSUCHINSTANCE)
            {
                if(snmp_oid_compare(name, name_len, vars->name, vars->name_length) >= 0)
                {
                    char oid1_buf[MAX_OID_LEN] = {0};
                    char oid2_buf[MAX_OID_LEN] = {0};
                    snprint_objid(oid1_buf, sizeof(oid1_buf), name, name_len);
                    snprint_objid(oid2_buf, sizeof(oid2_buf), vars->name, vars->name_length);
                    ERROR_LOG("Error: OID not increasing:(%s >= %s).", oid1_buf, oid2_buf);
                    goon = false;
                    ret_val = -1;
                }
                //产生新的oid
                memmove((char *)name, (char *)vars->name, vars->name_length * sizeof(oid));
                name_len = vars->name_length;
            }
            else
            {
                goon = false;
            }
        }

        ITL_FREE(response, snmp_free_pdu);
    } while (goon);

    ITL_FREE(response, snmp_free_pdu);

    return ret_val;
}
