cmake_minimum_required(VERSION 2.8.12)

project (414ChatRoom)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g -Wall -O0 -Wno-unused-variable -pthread")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

set(net_srcs 
base/AsyncLog.cpp
base/ConfigFileReader.cpp
base/Timestamp.cpp

net/Acceptor.cpp
net/ByteBuffer.cpp
net/Channel.cpp
net/Connector.cpp
net/EpollPoller.cpp
net/EventLoop.cpp
net/EventLoopThread.cpp
net/EventLoopThreadPool.cpp
net/InetAddress.cpp
net/Poller.cpp
net/PollPoller.cpp
net/ProtocolStream.cpp
net/SelectPoller.cpp
net/Sockets.cpp
net/TcpClient.cpp
net/TcpConnection.cpp
net/TcpServer.cpp
net/Timer.cpp
net/TimerQueue.cpp
)

set(mysqlapi_srcs
mysqlapi/DatabaseMysql.cpp
mysqlapi/Field.cpp
mysqlapi/QueryResult.cpp
mysqlapi/Row.cpp
)

set(mysqlmgr_srcs
mysqlmgr/MysqlManager.cpp
)

set(json_srcs
json/json.hcpp
)


set(chatserver_srcs
chatserversrc/main.cpp
chatserversrc/ChatServer.cpp
chatserversrc/ChatSession.cpp
chatserversrc/UserManager.cpp
# chatserversrc/MsgCacheManager.cpp
chatserversrc/TcpSession.cpp
# chatserversrc/MonitorSession.cpp
# chatserversrc/MonitorServer.cpp
# chatserversrc/HttpSession.cpp
# chatserversrc/HttpServer.cpp
# chatserversrc/BussinessLogic.cpp
)

# set(fileserver_srcs
# fileserversrc/main.cpp
# fileserversrc/FileServer.cpp
# fileserversrc/FileSession.cpp
# fileserversrc/FileManager.cpp
# fileserversrc/TcpSession.cpp)

# set(imgserver_srcs
# imgserversrc/main.cpp
# fileserversrc/FileServer.cpp
# fileserversrc/FileSession.cpp
# fileserversrc/FileManager.cpp
# fileserversrc/TcpSession.cpp)


add_executable(chatserver ${net_srcs} ${chatserver_srcs} ${mysqlapi_srcs} ${mysqlmgr_srcs})
# add_executable(chatserver ${net_srcs} ${json_srcs}  ${chatserver_srcs} ${mysqlapi_srcs} ${mysqlmgr_srcs} ${zlib_srcs} ${utils_srcs})
#光包含库目录是没用的，还必须使用TARGET_LINK_LIBRARIES链接该库
TARGET_INCLUDE_DIRECTORIES(chatserver PUBLIC json/)
TARGET_LINK_LIBRARIES(chatserver mysqlclient)

# <------------ add hiredis dependency --------------->
find_path(HIREDIS_HEADER hiredis PATHS /usr/include)
target_include_directories(chatserver PUBLIC ${HIREDIS_HEADER})

find_library(HIREDIS_LIB hiredis PATHS /usr/lib/aarch64-linux-gnu)
target_link_libraries(chatserver ${HIREDIS_LIB})

# <------------ add redis-plus-plus dependency -------------->
# NOTE: this should be *sw* NOT *redis++*
find_path(REDIS_PLUS_PLUS_HEADER sw PATHS include/)
target_include_directories(chatserver PUBLIC ${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++ PATHS /usr/lib/aarch64-linux-gnu/lib)
target_link_libraries(chatserver ${REDIS_PLUS_PLUS_LIB})
# add_executable(fileserver ${net_srcs}  ${fileserver_srcs} ${utils_srcs})
# TARGET_LINK_LIBRARIES(fileserver)

# add_executable(imgserver ${net_srcs}  ${imgserver_srcs} ${utils_srcs})
# TARGET_LINK_LIBRARIES(imgserver)
