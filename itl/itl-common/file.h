/** 
 * ========================================================================
 * @file file.h
 * @brief 
 * @author TAOMEE
 * @version 1.0
 * @date 2012-08-15
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_FILE_H_2012_08_15
#define H_FILE_H_2012_08_15


#include <limits.h>
#include "define.h"



class c_file
{
    public:

        c_file(const char * filename);

    public:

        // 文件全名，带路径
        char m_full[PATH_MAX];
        // 文件名，不带路径的
        char m_filename[NAME_MAX];

        char m_md5[MD5_LEN + 1];

    public:

        bool m_is_link;
        char m_link[PATH_MAX];
};


#endif
