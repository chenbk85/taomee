/*
 * =====================================================================================
 *
 *       Filename:  http_transfer.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年11月19日 14时29分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  maxwell (zdl), maxwell@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <string.h>

#include"http_transfer.h"

int Chttp_transfer::fill_post_data(void *ptr, size_t size, size_t nmemb, void *userp)
{
    struct WriteThis *pooh = (struct WriteThis *)userp;

    if(size*nmemb < 1){
        return 0;
    }
    if(pooh->sizeleft) {
        *(char *)ptr = pooh->readptr[0]; 
        pooh->readptr++;                 
        pooh->sizeleft--;                
        return 1;                        
    }   
    return 0;                          
}

int Chttp_transfer::back_post(void *buffer, size_t size, size_t nmemb, void *stream)
{
	struct post_back *back_data = (struct post_back*)stream;
	memcpy(back_data->data + back_data->len, (char*)buffer, nmemb);
	back_data->len += nmemb;
	back_data->data[back_data->len] = '\0';

	//printf("\nsize:%d nmemb:%d\n",size, nmemb);
	//printf("buf:%s ,buf_len:%d", back_data->data, back_data->len);
	return nmemb;
}

size_t Chttp_transfer::my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
    struct httpfile *out=(struct httpfile *)stream;
    if(out && !out->stream) {
		if(out->tag == 0){
			out->stream=fopen(out->filename, "w");
			out->tag++;
		}else{	
        	out->stream=fopen(out->filename, "ab+");
		}
        if(!out->stream){
            return -1; /* failure, can't open file to write */
		}
	}

	return fwrite(buffer, size, nmemb, out->stream);
}

/** 
 * @brief  构造函数
 * @param   
 * @return  
 */

Chttp_transfer::Chttp_transfer()
{
}

/** 
 * @brief   发送post请求
 * @param   url 发送post地址
 * @param   data 发送的数据
 * @return   
 */
void Chttp_transfer::http_post(const char* url, const char* data)
{
	
	struct WriteThis pooh;
	pooh.readptr= data;
	pooh.sizeleft= strlen(data);
	CURLcode res;

	if(url == NULL || data == NULL){
		printf("http_post arguments err");
		return;
	}
	this->back_data.len= 0;
	memset(this->back_data.data, 0, POST_BACK_DATA_LEN);
	
	CURL *curl = curl_easy_init();
	if(curl){
	    
    	curl_easy_setopt(curl, CURLOPT_URL, url);
    	/* Now specify we want to POST data */
    	curl_easy_setopt(curl, CURLOPT_POST, 1L);

    	/* we want to use our own read function */
    	curl_easy_setopt(curl, CURLOPT_READFUNCTION, fill_post_data);
	
    	/* pointer to pass to our read function */
    	curl_easy_setopt(curl, CURLOPT_READDATA, &pooh);

    	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, back_post);
    	/* Set a pointer to our struct to pass to the callback */
    	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &(this->back_data));
    	
		/* get verbose debug output please */
    	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    	
#ifdef USE_CHUNKED
    {
      	struct curl_slist *chunk = NULL;

      	chunk = curl_slist_append(chunk, "Transfer-Encoding: chunked");
      	res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
      	/* use curl_slist_free_all() after the *perform() call to free this
         list again */
    }
#else
    	/* Set the expected POST size. If you want to POST large amounts of data,
       	consider CURLOPT_POSTFIELDSIZE_LARGE */
    	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (curl_off_t)pooh.sizeleft);
#endif

#ifdef DISABLE_EXPECT
    	/*
      	Using POST with HTTP 1.1 implies the use of a "Expect: 100-continue"
      	header.  You can disable this header with CURLOPT_HTTPHEADER as usual.
      	NOTE: if you want chunked transfer too, you need to combine these two
      	since you can only set one list of headers with CURLOPT_HTTPHEADER. */

    	/* A less good option would be to enforce HTTP 1.0, but that might also
       	have other implications. */
    {
   		struct curl_slist *chunk = NULL;

      	chunk = curl_slist_append(chunk, "Expect:");
      	res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
      	/* use curl_slist_free_all() after the *perform() call to free this
         list again */
    }
#endif
    	/* Perform the request, res will get the return code */
    	res = curl_easy_perform(curl);
    
        if(CURLE_OK != res) {
            /* we failed */
            fprintf(stderr, "curl told us %d\n", res);
        }
		/* always cleanup */
    	curl_easy_cleanup(curl);	
	}
}

/** 
 * @brief   获取post返回数据
 * @return post返回数据  
 * 
 */
std::string Chttp_transfer::get_post_back_data()
{	
	return std::string(this->back_data.data, this->back_data.len); 
}

/** 
 * @brief   下载文件 
 * @param   url 源 
 * @param   filename 保存的文件名
 * @return   
 */
void Chttp_transfer::download_file(const char* url, const char* filename)
{
 
 	CURLcode res;
  	struct httpfile http_file={
    	0,
		filename, 
    	NULL
  	};
	if(url == NULL){
		printf("url null err");
		return;
	}
    CURL *curl = curl_easy_init();

	if(curl) {
    	curl_easy_setopt(curl, CURLOPT_URL, url);
    	/* Define our callback to get called when there's data to be written */
    	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
    	/* Set a pointer to our struct to pass to the callback */
    	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &http_file);
    	/* Switch on full protocol/debug output */
    	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    	res = curl_easy_perform(curl);

    	/* always cleanup */
    	curl_easy_cleanup(curl);

    	if(CURLE_OK != res) {
      		fprintf(stderr, "curl told us %d\n", res);
    	}
  }

  	if(http_file.stream){
    	fclose(http_file.stream); /* close the local file */
	}	
}
