/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file utils.h
 * @author richard <richard@taomee.com>
 * @date 2010-03-09
 */

#ifndef UTILS_H_2010_03_09
#define UTILS_H_2010_03_09

#include <stdint.h>
#include "../proto.h"

/**
 * @brief 为指定的信号安装信号处理函数
 * @param sig 要处理的信号
 * @param signal_handler 信号处理函数
 * @return 成功返回0，失败返回-1
 */
int mysignal(int sig, void(*signal_handler)(int));

/**
 * @brief 判断程序是否已经有实例正在运行
 * @return 如果已经有实例正在运行返回1，否则返回0，出错时返回-1
 */
int already_running(); 

/**
 * @brief 初始化设置proc标题
 * @param argc main函数的第一个参数
 * @param argv main函数的第二个参数
 * @return 无
 */
void init_proc_title(int argc, char *argv[]);

/**
 * @brief 设置proc标题
 * @param fmt 标题格式
 * @param ... 可变参数
 * @return 无
 */
void set_proc_title(const char *fmt, ...);

/**
 * @brief 反初始化设置proc标题
 * @return 无
 */
void uninit_proc_title();

inline slope_t cstr_to_slope(const char* str)
{
    if(NULL == str) {
        return SLOPE_UNSPECIFIED;
    }   
    
    if(!strcasecmp(str, "positive")) {
        return SLOPE_POSITIVE;
    } else  if(!strcasecmp(str, "negative")) {
        return SLOPE_NEGATIVE;
    } else if(!strcasecmp(str, "both")) {
        return SLOPE_BOTH;
    } else if(!strcasecmp(str, "zero")) {
        return SLOPE_ZERO;
    }

    return SLOPE_UNSPECIFIED;
}

struct  op_tag
{
    op_t op;
    char op_name[16];
};

inline char *op_to_cstr(op_t op) 
{
    static struct op_tag op_tags[] = 
    {   
        {OP_EQ, "="},
        {OP_GT, ">"},
        {OP_LT, "<"},
        {OP_GE, ">="},
        {OP_LE,  "<="}
    };  

    if(op == OP_UNKNOW)
        return NULL;

    for(unsigned int i = 0; i < sizeof(op_tags)/sizeof(op_tags[0]); i++)
    {   
        if(op_tags[i].op == op) 
            return op_tags[i].op_name;
    }   
    return NULL;
}  



inline op_t cstr_to_op(const char* str)
{
    if(str == NULL) 
	{
        return OP_UNKNOW;
    }   
    
    if(!strcasecmp(str, "eq")) 
	{
        return OP_EQ;
    }   
    
    if(!strcasecmp(str, "gt")) 
	{
        return OP_GT;
    }   
    
    if(!strcasecmp(str, "lt")) 
	{
        return OP_LT;
    }   
    
    if(!strcasecmp(str, "GE")) 
	{
        return OP_GE;
    }   

    if(!strcasecmp(str, "lE")) 
	{
        return OP_LE;
    }   

    return OP_UNKNOW;
}


/** 
 * @brief   类型判断函数
 * @param   const char*  要传入的数字符串
 * @return  false = no, true = yes 
 */
bool is_integer(const char *); 
bool is_intpos(const char *); 
bool is_intneg(const char *); 
bool is_intnonneg(const char *); 
bool is_intpercent(const char *); 

bool is_numeric(const char *); 
bool is_positive(const char *); 
bool is_negative(const char *); 
bool is_nonnegative(const char *); 
bool is_percentage(const char *); 
bool is_option(const char *); 
#define is_inet_addr(addr) resolve_host_or_addr(addr, AF_INET)

bool resolve_host_or_addr(const char *address, int family);

int get_local_ip_str(char *buf, unsigned int len);

int hostname_to_s_addr(const char *host_name);
/** 
 * @brief   sleep和usleep的封装
 * @param   usec  微妙数
 * @return  0=success,-1=failed
 */
int my_sleep(unsigned int usec);

#endif //UTILS_H

