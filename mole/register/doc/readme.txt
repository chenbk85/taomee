目前号码已用到50000000。

1. 关闭注册服务器

2. 清除已注册号码
cp ./usr_id_file ./usr_id_file.bk
cp ./usr_id_idx_file ./usr_id_idx_file.bk
./purge_registered_uid usr_id_file usr_id_idx_file

3. 和注册失败的号码合并
./strnum2num32 unreg_id_file unregbin
cat unregbin >> usr_id_file
./gen_usr_id_idx usr_id_file usr_id_idx_file
rm unreg_id_file

4. 产生新的可注册号和保留号（如有必要）
./gen_uid 15000000 19999999 reg rres
cat rres >> res

5. 生成二进制注册号，合并，生成注册号索引文件（如有必要）
./strnum2num32 reg regbin
cat regbin >> usr_id_file
./gen_usr_id_idx usr_id_file usr_id_idx_file

6. 打乱二进制注册号（如有必要）
./shuffle_uid usr_id_file

7. 生成时间戳文件（如有必要）
./gen_timestamp_file 70000000 timestamp_file

8. 启动注册服务器