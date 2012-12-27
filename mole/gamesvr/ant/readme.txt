ant/random/random.hpp

 1. int ranged_random(int min, int max)
    产生范围是 min 到 max 的伪随机数，包括 min 和 max。

ant/time/time.hpp

 1. suseconds_t timecmp(const timeval& tv1, const timeval& tv2)
    returns: 0 相等，正数 tv1 > tv2，负数 tv2 > tv1

 2. void timeadd(timeval& tv, double tmplus)
    把 tv 增加 tmplus 秒

 3. double timediff(const timeval& tv1, const timeval& tv2)
    返回 tv1 - tv2 的差，单位是秒

ant/inet/byteswap.hpp

 1. bswap(val)
    返回 val 转字节序后的值。val可以是任何基本整型

ant/inet/pdumanip.hpp

 1. template <typename T> void pack(void* pkg, T val, int& idx)
    将 val 打包到 (uint8_t*)pkg + idx，并把 idx 的值增加 val 相应的字节

 2. void pack(void* pkg, const void* val, std::size_t len, int& idx)
    将 val 开始，len 长度的数据打包到 (uint8_t*)pkg + idx，并把 idx 的值增加 len

 3. template <typename T> void unpack(const void* pkg, T& val, int& idx)
    将 (uint8_t*)pkg + idx 解包到 val，并把 idx 的值增加 val 相应的字节

 4. void unpack(const void* pkg, void* val, std::size_t len, int& idx)
    将 (uint8_t*)pkg + idx 开始，len 长度的数据解包到 val，并把 idx 的值增加 len


以上函数都放在名空间 ant 里。