/** 
 * ========================================================================
 * @file so_proto.h
 * @brief so和父进程的通讯协议
 * @author smyang
 * @version 1.0
 * @date 2012-07-06
 * Modify $Date: 2012-10-12 11:58:18 +0800 (五, 12 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_SO_PROTO_H_2012_07_06
#define H_SO_PROTO_H_2012_07_06


#include <string>
#include <vector>
#include "metric_value.h"
#include "file.h"

// 数据处理方式
// 求增量还是当前值
enum 
{
    SLOPE_CURR = 0,
    SLOPE_DIFF

};


// 从so文件中读取到的metric信息
struct metric_info_t
{
    // metric名称
    const char * name;
    // 返回值的类型
    uint32_t value_type;
    // 返回值的单位
    const char * unit;
    // 返回值的显示格式
    const char * fmt;
    uint32_t slope;
};




// metric的so文件
struct metric_so_t
{
    metric_so_t(const char * filename)
        : file(filename)
    {

    }

    c_file file;
    void * handle;
    int (*handle_init)(void * param);
    int (*handle_fini)();
    const metric_info_t * (*get_metric_info)(int index);
    int (*metric_handler)(int index, const char * arg, c_value * v);
};


#define SOCK_LEN (128)


struct mysql_so_param_t
{
    uint32_t port;
    char sock[SOCK_LEN];
    std::vector< std::string > metric_vec;
};

#endif
