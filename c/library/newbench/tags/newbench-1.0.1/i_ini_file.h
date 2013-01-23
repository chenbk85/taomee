/**
 * =====================================================================================
 *       @file  i_ini_file.h
 *      @brief  配置文件解析模块
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  03/24/2010 10:08:04 AM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  jasonwang (王国栋), jasonwang@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef I_INI_FILE_H
#define I_INI_FILE_H

struct i_ini_file
{
public:
    virtual ~i_ini_file() {};

    /**
     * @brief 初始化接口
     *
     * @param path 配置文件的路径
     *
     * @return 0 成功; <0 失败
     */
    virtual int init(const char* path) = 0;

    /**
     * @brief 反初始化接口
     *
     * @return 0 成功; <0 失败
     */
    virtual int uninit() = 0;

    /**
     * @brief 清除资源
     *
     * @return 0 成功; <0 失败
     */
    virtual int release() = 0;

    /**
     * @brief 读取字符串
     *
     * @param section 段名字符串
     * @param key 键名字符串
     * @param value 值字符串
     * @param size 值字符串的长度
     * @param default_value 值字符串的默认值
     *
     * @return 0 成功; <0 失败
     */
    virtual int read_string(const char *section, const char *key,char *value, int size,const char *default_value) = 0;

    /**
     * @brief 读取整形
     *
     * @param section 段名字符串
     * @param key 键名字符串
     * @param default_value 默认值
     *
     * @return 读取成功时返回读取的整型值，读取失败时返回默认值
     */
    virtual int read_int(const char *section, const char *key,int default_value) = 0;

    /**
     * @brief 读取整形数组
     *
     * @param section 段名字符串
     * @param key 键名字符串
     * @param list 保存整形数组的缓冲区
     * @param list_cnt 缓冲区大小
     * @param delimiter 解析时的分隔符
     *
     * @return >=0 读到的整形值的个数; <0 失败;
     */
    virtual int read_int_list(const char *section, const char *key, int* list, int list_cnt, char delimiter) = 0;

    /**
     * @brief 读取字符串数组
     *
     * @param section 段名字符串
     * @param key 键名字符串
     * @param list 保存字符串数组的二维字符数组缓冲区
     * @param list_cnt 缓冲区第一维的维数
     * @param list_len 缓冲区第二维的维数
     * @param delimiter 解析时的分隔符
     *
     * @return >=0 读到的字符串的个数; <0 失败;
     *
     */
    virtual int read_string_list(const char *section, const char *key, char* list, int list_cnt, int list_len, char delimiter) = 0;

    /**
     * @brief 读取二进制数据
     *
     * @param section 段名字符串
     * @param key 键名字符串
     * @param bin 保存二进制数据的缓冲区
     * @param bin_bytes 二进制数据缓冲区的大小
     *
     * @return =0 成功; <0 失败;
     */
    virtual int read_bin(const char *section, const char *key, char* bin, int bin_bytes) = 0;

    /**
     * @brief 写入字符串数据
     *
     * @param section 段名字符串
     * @param key 键名字符串
     * @param value 需要写入的字符串
     *
     * @return =0 成功; <0 失败;
     */
    virtual int write_string(const char *section, const char *key,const char *value) = 0;

    /**
     * @brief 写入整形数据
     *
     * @param section 段名字符串
     * @param key 键名字符串
     * @param value 需要写入的整形数据
     *
     * @return =0 成功; <0 失败;
     */
    virtual int write_int(const char *section, const char *key, int value) = 0;

    /**
     * @brief 写入整形数组
     *
     * @param section 段名字符串
     * @param key 键名字符串
     * @param list 需要写入的整形数组
     * @param list_cnt 整形数组的大小
     * @param delimiter 用于分隔数组元素的分隔符
     *
     * @return =0 成功; <0 失败;
     */
    virtual int write_int_list(const char *section, const char *key, int* list, int list_cnt, char delimiter) = 0;

    /**
     * @brief 写入字符串数组
     *
     * @param section 段名字符串
     * @param key 键名字符串
     * @param list 需要写入的字符串数组
     * @param list_cnt 字符串数组的大小
     * @param delimiter 用于分隔数组元素的分隔符
     *
     * @return =0 成功; <0 失败;
     */
    virtual int write_string_list(const char *section, const char *key, char** list, int list_cnt, char delimiter) = 0;

    /**
     * @brief 写入二进制数据块
     *
     * @param section 段名字符串
     * @param key 键名字符串
     * @param bin 需要写入的二进制数据块
     * @param bin_bytes 二进制数据块的大小
     *
     * @return =0 成功; <0 失败;
     */
    virtual int write_bin(const char *section, const char *key, char* bin, int bin_bytes) = 0;

    /**
     * @brief 获取最后一次操作的错误码
     *
     * @return 最后一次操作的错误码
     */
    virtual int get_last_errno() const = 0;

    /**
     * @brief 获取最后一次操作的错误描述
     *
     * @return 最后一次操作的错误描述
     */
    virtual const char* get_last_errstr() const = 0;
};

int create_ini_file_instance(i_ini_file** pp_instance);

#endif // ! I_INI_FILE_H
