///binlog缓存项统计
METRIC(binlog_cache_disk_use, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
METRIC(binlog_cache_use, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
///数据库与外界交互流量统计
METRIC(bytes_received, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
METRIC(bytes_sent, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
///数据库DML语句统计
METRIC(com_call_procedure, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
METRIC(com_delete, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
METRIC(com_insert, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
METRIC(com_select, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
METRIC(com_update, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
///临时表使用情况统计
METRIC(created_tmp_disk_tables, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
METRIC(created_tmp_files, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
METRIC(created_tmp_tables, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
///错误日是否可访问，1表示可访问，2表示存在不可访问，3表示不存在
METRIC(error_log_access, ITL_VALUE_INT32, "", "%d", SLOPE_CURR)
///错误日志大小
METRIC(error_log_size, ITL_VALUE_INT64, "", "%lld", SLOPE_CURR)
///是否有innodb的死锁情况--开关值0表示没有1表示有
//METRIC(have_innodb_deadlock, ITL_VALUE_INT32, "", "%d", SLOPE_CURR)
///是否有processlist 的死锁情况--开关值0表示没有1表示有
METRIC(have_locked_processlist, ITL_VALUE_INT32, "", "%d", SLOPE_CURR)
///数据库查询缓存--是否开过query cache
METRIC(have_query_cache, ITL_VALUE_INT32, "", "%d", SLOPE_CURR)
///是否存在用户权限的主机是通配的
METRIC(have_user_priv_host_wildcard, ITL_VALUE_INT32, "", "%d", SLOPE_CURR)
///是否存在用户权限的用户是root,且有grant权限
METRIC(have_user_priv_root_grant, ITL_VALUE_INT32, "", "%d", SLOPE_CURR)
///innoDB缓存页统计
METRIC(innodb_buffer_pool_pages_data, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
METRIC(innodb_buffer_pool_pages_dirty, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
METRIC(innodb_buffer_pool_pages_free, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
METRIC(innodb_buffer_pool_pages_total, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
///innoDB数据刷新情况统计
METRIC(innodb_data_fsyncs, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
METRIC(innodb_data_pending_fsyncs, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
METRIC(innodb_data_reads, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
METRIC(innodb_data_writes, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
///innodb 引擎的死锁事故
METRIC(innodb_deadlock_event, ITL_VALUE_STRING, "", "%s", SLOPE_CURR)
///instance alive
METRIC(instance_alive, ITL_VALUE_INT32, "", "%d", SLOPE_CURR)
///key_buffer读写命中率统计
METRIC(key_buffer_read_hit_rate, ITL_VALUE_DOUBLE, "%", "%.2lf", SLOPE_CURR)
METRIC(key_buffer_write_hit_rate, ITL_VALUE_DOUBLE, "%", "%.2lf", SLOPE_CURR)
METRIC(last_locked_processlist, ITL_VALUE_STRING, "", "%s", SLOPE_CURR)
METRIC(last_log_error, ITL_VALUE_STRING, "", "%s", SLOPE_CURR)
///是否记录慢查询的开关
METRIC(log_slow_queries, ITL_VALUE_INT32, "", "%d", SLOPE_CURR)
///慢查询记录阀值
METRIC(long_query_time, ITL_VALUE_UINT32, "", "%u", SLOPE_CURR)
///数据库连接数监控
METRIC(max_connections, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
METRIC(max_used_connections, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
///文件句柄相关
METRIC(open_files, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
//METRIC(open_tables, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
METRIC(open_tables, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
METRIC(opened_tables, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
///数据库协议版本信息
METRIC(protocol_version, ITL_VALUE_STRING, "", "%s", SLOPE_CURR)
///数据库查询缓存--query cache的命中计数
METRIC(qcache_hits, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
///数据库查询缓存--query cache的命中计数
METRIC(query_cache_type, ITL_VALUE_INT32, "", "%d", SLOPE_CURR)
///数据库DML语句统计
METRIC(questions, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
///文件句柄相关
METRIC(slave_open_temp_tables, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
///数据库活动状态信息
METRIC(slow_launch_threads, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
///慢查询数量统计
METRIC(slow_queries, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
///socket文件
METRIC(socket, ITL_VALUE_STRING, "", "%s", SLOPE_CURR)
///排序操作统计
METRIC(sort_range, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
METRIC(sort_rows, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
METRIC(sort_scan, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
///数据库表锁统计
METRIC(table_locks_immediate, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
METRIC(table_locks_waited, ITL_VALUE_UINT64, "", "%llu", SLOPE_DIFF)
///数据库活动状态信息
METRIC(threads_cached, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
METRIC(threads_connected, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
METRIC(threads_created, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
METRIC(threads_running, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
METRIC(uptime, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
METRIC(uptime_since_flush_status, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
///用户+主机+权限 信息
METRIC(user_host_priv, ITL_VALUE_STRING, "", "%s", SLOPE_CURR)
///数据库版本信息
METRIC(version, ITL_VALUE_STRING, "", "%s", SLOPE_CURR)

// 来自原来mysql-slave的一些监控项
METRIC(exec_master_log_pos, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
METRIC(master_host, ITL_VALUE_STRING, "", "%s", SLOPE_CURR)
METRIC(master_log_file, ITL_VALUE_STRING, "", "%s", SLOPE_CURR)
METRIC(master_port, ITL_VALUE_UINT32, "", "%u", SLOPE_CURR)
METRIC(read_master_log_pos, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
METRIC(relay_log_space, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
METRIC(seconds_behind_master, ITL_VALUE_UINT64, "", "%llu", SLOPE_CURR)
METRIC(slave_io_running, ITL_VALUE_INT32, "", "%d", SLOPE_CURR)
METRIC(slave_sql_running, ITL_VALUE_INT32, "", "%d", SLOPE_CURR)
