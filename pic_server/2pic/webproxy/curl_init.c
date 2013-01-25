#include <stdint.h>
#include <string.h>
#include <openssl/md5.h>
#include "libtaomee/log.h"
#include "curl_init.h"


static char error_buffer[CURL_ERROR_SIZE];
static int g_key_type;

int writer(char *data, uint32_t size, uint32_t nmemb, buffer_t *write_data)
{
	if(write_data == NULL)
		return 0;
	buffer_t* pb = (buffer_t*)write_data;
	uint32_t sizes = size * nmemb;
	if (sizeof(pb->data) < sizes)
	{
		ERROR_LOG("writer: pb->data < sizes, sizes=%u", sizes);
		return 0;
	}
	memcpy(pb->data, data, sizes);
	pb->len = sizes;
	DEBUG_LOG("writer1 data=%s, size=%u, nmemb=%u, write_data=%s len=%u", data, size, nmemb, 
	write_data->data,write_data->len);
	return sizes;
}


int curl_init(CURL** pconn, char *url, buffer_t *p_buffer)
{
	CURLcode code;
	CURL* conn;
	conn = curl_easy_init();
	if(conn == NULL)
	{
		ERROR_LOG("Failed to create CURL connection.\n");
		return -1;
	}
	*pconn = conn;
	code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, error_buffer);
	if(code != CURLE_OK)
	{
		ERROR_LOG("failed to set error buffer [%d]\n", code);
		return -1;
	}

	code = curl_easy_setopt(conn, CURLOPT_URL, url);
	if(code != CURLE_OK)
	{
		ERROR_LOG("failed to set url [%s]\n", error_buffer);
		return -1;
	}

	code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1);
	if(code != CURLE_OK)
	{
		ERROR_LOG("failed to redirect option [%s]\n", error_buffer);
		return -1;
	}

	code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, p_buffer);
	if(code != CURLE_OK)
	{
		ERROR_LOG("failed to set write data [%s]\n", error_buffer);
		return -1;
	}

	code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer);
	if(code != CURLE_OK)
	{
		ERROR_LOG("failed to set writer [%s]\n", error_buffer);
		return -1;
	}

	return 0;
}

int json_callback(void *ret, int type, const JSON_value *value){
	//DEBUG_LOG("type=%d",type);
	web_result_t *ctx = ret;
	switch(type){
		case JSON_T_KEY:
			if(strcmp(value->vu.str.value, "success") == 0){				
				g_key_type = success;	
				DEBUG_LOG("value->vu.str.value:%s",value->vu.str.value);
			}
			else if(strcmp(value->vu.str.value, "code") == 0){				
				g_key_type = code;	
				DEBUG_LOG("value->vu.str.value:%s",value->vu.str.value);
			}
			else if(strcmp(value->vu.str.value, "uid") == 0){				
				g_key_type = uid;	
				DEBUG_LOG("value->vu.str.value:%s",value->vu.str.value);
			}
			else if(strcmp(value->vu.str.value, "photoid") == 0){				
				g_key_type = photoid;	
				DEBUG_LOG("value->vu.str.value:%s",value->vu.str.value);
			}
		break;
		case JSON_T_TRUE:
			switch(g_key_type){
				case success:
					ctx->ret= 0;
					DEBUG_LOG("ret %u",ctx->ret);
				break;
				default:
					break;
			}
		break;
		case JSON_T_FALSE:
			switch(g_key_type){
				case success:
					ctx->ret= -1;
					DEBUG_LOG("ret %u",ctx->ret);
				break;
				default:
					break;
			}
		break;
		case JSON_T_INTEGER:
			switch(g_key_type){
				case code:
					ctx->errorid = value->vu.integer_value;
					DEBUG_LOG("errorid %u",ctx->errorid);
					break;
				case uid:
					ctx->userid= value->vu.integer_value;
					DEBUG_LOG("uid %u",ctx->userid);
					break;
				case photoid:
					ctx->photoid = value->vu.integer_value;
					DEBUG_LOG("photoid %u",ctx->photoid);
					break;
				default:
					break;
			}
		default:
		break;
	}
	return 1;
}


int send_llocc(char *bus_url,web_result_t* wet_ret,int (*p_callback)(void *ctx, int type, const JSON_value *value))
{

	CURL* conn = NULL;
	CURLcode code;
	buffer_t buf_tmp;	
	//curl_global_init(CURL_GLOBAL_ALL);
	if(curl_init(&conn, bus_url, &buf_tmp) != 0)
	{
		ERROR_LOG("connection initialize failed.");
		return -1;
	}
	code = curl_easy_perform(conn);
	if(code != CURLE_OK)
	{
		ERROR_LOG("failed to get %s(%s)\n", bus_url, error_buffer);
		return -1;
	}

	curl_easy_cleanup(conn);
	//curl_global_cleanup();

	//得到webServer返回
	int result = 0;
	JSON_config json_config;
	struct JSON_parser_struct *p_jc = NULL;
	init_JSON_config(&json_config);
	json_config.depth = 19;
	json_config.callback = p_callback;
	json_config.callback_ctx = (void*)wet_ret;
	json_config.allow_comments = 1;
	json_config.handle_floats_manually = 0;

	p_jc = new_JSON_parser(&json_config);

	int i;
	for(i = 0; i < buf_tmp.len; i++)
	{
		int next_char = buf_tmp.data[i];
		if(next_char <= 0)
		{
			break;
		}

		if(!JSON_parser_char(p_jc, next_char))
		{
			ERROR_LOG("JSON_parser_char :syntax error, byte:%d,%c", i,next_char);
			result = -1;
			goto done;
		}
	}

	if(!JSON_parser_done(p_jc))
	{
		ERROR_LOG("JSON_parser_done : syntax error");
		result = -1;
		goto done;
	}

done:
	delete_JSON_parser(p_jc);

	return result;

}
