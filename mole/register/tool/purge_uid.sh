#!/bin/sh

# Stop Register
pkill Register
sleep 5
# Backup
cp ../dat/usr_id_file ../dat/usr_id_file.bk
cp ../dat/usr_id_idx_file ../dat/usr_id_idx_file.bk
cp ../dat/unreg_id_file ../dat/unreg_id_file.bk
# Purge registered user id and re-generate user id index file
./purge_registered_uid ../dat/usr_id_file ../dat/usr_id_idx_file
# Merge non-regged user id to user id file
./strnum2num32 ../dat/unreg_id_file ../dat/unregbin
cat ../dat/unregbin >> ../dat/usr_id_file
./gen_usr_id_idx ../dat/usr_id_file ../dat/usr_id_idx_file
rm ../dat/unreg_id_file
