#!/bin/sh

#创建目录
echo '#################### mkdir /home/nginx ##################'
mkdir /home/nginx
cd /home/nginx

#gcc编译器安装
echo '#################### install gcc environment ##################'
yum -y install gcc-c++

#pcre pcre-devel安装
echo '#################### install pcre pcre-devel ##################'
yum -y install pcre pcre-devel

#zlib安装
echo '#################### install zlib zlib-devel ##################'
yum -y install zlib zlib-devel

#openSSL安装
echo '#################### install openssl openssl-devel ##################'
yum -y install openssl openssl-devel

#php-fpm安装
echo '#################### install php-fpm ##################'
yum -y install php-fpm

#下载nginx安装包
echo '#################### download nginx packetge ##################'
wget http://nginx.org/download/nginx-1.6.2.tar.gz

#解压
echo '#################### install nginx ##################'
tar -zxvf nginx-1.6.2.tar.gz
cd nginx-1.6.2
 
#配置 编译 安装
./configure
make
make install
 
#配置环境
ln -s /usr/local/nginx/sbin/nginx /usr/bin/nginx

#设置开机启动
echo '#################### config auto start ##################'
echo /usr/local/nginx/sbin/nginx >> /etc/rc.local
chmod -R 755 /etc/rc.local
 
#启动
echo '#################### start nginx ##################'
nginx
 
#停止
#nginx -s stop
 
#退出
#nginx -s quit
 
#重启
#nginx -s reload
