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

# deploy configure files
cp 000-default /etc/apache2/sites-enabled/
cp fcgid.conf /etc/apache2/mods-enabled/
cp apache2.conf  /etc/apache2/
cp apc.ini /etc/php5/conf.d/
cp php.ini /etc/php5/cgi/

# 
mkdir -p /opt/taomee/pic_server_root/log
mkdir -p /opt/taomee/pic_server_root/mole_pictures
mkdir -p /opt/taomee/pic_server_root/display_wall
php5-cgi ./mkdir.php

# 
chown -R www-data:www-data /opt/taomee/pic_server_root/mole_pictures /opt/taomee/pic_server_root/log /opt/taomee/pic_server_root/display_wall

# configure apache2
ln -s /opt/taomee/pic_server_root/mole_pictures /var/www/picserver/mole_pictures
ln -s /opt/taomee/pic_server_root/display_wall /var/www/picserver/display_wall

tar zxvf cgi-bin.tgz -C /opt/taomee/pic_server_root/
/etc/init.d/apache2 restart
