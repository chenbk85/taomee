/*
author : mason@taomee.com
*/
#include <net-snmp/net-snmp-config.h>
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include <sys/types.h>
#if HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif

#include <sys/time.h>
#include <time.h>

#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <stdio.h>
#if HAVE_NETDB_H
#include <netdb.h>
#endif
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <net-snmp/utilities.h>
#include <net-snmp/net-snmp-includes.h>
#include "./snmplib.h"
#include "./log.h"

/**
 * @brief   简单的snmpwalk函数
 * @param   ip            交换机ip
 * @param   community     交换机团体
 * @param   root_oid      遍历的oid根
 * @param   timeout       超时值
 * @param   retry         重试次数
 * @param   val_list      值列表数组
 * @param   val_list_len  值列表数组长度:值结果参数，传入最大个数传出实际个数
 * @param   buf           缓冲区 外部分配的用于保存值列表中的字符串类型的数据部分
 * @param   buf_len       缓冲区长度
 * @return  -1 = failed 0 = success
 */
int simple_snmp_walk(const char *ip,
        const char *community,
        const char *root_oid,
        unsigned int timeout,
        unsigned retry,
        snmp_val_t *val_list,
        unsigned int *val_list_len,
        char *buf,
        unsigned int buf_len)
{
    if(NULL == ip || NULL == community || NULL == root_oid || NULL == val_list || NULL == val_list_len) {
        ERROR_LOG("ERROR: parament cannot be NULL.");
        return -1;
    }
    //if(!is_inet_addr(ip)) 
    //{
    //    ERROR_LOG("Inet addr[%s] can not be resolved.", ip);
    //    return -1;
    //}

    netsnmp_session session;
    void *sd = NULL;//session handle
    netsnmp_pdu *pdu = NULL, *response = NULL;
    netsnmp_variable_list *vars = NULL;
    oid name[MAX_OID_LEN] = {0};
    size_t name_length = 0;
    oid root[MAX_OID_LEN] = {0};
    size_t rootlen = 0;
    int count = 0;
    int goon = 0;
    int status = 0;
    int exitval = 0;
    unsigned int i = 0;
    unsigned int max_val_list_len = *val_list_len;
    char *wp = buf;
    int remain_len = buf_len - 1;//给结尾'\0'留一个空间
    unsigned int len = 0;

    //init the session
    snmp_sess_init(&session);
    session.peername = (char *)ip;
    session.version = SNMP_VERSION_2c;
    session.community = (unsigned char *)community;
    session.community_len = strlen(community);
    session.retries = retry >= 0 ? retry : 0;
    session.timeout = timeout >= 0 ? timeout * 1000000L : DEFAULT_TIME_OUT * 1000000L;//缺省10秒

    sd = snmp_sess_open(&session);
    if(NULL == sd) {
        ERROR_LOG("snmp_sess_open(%s) failed.", ip);
        return -1;
    }

    rootlen = MAX_OID_LEN;
    if(NULL == snmp_parse_oid(root_oid, root, &rootlen)) {
        ERROR_LOG("snmp_parse_oid() for root_oid[%s] failed.", root_oid);
        snmp_sess_close(sd);
        return -1;
    }

    memmove(name, root, rootlen * sizeof(oid));
    name_length = rootlen;

    goon = 1;
    while(goon && i < max_val_list_len) {
        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
        snmp_add_null_var(pdu, name, name_length);

        //发送请求
        status = snmp_sess_synch_response(sd, pdu, &response);
        if(STAT_SUCCESS == status) {//成功
            if(SNMP_ERR_NOERROR == response->errstat) {//无错
                for(vars = response->variables; vars; vars = vars->next_variable) {
                    //不是这个子树的
                    if(vars->name_length < rootlen || memcmp(root, vars->name, rootlen * sizeof(oid)) != 0) {
                        goon = 0;
                        continue;
                    }
                    //for print debug  info
                    //char buf[256] = {0};
                    //snprint_variable(buf, sizeof(buf), vars->name, vars->name_length, vars);
                    //DEBUG_LOG("Snmp Walk output:[%s]", buf);
                    switch(vars->type) {
                    case  ASN_INTEGER:
                    {
                        val_list[i].val_type = VAL_TYPE_INT32;
                        val_list[i].val.int32_val = *vars->val.integer;
                        i++;
                        break;
                    }
                    case  ASN_GAUGE:
                    case  ASN_COUNTER:
                    case  ASN_TIMETICKS:
                    case  ASN_UINTEGER:
                    {
                        val_list[i].val_type = VAL_TYPE_UINT32;
                        val_list[i].val.uint32_val = (unsigned int)(*vars->val.integer & 0xffffffff);
                        i++;
                        break;
                    }
                    case  ASN_COUNTER64:
                    {
                        val_list[i].val_type = VAL_TYPE_UINT64;
                        val_list[i].val.uint64_val = vars->val.counter64->high;
                        val_list[i].val.uint64_val <<= 32;
                        val_list[i].val.uint64_val += vars->val.counter64->low;
                        i++;
                        break;
                    }
                    case  ASN_IPADDRESS:
                    {
                        val_list[i].val_type = VAL_TYPE_STR;
                        val_list[i].val.str_val = NULL;
                        if(remain_len > 0 
                           && (len = snprint_ipaddress(wp, remain_len, vars, NULL, NULL, NULL)) > 0) {
                            val_list[i].val.str_val = wp;
                            i++;
                            len++;//留一个位置给'\0'
                            remain_len -= len;
                            if(remain_len <= 0) {
                                ERROR_LOG("Buffer space[%u] is not enough.", buf_len);
                                break;
                            }
                            wp[len - 1] = '\0';
                            wp += len;
                        }
                        break;
                    }
                    case  ASN_BIT_STR:
                    {
                        val_list[i].val_type = VAL_TYPE_STR;
                        val_list[i].val.str_val = NULL;
                        if(remain_len > 0 && 
                                (len = snprint_bitstring(wp, remain_len, vars, NULL, NULL, NULL)) > 0) {
                            val_list[i].val.str_val = wp;
                            i++;
                            len++;//留一个位置给'\0'
                            remain_len -= len;
                            if(remain_len <= 0) {
                                ERROR_LOG("Buffer space[%u] is not enough.", buf_len);
                                break;
                            }
                            wp[len - 1] = '\0';
                            wp += len;
                        }
                        break;
                    }
                    case  ASN_NULL:
                    {
                        val_list[i].val_type = VAL_TYPE_STR;
                        val_list[i].val.str_val = NULL;
                        if(remain_len > 0 && 
                                (len = snprint_null(wp, remain_len, vars, NULL, NULL, NULL)) > 0) {
                            val_list[i].val.str_val = wp;
                            i++;
                            len++;//留一个位置给'\0'
                            remain_len -= len;
                            if(remain_len <= 0) {
                                ERROR_LOG("Buffer space[%u] is not enough.", buf_len);
                                break;
                            }
                            wp[len - 1] = '\0';
                            wp += len;
                        }
                        break;
                    }
                    case  ASN_OBJECT_ID:
                    {
                        val_list[i].val_type = VAL_TYPE_STR;
                        val_list[i].val.str_val = NULL;
                        if(remain_len > 0 &&
                                (len = snprint_object_identifier(wp, remain_len, vars, NULL, NULL, NULL)) > 0) {
                            val_list[i].val.str_val = wp;
                            i++;
                            len++;//留一个位置给'\0'
                            remain_len -= len;
                            if(remain_len <= 0) {
                                ERROR_LOG("Buffer space[%u] is not enough.", buf_len);
                                break;
                            }
                            wp[len - 1] = '\0';
                            wp += len;
                        }
                        break;
                    }
                    case  ASN_OCTET_STR:
                    {
                        val_list[i].val_type = VAL_TYPE_STR;
                        val_list[i].val.str_val = NULL;
                        if(remain_len > 0 && 
                                (len = snprint_octet_string(wp, remain_len, vars, NULL, NULL, NULL)) > 0) {
                            val_list[i].val.str_val = wp;
                            i++;
                            len++;//留一个位置给'\0'
                            remain_len -= len;
                            if(remain_len <= 0) {
                                ERROR_LOG("Buffer space[%u] is not enough.", buf_len);
                                break;
                            }
                            wp[len - 1] = '\0';
                            wp += len;
                        }
                        break;
                    }
                    case  ASN_OPAQUE:
                    {
                        val_list[i].val_type = VAL_TYPE_STR;
                        val_list[i].val.str_val = NULL;
                        if(remain_len > 0 && 
                                (len = snprint_opaque(wp, remain_len, vars, NULL, NULL, NULL)) > 0)
                        {
                            val_list[i].val.str_val = wp;
                            i++;
                            len++;//留一个位置给'\0'
                            remain_len -= len;
                            if(remain_len <= 0) {
                                ERROR_LOG("Buffer space[%u] is not enough.", buf_len);
                                break;
                            }
                            wp[len - 1] = '\0';
                            wp += len;
                        }
                        break;
                    }
                    default :
                        ERROR_LOG("Bad value type[%d], do not supported.", vars->type);
                        break;
                    }

                    if(vars->type != SNMP_ENDOFMIBVIEW &&
                       vars->type != SNMP_NOSUCHOBJECT && 
                       vars->type != SNMP_NOSUCHINSTANCE) {
                        if(snmp_oid_compare(name, name_length, vars->name, vars->name_length) >= 0) {
                            char oid1_buf[MAX_OID_LEN] = {0};
                            char oid2_buf[MAX_OID_LEN] = {0};
                            snprint_objid(oid1_buf, sizeof(oid1_buf), name, name_length);
                            snprint_objid(oid2_buf, sizeof(oid2_buf), vars->name, vars->name_length);
                            ERROR_LOG("Error: OID not increasing:(%s >= %s).", oid1_buf, oid2_buf);
                            goon = 0;
                            exitval = -1;
                        }
                        //产生新的oid
                        memmove((char *)name, (char *)vars->name, vars->name_length * sizeof(oid));
                        name_length = vars->name_length;
                    } else {
                        goon = 0;
                    }
                }
            } else {
                goon = 0;
                if(SNMP_ERR_NOSUCHNAME == response->errstat) {
                    ERROR_LOG("End of MIB[SNMP_ERR_NOSUCHNAME].");
                } else {
                    ERROR_LOG("Error in packet.Reason: %s.", snmp_errstring(response->errstat));
                    if(response->errindex != 0) {
                        ERROR_LOG("Failed object:");
                        for(count = 1, vars = response->variables; vars && count != response->errindex;
                            vars = vars->next_variable, count++) {
                            char oid_buf[MAX_OID_LEN] = {0};
                            snprint_objid(oid_buf, sizeof(oid_buf), vars->name, vars->name_length);
                            ERROR_LOG("    |__%s", oid_buf);
                        }
                    }
                    exitval = -1;
                }
            }
        } else if(STAT_TIMEOUT == status) {
            ERROR_LOG("Timeout: no response from [%s].", session.peername);
            goon = 0;
            exitval = -1;
        } else { 
            ERROR_LOG("Critical:Unknown error from session [%s].", session.peername);
            goon = 0;
            exitval = -1;
        }
        if(response) {
            snmp_free_pdu(response);
        }
    }
    snmp_sess_close(sd);
    *val_list_len = i;

    return exitval;
}

