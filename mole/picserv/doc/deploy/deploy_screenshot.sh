#!/bin/sh

apt-get install apache2
apt-get install libapache2-mod-fcgid
apt-get install php5-cgi apache2-mpm-worker
apt-get install php-pear
apt-get install php5-dev 
apt-get install build-essential
apt-get install apache2-prefork-dev
apt-get install php5-mcrypt

pecl install apc

a2dismod cgid

# set apache docroot
mkdir /var/www/picserver
cp ./crossdomain.xml /var/www/picserver/

##
cp 000-default /etc/apache2/sites-enabled/
cp fcgid.conf /etc/apache2/mods-enabled/
cp apache2.conf  /etc/apache2/
cp apc.ini /etc/php5/conf.d/
cp php.ini /etc/php5/cgi/

# 
mkdir -P /opt/taomee/pic_server_root
mkdir -p /opt/taomee/pic_server_root/log
mkdir -p /opt/taomee/pic_server_root/screenshot_tmp_upload

chown -R www-data:www-data /opt/taomee/pic_server_root/screenshot_tmp_upload /opt/taomee/pic_server_root/log 

ln -s /opt/taomee/pic_server_root/screenshot_tmp_upload /var/www/picserver/screenshot_tmp_upload

tar zxvf cgi-bin.tgz -C /opt/taomee/pic_server_root/
/etc/init.d/apache2 restart

echo "30       1 * * 5,1 www-data     mv /opt/taomee/pic_server_root/log/screenshot_upload.error /opt/taomee/pic_server_root/log/screenshot_upload.error.1
*/30     * * * * www-data       find /opt/taomee/pic_server_root/screenshot_tmp_upload/ -type f -mmin +30 -exec rm {} \;" >> /etc/crontab
