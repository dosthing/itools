说明：
1、将对应编译器下的mydogser复制到打包工具下app/bin/目录或者base/下其他目录
2、修改base/root/init.sh  在/app/appStart.sh & 前加
mydogser &后保存

使用
1、查询/tmp/systemRunInfo 获取看门狗监听消息
2、浏览器输入ip:10080/log 也可以查看信息
3、浏览器输入ip:10080/adminxc12345678打开telnet