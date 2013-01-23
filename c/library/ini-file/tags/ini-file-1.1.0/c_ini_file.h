/**
 * @file
 * @brief initialization file read and write API
 *	-size of the ini file must less than 16K
 *	-after '=' in key value pair, can not support empty char. this would not like WIN32 API
 *	-support comment using ';' prefix
 *	-can not support multi line
 * @author Deng Yangjun
 * @date 2007-1-9
 * @version 0.2
 */

#ifndef __INI_FILE_H__
#define __INI_FILE_H__

#include "i_ini_file.h"

/// 最大文件路径长度
#define MAX_INI_FILE_PATH_LEN (1024 * 4)

/// 最大文件缓冲区大小
#define MAX_FILE_BUF_SIZE (1024 * 1024)

/// 错误 ID
#define INI_FILE_E_SUCCESS 0
#define INI_FILE_E_PARAMETER 1
#define INI_FILE_E_OPEN_FILE 2
#define INI_FILE_E_WRITE_FILE 3
#define INI_FILE_E_BUF_OVERFLOW 4
#define INI_FILE_E_NOT_INITED 5
#define INI_FILE_E_KEY_NOT_FOUND 6
#define INI_FILE_E_OUT_OF_MEMORY 7
#define INI_FILE_E_ILLEGAL_FORMAT 8

class c_ini_file : public i_ini_file
{
public:
    c_ini_file();
    ~c_ini_file();

    int init(const char* path);
    int uninit();

    int release();

    int read_string(const char *section, const char *key, char *value, int size, const char *default_value);
    int read_int(const char *section, const char *key, int default_value);
    int read_int_list(const char *section, const char *key, int* list, int list_cnt, char delimiter);
    int read_string_list(const char *section, const char *key, char* list, int list_cnt, int list_len, char delimiter);
    int read_bin(const char *section, const char *key, char* bin, int bin_bytes);
    int write_string(const char *section, const char *key,const char *value);
    int write_int(const char *section, const char *key, int value);
    int write_int_list(const char *section, const char *key, int* list, int list_cnt, char delimiter);
    int write_string_list(const char *section, const char *key, char** list, int list_cnt, char delimiter);
    int write_bin(const char *section, const char *key, char* bin, int bin_bytes);

    int get_last_errno() const;
    const char* get_last_errstr() const;

protected:
    void set_err(int errno);
    int load_ini_file(const char *file);
    int newline(char c);
    int end_of_string(char c);
    int left_barce(char c);
    int isright_brace(char c);
    int is_hex(char hex);
    char* byte2hexstr(char byte, char* out_buf);
    char hexchar2byte(char hex1, char hex2);
    int is_space(char c);
    int trim_token(int *s, int *e);
    int parse_file(const char *section, const char *key, int *sec_s, int *sec_e, int *key_s,int *key_e, int *value_s, int *value_e);

private:
    char m_path[MAX_INI_FILE_PATH_LEN];

    char* m_file_buf;
    int m_file_size;

    char* m_working_buf;

    int m_errno;
};

#endif // ! __INI_FILE_H__