/**
 * @brief   简单的snmpget函数
 * @param   ip            交换机ip
 * @param   community     交换机团体
 * @param   oids          遍历的所有oid
 * @param   oid_num       遍历的所有oid的个数
 * @param   timeout       超时值
 * @param   retry         重试次数
 * @param   val_list      值列表数组
 * @param   val_list_len  值列表数组长度:值结果参数，传入最大个数传出实际个数
 * @param   buf           缓冲区 外部分配的用于保存值列表中的字符串类型的数据部分
 * @param   buf_len       缓冲区长度
 * @return  -1 = failed 0 = success
 */
int simple_snmp_get(const char *ip,
        const char *community,
        const char **oids,
        unsigned int oid_num,
        unsigned int timeout,
        unsigned int retry,
        snmp_val_t *val_list,
        unsigned int *val_list_len,
        char *buf,
        unsigned int buf_len)
{
    if(NULL == ip || NULL == community || NULL == val_list || NULL == val_list_len) {
        ERROR_LOG("ERROR: parament cannot be NULL.");
        return -1;
    }
    if(NULL == oids || 0 == oid_num) {
        ERROR_LOG("No object ids.");
        return -1;
    }
    if(oid_num > SNMP_MAX_CMDLINE_OIDS) {
        ERROR_LOG("Too many object identifiers specified,Only %d allowed in one request.", SNMP_MAX_CMDLINE_OIDS);
        return -1;
    }

    netsnmp_session session;
    void *sd = NULL;
    netsnmp_pdu *pdu = NULL;
    netsnmp_pdu *response = NULL;
    netsnmp_variable_list *vars = NULL;
    int count;
    int current_name = 0;
    char *names[SNMP_MAX_CMDLINE_OIDS];
    oid name[MAX_OID_LEN];
    size_t name_length;
    int status;
    int failures = 0;
    int exitval = 0;
    unsigned int i = 0;
    unsigned int max_val_list_len = *val_list_len;
    char *wp = buf;
    int remain_len = buf_len - 1;
    unsigned int len = 0;

    for(unsigned int k = 0; k < oid_num; k++) {
        names[current_name++] = (char *)oids[k];
    }

    //init the session
    snmp_sess_init(&session);
    session.peername = (char *)ip;
    session.version = SNMP_VERSION_2c;
    session.community = (unsigned char *)community;
    session.community_len = strlen(community);
    session.retries = retry >= 0 ? retry : 0;
    session.timeout = timeout >= 0 ? timeout * 1000000L : DEFAULT_TIME_OUT * 1000000L;//缺省10秒

    sd = snmp_sess_open(&session);
    if(NULL == sd) {
        ERROR_LOG("snmp_sess_open(%s) failed.", ip);
        return -1;
    }

    pdu = snmp_pdu_create(SNMP_MSG_GET);
    for(count = 0; count < current_name; count++) {
        name_length = MAX_OID_LEN;
        if(!snmp_parse_oid(names[count], name, &name_length)) {
            ERROR_LOG("Parse oid [%s] failed.", names[count]);
            failures++;
        } else {
            snmp_add_null_var(pdu, name, name_length);
        }
    }
    if(failures) {
        ERROR_LOG("Wrong oid in oid list, will return.");
        snmp_sess_close(sd);
        return -1;
    }

    //发送请求
    //snmp_synch_response_cb;
    status = snmp_sess_synch_response(sd, pdu, &response);
    if(STAT_SUCCESS == status) {
        if(SNMP_ERR_NOERROR == response->errstat) {
            for(vars = response->variables; vars && i < max_val_list_len; vars = vars->next_variable) {
                //for print debug info
                //char buf[256] = {0};
                //snprint_variable(buf, sizeof(buf), vars->name, vars->name_length, vars);
                //DEBUG_LOG("Snmp Get output:[%s]", buf);

                switch(vars->type) {
                case ASN_INTEGER:
                {
                    val_list[i].val_type = VAL_TYPE_INT32;
                    val_list[i].val.int32_val = (int)(*vars->val.integer);
                    i++;
                    break;
                }
                case ASN_GAUGE:
                case ASN_COUNTER:
                case ASN_TIMETICKS:
                case ASN_UINTEGER:
                {
                    val_list[i].val_type = VAL_TYPE_UINT32;
                    val_list[i].val.uint32_val = (unsigned int)(*vars->val.integer & 0xffffffff);
                    i++;
                    break;
                }
                case  ASN_COUNTER64:
                {
                    val_list[i].val_type = VAL_TYPE_UINT64;
                    val_list[i].val.uint64_val = vars->val.counter64->high;
                    val_list[i].val.uint64_val <<= 32;
                    val_list[i].val.uint64_val += vars->val.counter64->low;
                    i++;
                    break;
                }
                case ASN_IPADDRESS:
                {
                    val_list[i].val_type = VAL_TYPE_STR;
                    val_list[i].val.str_val = NULL;
                    if(remain_len > 0 && 
                            (len = snprint_ipaddress(wp, remain_len, vars, NULL, NULL, NULL)) > 0) {
                        val_list[i].val.str_val = wp;
                        i++;
                        len++;//留一个位置给'\0'
                        remain_len -= len;
                        if(remain_len <= 0) {
                            ERROR_LOG("No enough buffer space[%u] for value.", buf_len);
                            break;
                        }
                        wp[len - 1] = '\0';
                        wp += len;
                    }
                    break;
                }
                case  ASN_BIT_STR:
                {
                    val_list[i].val_type = VAL_TYPE_STR;
                    val_list[i].val.str_val = NULL;
                    if(remain_len > 0 && 
                            (len = snprint_bitstring(wp, remain_len, vars, NULL, NULL, NULL)) > 0) {
                        val_list[i].val.str_val = wp;
                        i++;
                        len++;//留一个位置给'\0'
                        remain_len -= len;
                        if(remain_len <= 0) {
                            ERROR_LOG("No enough buffer space[%u] for value.", buf_len);
                            break;
                        }
                        wp[len - 1] = '\0';
                        wp += len;
                    }
                    break;
                }
                case  ASN_NULL:
                {
                    val_list[i].val_type = VAL_TYPE_STR;
                    val_list[i].val.str_val = NULL;
                    if(remain_len > 0 &&
                            (len = snprint_null(wp, remain_len, vars, NULL, NULL, NULL)) > 0) {
                        val_list[i].val.str_val = wp;
                        i++;
                        len++;//留一个位置给'\0'
                        remain_len -= len;
                        if(remain_len <= 0) {
                            ERROR_LOG("No enough buffer space[%u] for value.", buf_len);
                            break;
                        }
                        wp[len - 1] = '\0';
                        wp += len;
                    }
                    break;
                }
                case  ASN_OBJECT_ID:
                {
                    val_list[i].val_type = VAL_TYPE_STR;
                    val_list[i].val.str_val = NULL;
                    if(remain_len > 0 && 
                            (len = snprint_object_identifier(wp, remain_len, vars, NULL, NULL, NULL)) > 0)
                    {
                        val_list[i].val.str_val = wp;
                        i++;
                        len++;//留一个位置给'\0'
                        remain_len -= len;
                        if(remain_len <= 0) {
                            ERROR_LOG("No enough buffer space[%u] for value.", buf_len);
                            break;
                        }
                        wp[len - 1] = '\0';
                        wp += len;
                    }
                    break;
                }
                case  ASN_OCTET_STR:
                {
                    val_list[i].val_type = VAL_TYPE_STR;
                    val_list[i].val.str_val = NULL;
                    if(remain_len > 0 && 
                            (len = snprint_octet_string(wp, remain_len, vars, NULL, NULL, NULL)) > 0) {
                        val_list[i].val.str_val = wp;
                        i++;
                        len++;//留一个位置给'\0'
                        remain_len -= len;
                        if(remain_len <= 0) {
                            ERROR_LOG("No enough buffer space[%u] for value.", buf_len);
                            break;
                        }
                        wp[len - 1] = '\0';
                        wp += len;
                    }
                    break;
                }
                case  ASN_OPAQUE:
                {
                    val_list[i].val_type = VAL_TYPE_STR;
                    val_list[i].val.str_val = NULL;
                    if(remain_len > 0 && 
                            (len = snprint_opaque(wp, remain_len, vars, NULL, NULL, NULL)) > 0) {
                        val_list[i].val.str_val = wp;
                        i++;
                        len++;//留一个位置给'\0'
                        remain_len -= len;
                        if(remain_len <= 0) {
                            ERROR_LOG("No enough buffer space[%u] for value.", buf_len);
                            break;
                        }
                        wp[len - 1] = '\0';
                        wp += len;
                    }
                    break;
                }
                default :
                    ERROR_LOG("Bad value type[0X%X], do not supported.", vars->type);
                    break;
                }
            }
        } else {
            ERROR_LOG("Error in packet.Reason: %s.", snmp_errstring(response->errstat));
            if(response->errindex != 0) {
                ERROR_LOG("Failed object:");
                for(count = 1, vars = response->variables; vars && count != response->errindex;
                        vars = vars->next_variable, count++) {
                    char oid_buf[MAX_OID_LEN] = {0};
                    snprint_objid(oid_buf, sizeof(oid_buf), vars->name, vars->name_length);
                    ERROR_LOG("    |__%s", oid_buf);
                }
            }
            exitval = -1;
        } 
    } else if(STAT_TIMEOUT == status) {
        ERROR_LOG("Timeout: no response from [%s].", session.peername);
        exitval = -1;
    } else {        
        ERROR_LOG("Critical:unknown error from session [%s].", session.peername);
        exitval = -1;
    }

    if(response) snmp_free_pdu(response);
    snmp_sess_close(sd);
    *val_list_len = i;

    return exitval;
}
