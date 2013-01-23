/**
 * @file
 * @brief initialization file read and write API implementation
 * @author Deng Yangjun, modified by Jasonwang(王国栋)
 * @date 2007-1-9, modified date 2010-03-24
 * @version 0.2
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <iostream>
using namespace std;

#include "c_ini_file.h"

#define LEFT_BRACE '['
#define RIGHT_BRACE ']'

c_ini_file::c_ini_file()
    : m_file_buf(NULL), m_file_size(0), m_working_buf(NULL), m_errno(0)
{
    memset(m_path, 0, sizeof(m_path));
}

c_ini_file::~c_ini_file()
{
    uninit();
}

int c_ini_file::init(const char* path)
{
    if ((! path) || (strlen(path) == 0) || (strlen(path) > MAX_INI_FILE_PATH_LEN)) {
        set_err(INI_FILE_E_PARAMETER);
        return -1;
    }

    if (m_file_buf == NULL) {
        m_file_buf = (char*)malloc(sizeof(char) * MAX_FILE_BUF_SIZE);
        if (m_file_buf == NULL) {
            set_err(INI_FILE_E_OUT_OF_MEMORY);
            return -1;
        }
    }

    if (m_working_buf == NULL) {
        m_working_buf = (char*)malloc(sizeof(char) * MAX_FILE_BUF_SIZE);
        if (m_working_buf == NULL) {
            set_err(INI_FILE_E_OUT_OF_MEMORY);
            return -1;
        }
    }

    if (load_ini_file(path)) {
        return -1;
    }

    strcpy(m_path, path);
    set_err(INI_FILE_E_SUCCESS);
    return 0;
}

int c_ini_file::uninit()
{
    if (m_file_buf != NULL) {
        free(m_file_buf);
        m_file_buf = NULL;
    }

    if (m_working_buf != NULL) {
        free(m_working_buf);
        m_working_buf = NULL;
    }

    set_err(INI_FILE_E_SUCCESS);
    return 0;
}

int c_ini_file::release()
{
    delete this;
    return 0;
}

int c_ini_file::load_ini_file(const char *file)
{
    FILE* in = fopen(file, "r");
    if(NULL == in) {
        /// 打开文件失败不认为是错误，写入文件失败才是错误
        m_file_buf[0] = '\0';
        m_file_size = 0;
        return 0;
    }

    int i=0;
    m_file_buf[i] = fgetc(in);
    while(m_file_buf[i] != (char)EOF) {
        i++;

        /// 不允许文件大于缓冲区大小
        if (i >= MAX_FILE_BUF_SIZE) {
            set_err(INI_FILE_E_BUF_OVERFLOW);
            return -1;
        }

        m_file_buf[i] = fgetc(in);
    }

    m_file_buf[i]='\0';
    m_file_size = i;

    fclose(in);
    return 0;
}

int c_ini_file::newline(char c)
{
    return ('\n' == c ||  '\r' == c )? 1 : 0;
}

int c_ini_file::end_of_string(char c)
{
    return '\0'==c? 1 : 0;
}

int c_ini_file::left_barce(char c)
{
    return LEFT_BRACE == c? 1 : 0;
}

int c_ini_file::isright_brace(char c )
{
    return RIGHT_BRACE == c? 1 : 0;
}

int c_ini_file::is_space(char c)
{
    return (' '==c || '\t'==c)? 1 : 0;
}

/// 删除符号首尾的空格
int c_ini_file::trim_token(int *s, int *e)
{
    const char *p = m_file_buf;

    while (*s < *e) {
        if (is_space(p[*s])) {
            (*s) += 1;
        }
        else {
            break;
        }
    }

    while (*s < *e) {
        if (is_space(p[*e])) {
            (*e) -= 1;
        }
        else {
            break;
        }
    }

    return 0;
}

int c_ini_file::parse_file(const char *section, const char *key, int *sec_s, int *sec_e, int *key_s,int *key_e, int *value_s, int *value_e)
{
    const char *p = m_file_buf;
    int i=0;
    int sec_s_tmp = -1, sec_e_tmp = -1, key_s_tmp = -1, key_e_tmp = -1;

    *sec_e = *sec_s = *key_e = *key_s = *value_s = *value_e = -1;
    while(!end_of_string(p[i])) {
        //find the section
        if ((i == 0) || newline(p[i-1])) {
            /// 跳过行首的空格
            while(isspace(p[i])) {
                i++;
            }

            if(left_barce(p[i])) {
                int section_start = i + 1;

                //find the ']'
                do {
                    i++;
                } while(!isright_brace(p[i]) && !end_of_string(p[i]));

                sec_s_tmp = section_start;
                sec_e_tmp = i - 1;

                /// 删除 section 首尾的空格
                trim_token(&sec_s_tmp, &sec_e_tmp);

                if((strlen(section) == (size_t)(sec_e_tmp - sec_s_tmp + 1)) && (strncmp(p + sec_s_tmp, section, sec_e_tmp - sec_s_tmp + 1) == 0)) {
                    int newline_start = 0;

                    i++;

                    //Skip over space char after ']'
                    while(isspace(p[i])) {
                        i++;
                    }

                    *sec_s = section_start;
                    *sec_e = i;

                    while(! (newline(p[i-1]) && left_barce(p[i])) && !end_of_string(p[i])) {
                        int j = 0;
                        //get a new line
                        newline_start = i;

                        while(!newline(p[i]) && !end_of_string(p[i])) {
                            i++;
                        }

                        //now i  is equal to end of the line
                        j = newline_start;

                        if(p[j] != ':') {  //skip over comment
                            while(j < i && p[j] != '=') {
                                j++;
                                if('=' == p[j]) {

                                    //find the key ok
                                    key_s_tmp = newline_start;
                                    key_e_tmp = j-1;

                                    /// 删除 key 首尾的空格
                                    trim_token(&key_s_tmp, &key_e_tmp);

                                    if((strlen(key)==(size_t)(key_e_tmp - key_s_tmp + 1)) &&
                                            (strncmp(key, p + key_s_tmp, key_e_tmp - key_s_tmp + 1)==0)) {
                                        *key_s = key_s_tmp;
                                        *key_e = key_e_tmp;

                                        *value_s = j+1;
                                        *value_e = i-1;

                                        /// 删除值 value 的空格
                                        trim_token(value_s, value_e);

                                        return 0;
                                    }
                                }
                            }
                        }

                        i++;
                    }
                }
            }
            else {
                i++;
            }
        }
        else {
            i++;
        }
    }

    return -1;
}

int c_ini_file::read_string(const char *section, const char *key, char *value, int size, const char *default_value)
{
    if ((section == NULL) || (strlen(section) == 0) || (key == NULL) || (strlen(key) == 0) || (value == NULL) || (size <= 0)) {
        set_err(INI_FILE_E_PARAMETER);
        return -1;
    }

    if (m_file_buf == NULL) {
        if(default_value != NULL) {
            strncpy(value, default_value, size);
            set_err(INI_FILE_E_SUCCESS);
            return 0;
        }
        else {
            set_err(INI_FILE_E_NOT_INITED);
            return -1;
        }
    }

    int sec_s, sec_e, key_s, key_e, value_s, value_e;
    if(parse_file(section, key, &sec_s, &sec_e, &key_s, &key_e, &value_s, &value_e)) {
        if(default_value != NULL) {
            strncpy(value, default_value, size);
            set_err(INI_FILE_E_SUCCESS);
            return 0;
        }
        else {
            set_err(INI_FILE_E_KEY_NOT_FOUND);
            return -1;
        }
    }

    int cpcount = value_e - value_s + 1;
    if(size - 1 < cpcount) {
        cpcount =  size - 1;
    }

    memset(value, 0, size);
    memcpy(value, m_file_buf + value_s, cpcount);
    value[cpcount] = '\0';

    set_err(INI_FILE_E_SUCCESS);
    return 0;
}

int c_ini_file::read_int(const char *section, const char *key, int default_value)
{
    char value[32] = {0};
    if(read_string(section,key,value, sizeof(value), NULL)) {
        return default_value;
    }

    set_err(INI_FILE_E_SUCCESS);
    return atoi(value);
}

int c_ini_file::write_string(const char *section, const char *key, const char *value)
{
    if (m_file_buf == NULL) {
        set_err(INI_FILE_E_NOT_INITED);
        return -1;
    }

    if ((section == NULL) || (key == NULL) || (value == NULL)) {
        set_err(INI_FILE_E_PARAMETER);
        return -1;
    }

    int section_len = (int)strlen(section);
    int key_len = (int)strlen(key);
    int value_len = (int)strlen(value);

    if ((section_len == 0) || (key_len == 0) || (value_len == 0)) {
        set_err(INI_FILE_E_PARAMETER);
        return -1;
    }

    if (m_file_size == 0) {
        sprintf(m_file_buf, "[%s]\n%s=%s\n", section, key, value);
    }
    else {
        int sec_s, sec_e, key_s, key_e, value_s, value_e;
        parse_file(section, key, &sec_s, &sec_e, &key_s, &key_e, &value_s, &value_e);
        if (sec_s == -1) {
            /// not find the section, then add the new section at end of the file
            if (m_file_size + section_len + key_len + value_len + 6 > MAX_FILE_BUF_SIZE) {
                set_err(INI_FILE_E_BUF_OVERFLOW);
                return -1;
            }

            sprintf(m_file_buf + m_file_size, "\n[%s]\n%s=%s\n", section, key, value);
        }
        else if (key_s == -1) {
            /// not find the key, then add the new key=value at end of the section
            int append_len = key_len + value_len + 2;
            if (m_file_size + append_len > MAX_FILE_BUF_SIZE) {
                set_err(INI_FILE_E_BUF_OVERFLOW);
                return -1;
            }

            /// 数据往后移
            int idx = 0;
            for (idx = m_file_size + append_len; idx >= sec_e + append_len; idx--) {
                m_file_buf[idx] = m_file_buf[idx - append_len];
            }

            /// 填入新数据
            for (idx = sec_e; idx < sec_e + key_len; idx++) {
                m_file_buf[idx] = key[idx - sec_e];
            }

            m_file_buf[idx] = '=';
            idx ++;

            for (; idx < sec_e + key_len + value_len + 1; idx++) {
                m_file_buf[idx] = value[idx - key_len - sec_e - 1];
            }

            m_file_buf[idx] = '\n';
        }
        else {
            /// 更新值
            int idx = 0;
            int append_len = value_len - value_e + value_s -1;
            if (m_file_size + append_len > MAX_FILE_BUF_SIZE) {
                set_err(INI_FILE_E_BUF_OVERFLOW);
                return -1;
            }

            if (append_len <= 0) {
                /// 向前移动
                for (idx = value_e + append_len + 1; idx <= m_file_size + append_len; idx++) {
                    m_file_buf[idx] = m_file_buf[idx - append_len];
                }
            }
            else {
                /// 向后移动
                for (idx = m_file_size + append_len; idx >= value_e + append_len + 1; idx--) {
                    m_file_buf[idx] = m_file_buf[idx - append_len];
                }
            }

            /// 填入新数据
            for (idx = value_s; idx < value_s + value_len; idx++) {
                m_file_buf[idx] = value[idx - value_s];
            }
        }
    }

    FILE* out = fopen(m_path, "w");
    if(out == NULL) {
        set_err(INI_FILE_E_OPEN_FILE);
        return -1;
    }

    if(fputs(m_file_buf, out) == -1) {
        set_err(INI_FILE_E_WRITE_FILE);
        fclose(out);
        return -1;
    }

    m_file_size = strlen(m_file_buf);
    fclose(out);
    set_err(INI_FILE_E_SUCCESS);
    return 0;
}

int c_ini_file::write_int(const char *section, const char *key, int value)
{
    char str[32] = {0};
    sprintf(str, "%d", value);

    return write_string(section, key, str);
}

int c_ini_file::write_int_list(const char *section, const char *key, int* list, int list_cnt, char delimiter)
{
    if (list_cnt <= 0) {
        return 0;
    }

    if (m_working_buf == NULL) {
        set_err(INI_FILE_E_NOT_INITED);
        return -1;
    }

    m_working_buf[0] = '\0';
    char str[32] = {0};
    for (int i = 0; i < list_cnt; i++) {
        if (i == list_cnt - 1) {
            sprintf(str, "%d", list[i]);
        }
        else {
            sprintf(str, "%d%c", list[i], delimiter);
        }

        int slen = strlen(str);
        if (strlen(m_working_buf) + slen > MAX_FILE_BUF_SIZE) {
            set_err(INI_FILE_E_BUF_OVERFLOW);
            return -1;
        }

        strcat(m_working_buf, str);
    }

    return write_string(section, key, m_working_buf);
}

int c_ini_file::read_int_list(const char *section, const char *key, int* list, int list_cnt, char delimiter)
{
    if (m_working_buf == NULL) {
        set_err(INI_FILE_E_NOT_INITED);
        return -1;
    }

    if (read_string(section, key, m_working_buf, MAX_FILE_BUF_SIZE, NULL) == 0) {
        int cnt = 0;
        char* cur_pos = m_working_buf;
        int buflen = strlen(m_working_buf);
        for (int i=0; i<=buflen; i++) {
            if (cnt >= list_cnt) {
                break;
            }

            if ((m_working_buf[i] == delimiter) || (i == buflen)) {
                m_working_buf[i] = '\0';
                list[cnt] = atoi(cur_pos);
                cur_pos = m_working_buf + i + 1;
                cnt ++;
            }
        }

        return cnt;
    }

    return 0;
}

int c_ini_file::write_string_list(const char *section, const char *key, char** list, int list_cnt, char delimiter)
{
    if (list_cnt <= 0) {
        return 0;
    }

    if (m_working_buf == NULL) {
        set_err(INI_FILE_E_NOT_INITED);
        return -1;
    }

    if (strlen(list[0]) > MAX_FILE_BUF_SIZE) {
        set_err(INI_FILE_E_BUF_OVERFLOW);
        return -1;
    }

    strcpy(m_working_buf, list[0]);
    for (int i=1; i<list_cnt; i++) {
        if (strlen(m_working_buf) + strlen(list[i]) + 1 > MAX_FILE_BUF_SIZE) {
            set_err(INI_FILE_E_BUF_OVERFLOW);
            return -1;
        }

        strcat(m_working_buf, ",");
        strcat(m_working_buf, list[i]);
    }

    return write_string(section, key, m_working_buf);
}

int c_ini_file::read_string_list(const char *section, const char *key, char* list, int list_cnt, int list_len, char delimiter)
{
    if (m_working_buf == NULL) {
        set_err(INI_FILE_E_NOT_INITED);
        return -1;
    }

    if (read_string(section, key, m_working_buf, MAX_FILE_BUF_SIZE, NULL) == 0) {
        int cnt = 0;
        char* cur_pos = m_working_buf;
        int buflen = strlen(m_working_buf);
        for (int i=0; i<=buflen; i++) {
            if (cnt >= list_cnt) {
                break;
            }

            if ((m_working_buf[i] == delimiter) || (i == buflen)) {
                m_working_buf[i] = '\0';

                if ((int)strlen(cur_pos) > list_len) {
                    set_err(INI_FILE_E_BUF_OVERFLOW);
                    return -1;
                }

                strcpy(list + cnt * list_len, cur_pos);
                cur_pos = m_working_buf + i + 1;
                cnt ++;
            }
        }

        return cnt;
    }

    return 0;
}

int c_ini_file::is_hex(char hex)
{
    if (((hex >= '0') && (hex <= '9')) || ((hex >= 'A') && (hex <= 'F')) ||
        ((hex >= 'a') && (hex <= 'f'))) {
        return 1;
    }

    return 0;
}

char* c_ini_file::byte2hexstr(char byte, char* out_buf)
{
    out_buf[0] = (byte >> 4) > 9 ? ('A' + (byte >> 4) - 10): ('0' + (byte >> 4));
    out_buf[1] = (byte & 15) > 9 ? ('A' + (byte & 15) - 10): ('0' + (byte & 15));
    out_buf[2] = 0;

    return out_buf;
}

char c_ini_file::hexchar2byte(char hex1, char hex2)
{
    return ((hex1+(hex1>64?9:0))<<4) + ((hex2+(hex2>64?9:0))&15);
}

int c_ini_file::write_bin(const char *section, const char *key, char* bin, int bin_bytes)
{
    if (m_working_buf == NULL) {
        set_err(INI_FILE_E_NOT_INITED);
        return -1;
    }

    char hex_buf[3] = { 0 };
    m_working_buf[0] = '\0';
    for (int i=0; i<bin_bytes; i++) {
        strcat(m_working_buf, byte2hexstr(bin[i], hex_buf));
    }

    return write_string(section, key, m_working_buf);
}

int c_ini_file::read_bin( const char *section, const char *key, char* bin, int bin_bytes)
{
    if (m_working_buf == NULL) {
        set_err(INI_FILE_E_NOT_INITED);
        return -1;
    }

    if (read_string(section, key, m_working_buf, MAX_FILE_BUF_SIZE, NULL) == 0) {
        int cnt = 0;
        for (int i=0; i<(int)strlen(m_working_buf); i++) {
            if (! is_hex(m_working_buf[i])) {
                set_err(INI_FILE_E_ILLEGAL_FORMAT);
                return -1;
            }

            if ((i + 1) % 2 == 0) {
                bin[cnt] = hexchar2byte(m_working_buf[i-1], m_working_buf[i]);
                cnt ++;
            }
        }

        return cnt;
    }

    return 0;
}

void c_ini_file::set_err(int errno)
{
    m_errno = errno;
}

int c_ini_file::get_last_errno() const
{
    return m_errno;
}

const char* c_ini_file::get_last_errstr() const
{
    switch (m_errno) {
    case INI_FILE_E_SUCCESS:
        return "Success";
    case INI_FILE_E_PARAMETER:
        return "Parameter error.";
    case INI_FILE_E_OPEN_FILE:
        return "File open fail.";
    case INI_FILE_E_WRITE_FILE:
        return "File write fail.";
    case INI_FILE_E_BUF_OVERFLOW:
        return "Not enough buffer.";
    case INI_FILE_E_NOT_INITED:
        return "Not inited.";
    case INI_FILE_E_KEY_NOT_FOUND:
        return "Key not found.";
    case INI_FILE_E_OUT_OF_MEMORY:
        return "Out of memory.";
    case INI_FILE_E_ILLEGAL_FORMAT:
        return "Illegal ini file format.";
    default:
        return "Unkown error.";
    }

    return "Unkown error.";
}

int create_ini_file_instance(i_ini_file** pp_instance)
{
    if (pp_instance == NULL) {
        return -1;
    }

    c_ini_file *p_instance = new(nothrow) c_ini_file;
    if (p_instance == NULL) {
        return -1;
    }

    *pp_instance = dynamic_cast<i_ini_file*>(p_instance);
    return 0;
}
