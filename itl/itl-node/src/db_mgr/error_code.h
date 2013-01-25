DB_MGR_ERR(DB_MGR_ERR_SOCK, "fail to find the sock")
DB_MGR_ERR(DB_MGR_ERR_CONNECT, "fail to connect to mysql")
DB_MGR_ERR(DB_MGR_ERR_REQ, "wrong request data")

// #define RESULT_OK               (0)
// #define RESULT_ESYSTEM          (0x1001)    //系统错误
// #define RESULT_ECOMMAND         (0x1002)    //命令号错误
// #define RESULT_EUNCONNECT       (0x1003)    //无法连接
// #define RESULT_ETIMEOUT         (0x1004)    //链接超时
// #define RESULT_ENOMEMORY        (0x1005)    //内存不足
// #define RESULT_ENODEHEAD        (0x1006)    //链接错误

// #define RESULT_EMYSQL           (0x10000000)//mysql起始错误号

// #define RESULT_ECALUCMD5        (0x2000)    //计算MD5出错
// #define RESULT_ENODECHECKMD5    (0x2001)    //MD5验证不正确
// #define RESULT_EGETHOSTIP       (0x2002)    //获取本机IP出错
// #define RESULT_EHOSTIP          (0x2003)    //实例ip错误
// #define RESULT_EMYSQL_CONNECT   (0x2004)    //连接mysql出错，mysql==null
// #define RESULT_EGETSQL          (0x2005)    //拼sql语句出错

// #define RESULT_EGETINFO_PAR     (0x2010)    //0x1001协议参数错误，db,table,column字段都不为空
// #define RESULT_EGETINFO_NOBUF   (0x2011)    //0x1001协议执行错误，信息过多，缓冲区溢出
// #define RESULT_ECHKUSER_FILE    (0x2012)    //检查用户信息时，打开文件错误
// #define RESULT_ECHKUSER_NOBUF   (0x2013)    //检查用户信息时，信息过多，缓冲区溢出
// #define RESULT_ECHKUSER_MORE    (0x2014)    //检查用户信息时，找到多个相同用户
// #define RESULT_EGETUSER_FILE    (0x2015)    //获取用户信息时，打开文件错误
// #define RESULT_EGETUSER_NOBUF   (0x2016)    //获取用户信息时，信息过多，缓冲区溢出
// #define RESULT_EGETINFO_DB      (0x2017)    //获取库信息出错
// #define RESULT_EGETINFO_TAB     (0x2018)    //获取表信息出错，库不存在
// #define RESULT_EGETINFO_COL     (0x2019)    //获取字段信息出错，库或表不存在

// #define RESULT_EGETPRIV_GLO     (0x2020)    //获取全局权限信息时，打开文件错误 user.MYD
// #define RESULT_EGETPRIV_DB      (0x2021)    //获取库权限信息时，打开文件错误 db.MYD
// #define RESULT_EGETPRIV_TAB     (0x2022)    //获取表权限信息时，打开文件错误 tables_priv.MYD
// #define RESULT_EGETPRIV_COL     (0x2023)    //获取字段权限信息时，打开文件错误 columns_priv.MYD
// #define RESULT_EGETPRIV_NOBUF   (0x2024)    //获取权限信息时，信息过多，缓冲区溢出
// #define RESULT_EGETPRIV_PAR     (0x2025)    //0x1002协议参数错误，level=1时，db,table,column字段都不为空
// #define RESULT_EGETPRIV_LEVPAR  (0x2026)    //0x1002协议参数错误，level值非法

// #define RESULT_EADDUSER_FILE    (0x2030)    //添加用户时，查询密码打开文件错误 user.MYD
// #define RESULT_EADDUSER_NOBUF   (0x2031)    //添加用户时，查询密码信息过多，缓冲区溢出
// #define RESULT_EADDUSER_MORE    (0x2032)    //添加用户时，检查用户信息，找到多个相同用户

// #define RESULT_EDELUSER_FILE    (0x2040)    //删除用户时，查询密码打开文件错误 user.MYD
// #define RESULT_EDELUSER_NOBUF   (0x2041)    //删除用户时，查询密码信息过多，缓冲区溢出
// #define RESULT_EDELUSER_MORE    (0x2042)    //删除用户时，检查用户信息，找到多个相同用户
// #define RESULT_EDELUSER_NOFIND  (0x2043)    //删除用户时，没有找到该用户

// #define RESULT_ECHKPWD_FILE     (0x2050)    //检查密码时，打开文件错误 user.MYD
// #define RESULT_ECHKPWD_NOBUF    (0x2051)    //检查密码时，信息过多，缓冲区溢出
// #define RESULT_ECHKPWD_MORE     (0x2052)    //检查密码时，找到多个相同用户

// #define RESULT_EADDPRIV_GTM     (0x2060)    //添加全局权限时，找到多个相同用户
// #define RESULT_EADDPRIV_GEXIST  (0x2061)    //添加全局权限时，权限已存在
// #define RESULT_EADDPRIV_GFILE   (0x2063)    //添加全局权限时，打开文件错误 user.MYD
// #define RESULT_EADDPRIV_DEXIST  (0x2064)    //添加库权限时，权限已存在
// #define RESULT_EADDPRIV_DFILE   (0x2065)    //添加库权限时，打开文件错误 db.MYD
// #define RESULT_EADDPRIV_TEXIST  (0x2066)    //添加表权限时，权限已存在
// #define RESULT_EADDPRIV_TFILE   (0x2067)    //添加表权限时，打开文件错误 tables_priv.MYD
// #define RESULT_EADDPRIV_CEXIST  (0x2068)    //添加字段权限时，权限已存在
// #define RESULT_EADDPRIV_CFILE   (0x2069)    //添加字段权限时，打开文件错误 columns_priv.MYD

// #define RESULT_EEDITPRIV_GFILE  (0x2070)    //修改全局权限时，打开文件错误 user.MYD
// #define RESULT_EEDITPRIV_DFILE  (0x2071)    //修改库权限时，打开文件错误 db.MYD
// #define RESULT_EEDITPRIV_TFILE  (0x2072)    //修改表权限时，打开文件错误 tables_priv.MYD
// #define RESULT_EEDITPRIV_CFILE  (0x2073)    //修改字段权限时，打开文件错误 columns_priv.MYD

// #define RESULT_WEIZHI           (100000)
// #define RESULT_WEIZHIXING       (10000)

// #endif //ERROR_CODE_2012_2_23_H
