# 414chatroom
前端使用QT 后台使用muduo库实现的即使聊天软件

安装mysql
sudo apt install mysql-server mysql-client libmysqlclient-dev

安装redis
sudo apt install redis-server
sudo apt-get install libhiredis-dev
git clone https://github.com/sewenew/redis-plus-plus.git
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=/usr/lib/aarch64-linux-gnu/ -DCMAKE_INSTALL_PREFIX=/usr/lib/aarch64-linux-gnu/ ..
make
sudo make install