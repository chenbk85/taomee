/** 
 * ========================================================================
 * @file file
 * @brief 
 * @author TAOMEE
 * @version 1.0
 * @date 2012-08-15
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "file.h"
#include "itl_util.h"



c_file::c_file(const char * filename)
{
    STRNCPY(m_full, filename, sizeof(m_full));
    STRNCPY(m_filename, get_filename(filename), sizeof(m_filename));
    get_file_md5(m_full, m_md5);

    memset(m_link, 0, sizeof(m_link));
    ssize_t ret = readlink(m_full, m_link, sizeof(m_link));
    if (-1 == ret)
    {
        m_is_link = false;
        memset(m_link, 0, sizeof(m_link));

    }
    else
    {
        m_is_link = true;
    }

}
