/*
 * =====================================================================================
 *
 *       Filename:  http_transfer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年11月19日 14时31分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  maxwell (zdl), maxwell@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef HTTP_TRANSFER_H
#define HTTP_TRANSFER_H
#include <stdio.h>
#include <iostream>
#include <curl/curl.h>

#define POST_TAG_LEN 17
#define POST_CONTENT_LEN 16

#define POST_BACK_DATA_LEN 10240


class Chttp_transfer
{
public: 
    Chttp_transfer();

    /** 
     * @brief   发送post请求
     * @param   url 发送post地址
     * @param   data 发送的数据
     * @return   
     */
    void http_post(const char *url, const char *data); 

    /** 
     * @brief   获取post返回数据
     * @return  post返回数据  
     * 
     */
    std::string get_post_back_data();	 

    /** 
     * @brief   下载文件 
     * @param   url 源 
     * @param   filename 保存的文件名
     * @return   
     */
    void download_file(const char *url, const char *filename);//下载文件

protected:
    static int fill_post_data(void *ptr, size_t size, size_t nmemb, void *userp);

    static int back_post(void *buffer, size_t size, size_t nmemb, void *stream);

    static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream);

private:
    struct WriteThis {
      const char *readptr;
      int sizeleft;
    };
    
    struct post_back{
    	char data[POST_BACK_DATA_LEN];
    	int len;	
    };
    
    struct httpfile {
    	int tag;
        const char *filename;
        FILE *stream;
    };

	struct post_back back_data;
};

#endif
