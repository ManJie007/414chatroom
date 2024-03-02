/**
 * ChatSession.cpp
 * zhangyl, 2017.03.10
 **/
#include "ChatSession.h"
#include <string.h>
#include <sstream>
#include <list>
#include "../net/TcpConnection.h"
#include "../net/ProtocolStream.h"
#include "../base/AsyncLog.h"
#include "../base/Singleton.h"
#include "../json/json.hpp"
#include "Msg.h"
#include "UserManager.h"
#include "ChatServer.h"
#include "MsgCacheManager.h"

// 包最大字节数限制为10M
#define MAX_PACKAGE_SIZE 10 * 1024 * 1024

using namespace std;
using namespace net;

// 允许的最大时数据包来往间隔，这里设置成30秒
#define MAX_NO_PACKAGE_INTERVAL 30

ChatSession::ChatSession(const std::shared_ptr<TcpConnection> &conn, int sessionid) : TcpSession(conn),
                                                                                      m_id(sessionid),
                                                                                      m_seq(0),
                                                                                      m_isLogin(false)
{
    m_userinfo.userid = 0;
    m_lastPackageTime = time(NULL);

    // #ifndef _DEBUG
    // 暂且注释掉，不利于调试
    // EnableHearbeatCheck();
    // #endif
}

ChatSession::~ChatSession()
{
    std::shared_ptr<TcpConnection> conn = getConnectionPtr();
    if (conn)
    {
        // LOGI("remove check online timerId, userid: %d, clientType: %d, client address: %s", m_userinfo.userid, m_userinfo.clienttype, conn->peerAddress().toIpPort().c_str());
        printf("remove check online timerId, userid: %d, client address: %s\n", m_userinfo.userid, conn->peerAddress().toIpPort().c_str());
        // conn->getLoop()->remove(m_checkOnlineTimerId);
    }
}

void ChatSession::onRead(const std::shared_ptr<TcpConnection> &conn, ByteBuffer *pBuffer, Timestamp receivTime)
{
    while (true)
    {
        // 不够一个包头大小
        if (pBuffer->readableBytes() < (size_t)sizeof(chat_msg_header))
        {
            // LOGI << "buffer is not enough for a package header, pBuffer->readableBytes()=" << pBuffer->readableBytes() << ", sizeof(msg)=" << sizeof(msg);
            printf("buffer is not enough for a package header, pBuffer->readableBytes()=%ld\n", pBuffer->readableBytes());
            return;
        }

        // 取包头信息
        chat_msg_header header;
        memcpy(&header, pBuffer->peek(), sizeof(chat_msg_header));
        // 包头有错误，立即关闭连接
        if (header.originsize > MAX_PACKAGE_SIZE)
        {
            // 客户端发非法数据包，服务器主动关闭之
            // LOGE("Illegal package, compresssize: %lld, originsize: %lld, close TcpConnection, client: %s", header.compresssize, header.originsize, conn->peerAddress().toIpPort().c_str());
            printf("Illegal package, originsize: %d, close TcpConnection, client: %s\n", header.originsize, conn->peerAddress().toIpPort().c_str());
            conn->forceClose();
            return;
        }

        // 收到的数据不够一个完整的包
        if (pBuffer->readableBytes() < (size_t)header.originsize + sizeof(chat_msg_header))
            return;

        pBuffer->retrieve(sizeof(chat_msg_header));
        std::string inbuf;
        inbuf.append(pBuffer->peek(), header.originsize);
        pBuffer->retrieve(header.originsize);
        if (!process(conn, inbuf.c_str(), inbuf.length()))
        {
            // 客户端发非法数据包，服务器主动关闭之
            // LOGE("Process error, close TcpConnection, client: %s", conn->peerAddress().toIpPort().c_str());
            printf("Process error, close TcpConnection, client: %s", conn->peerAddress().toIpPort().c_str());
            conn->forceClose();
            return;
        }

        m_lastPackageTime = time(NULL);

    } // end while-loop
}

bool ChatSession::process(const std::shared_ptr<TcpConnection> &conn, const char *inbuf, size_t buflength)
{
    BinaryStreamReader readStream(inbuf, buflength);
    int32_t cmd;
    if (!readStream.ReadInt32(cmd))
    {
        // LOGE("read cmd error, client: %s", conn->peerAddress().toIpPort().c_str());
        printf("read cmd error, client: %s\n", conn->peerAddress().toIpPort().c_str());
        return false;
    }

    // int seq;
    if (!readStream.ReadInt32(m_seq))
    {
        // LOGE("read seq error, client: %s", conn->peerAddress().toIpPort().c_str());
        printf("read seq error, client: %s", conn->peerAddress().toIpPort().c_str());
        return false;
    }

    std::string data;
    size_t datalength;
    if (!readStream.ReadString(&data, 0, datalength))
    {
        // LOGE("read data error, client: %s", conn->peerAddress().toIpPort().c_str());
        printf("read data error, client: %s\n", conn->peerAddress().toIpPort().c_str());
        return false;
    }

    // TODO 心跳功能
    // // 心跳包太频繁，不打印
    // if (cmd != msg_type_heartbeat)
    //     LOGI("Request from client: userid=%d, cmd=%d, seq=%d, data=%s, datalength=%d, buflength=%d", m_userinfo.userid, cmd, m_seq, data.c_str(), datalength, buflength);

    if (Singleton<ChatServer>::Instance().isLogPackageBinaryEnabled())
    {
        // LOGI("body stream, buflength: %d, client: %s", buflength, conn->peerAddress().toIpPort().c_str());
        printf("body stream, buflength: %ld, client: %s\n", buflength, conn->peerAddress().toIpPort().c_str());
        // LOG_DEBUG_BIN((unsigned char*)inbuf, buflength);
    }

    switch (cmd)
    {
    // TODO
    // // 心跳包
    // case msg_type_heartbeat:
    //     onHeartbeatResponse(conn);
    //     break;

    // // 注册
    // case msg_type_register:
    //     onRegisterResponse(data, conn);
    //     break;

    // 登录
    case msg_type_login:
        onLoginResponse(data, conn);
        break;

    // 其他命令必须在已经登录的前提下才能进行操作
    default:
    {
        if (m_isLogin)
        {
            switch (cmd)
            {
            // 获取好友列表
            case msg_type_getuserlist:
                onGetUserListResponse(conn);
                break;

            // 聊天消息
            case msg_type_chat:
                int32_t target;
                if (!readStream.ReadInt32(target))
                {
                    LOGE("read target error, client: %s", conn->peerAddress().toIpPort().c_str());
                    return false;
                }
                onChatResponse(target, data, conn);
                break;

            default:
                // pBuffer->retrieveAll();
                // LOGE("unsupport cmd, cmd: %d, data=%s, connection name:", cmd, data.c_str(), conn->peerAddress().toIpPort().c_str());
                printf("unsupport cmd, cmd: %d, data=%s, connection name:%s\n", cmd, data.c_str(), conn->peerAddress().toIpPort().c_str());
                // conn->forceClose();
                return false;
            } // end inner-switch
        }
        else
        {
            // 用户未登录，告诉客户端不能进行操作提示“未登录”
            std::string data = "{\"code\": 2, \"msg\": \"not login, please login first!\"}";
            send(cmd, m_seq, data);
            // LOGI("Response to client: cmd=%d, , data=%s, , sessionId=%d", cmd, data.c_str(), m_id);
            printf("Response to client: cmd=%d, , data=%s, , sessionId=%d\n", cmd, data.c_str(), m_id);
        } // end if
    }     // end default
    }     // end outer-switch

    ++m_seq;

    return true;
}

// void ChatSession::onHeartbeatResponse(const std::shared_ptr<TcpConnection> &conn)
// {
//     std::string dummydata;
//     send(msg_type_heartbeat, m_seq, dummydata);

//     // 心跳包日志就不要打印了，很容易写满日志
//     // LOGI << "Response to client: cmd=1000" << ", sessionId=" << m_id;
// }

void ChatSession::onRegisterResponse(const std::string &data, const std::shared_ptr<TcpConnection> &conn)
{
    // string retData;
    // BussinessLogic::registerUser(data, conn, true, retData);

    // if (!retData.empty())
    // {
    //     send(msg_type_register, m_seq, retData);

    //     LOGI("Response to client: cmd=msg_type_register, data: %s. client: %s", retData.c_str(), conn->peerAddress().toIpPort().c_str());
    // }
}

void ChatSession::onLoginResponse(const std::string &data, const std::shared_ptr<TcpConnection> &conn)
{
    //{"username": "13917043329", "password": "123", "clienttype": 1, "status": 1}
    // {"username": "13917043329", "password": "123"}
    using json = nlohmann::json;
    auto j = json::parse(data);

    // TODO 容错处理
    string username = j["username"];
    string password = j["password"];
    std::ostringstream os;
    User cachedUser;
    cachedUser.userid = 0;
    Singleton<UserManager>::Instance().getUserInfoByUsername(username, cachedUser);
    ChatServer &imserver = Singleton<ChatServer>::Instance();

    if (cachedUser.password != password)
        os << "{\"code\": 1, \"msg\": \"incorrect password\"}";
    else
    {
        // 如果该账号已经登录，则将前一个账号踢下线
        std::shared_ptr<ChatSession> targetSession;
        imserver.getSessionByUserId(targetSession, cachedUser.userid);
        if (targetSession)
        {
            string dummydata;
            targetSession->send(msg_type_kickuser, m_seq, dummydata);
            // 被踢下线的Session标记为无效的
            targetSession->makeSessionInvalid();

            LOGI("Response to client, userid: %d, cmd=msg_type_kickuser", targetSession->getUserId());

            // 关闭连接
            // targetSession->GetConnectionPtr()->forceClose();
        }

        // 记录用户信息
        m_userinfo.userid = cachedUser.userid;
        m_userinfo.username = username;

        os << "{\"code\": 0, \"msg\": \"ok\", \"userid\": " << m_userinfo.userid << ",\"username\":\"" << cachedUser.username << "\"}";
    }

    // 登录信息应答
    send(msg_type_login, m_seq, os.str());

    // LOGI("Response to client: cmd=msg_type_login, data=%s, userid=", os.str().c_str(), m_userinfo.userid);
    printf("Response to client: cmd=msg_type_login, data=%s, userid=%d\n", os.str().c_str(), m_userinfo.userid);

    // 设置已经登录的标志
    m_isLogin = true;

    // 推送离线聊天消息
    // std::list<ChatMsgCache> listChatCache;
    // Singleton<MsgCacheManager>::Instance().getChatMsgCache(m_userinfo.userid, listChatCache);
    // for (const auto &iter : listChatCache)
    // {
    //     send(iter.chatmsg);
    // }

    // // 给其他用户推送上线消息
    // std::list<User> friends;
    // Singleton<UserManager>::Instance().getFriendInfoByUserId(m_userinfo.userid, friends);
    // for (const auto &iter : friends)
    // {
    //     // 因为存在一个用户id，多个终端，所以，同一个userid可能对应多个session
    //     std::list<std::shared_ptr<ChatSession>> sessions;
    //     imserver.getSessionsByUserId(sessions, iter.userid);
    //     for (auto &iter2 : sessions)
    //     {
    //         if (iter2)
    //         {
    //             iter2->sendUserStatusChangeMsg(m_userinfo.userid, 1, m_userinfo.status);

    //             LOGI("sendUserStatusChangeMsg to user(userid: %d): user go online, online userid: %d, status: %d", iter2->getUserId(), m_userinfo.userid, m_userinfo.status);
    //         }
    //     }
    // }
}

void ChatSession::onGetUserListResponse(const std::shared_ptr<TcpConnection> &conn)
{
    std::string userlist;
    makeUpUserListInfo(userlist, conn);
    std::ostringstream os;
    os << "{\"code\": 0, \"msg\": \"ok\", \"userlistinfo\":" << userlist << "}";
    send(msg_type_getuserlist, m_seq, os.str());

    // LOGI("Response to client: userid: %d, cmd=msg_type_getofriendlist, data: %s", m_userinfo.userid, os.str().c_str());
    printf("Response to client: userid: %d, cmd=msg_type_getofriendlist, data: %s\n", m_userinfo.userid, os.str().c_str());
}

void ChatSession::sendUserStatusChangeMsg(int32_t userid, int type, int status /* = 0*/)
{
    // string data;
    // //用户上线
    // if (type == 1)
    // {
    //     int32_t clientType = Singleton<ChatServer>::Instance().getUserClientTypeByUserId(userid);
    //     char szData[64];
    //     memset(szData, 0, sizeof(szData));
    //     sprintf(szData, "{ \"type\": 1, \"onlinestatus\": %d, \"clienttype\": %d}", status, clientType);
    //     data = szData;
    // }
    // //用户下线
    // else if (type == 2)
    // {
    //     data = "{\"type\": 2, \"onlinestatus\": 0}";
    // }
    // //个人昵称、头像、签名等信息更改
    // else if (type == 3)
    // {
    //     data = "{\"type\": 3}";
    // }

    // std::string outbuf;
    // BinaryStreamWriter writeStream(&outbuf);
    // writeStream.WriteInt32(msg_type_userstatuschange);
    // writeStream.WriteInt32(m_seq);
    // writeStream.WriteString(data);
    // writeStream.WriteInt32(userid);
    // writeStream.Flush();

    // send(outbuf);

    // LOGI("send to client: userid: %d, cmd=msg_type_userstatuschange, data: %s", m_userinfo.userid, data.c_str());
}

void ChatSession::makeSessionInvalid()
{
    m_userinfo.userid = 0;
}

bool ChatSession::isSessionValid()
{
    return m_userinfo.userid > 0;
}

void ChatSession::onChatResponse(int32_t targetid, const std::string &data, const std::shared_ptr<TcpConnection> &conn)
{
    // std::string modifiedChatData;
    // if (!modifyChatMsgLocalTimeToServerTime(data, modifiedChatData))
    // {
    //     LOGE("invalid chat json, chatjson: %s, senderid: %d, targetid: %d, chatmsg: %s, client: %s", data.c_str(), m_userinfo.userid, targetid, data.c_str(), conn->peerAddress().toIpPort().c_str());
    //     return;
    // }

    // std::string outbuf;
    // BinaryStreamWriter writeStream(&outbuf);
    // writeStream.WriteInt32(msg_type_chat);
    // writeStream.WriteInt32(m_seq);
    // writeStream.WriteString(modifiedChatData);
    // // 消息发送者
    // writeStream.WriteInt32(m_userinfo.userid);
    // // 消息接受者
    // writeStream.WriteInt32(targetid);
    // writeStream.Flush();

    // UserManager &userMgr = Singleton<UserManager>::Instance();
    // // 写入消息记录
    // if (!userMgr.saveChatMsgToDb(m_userinfo.userid, targetid, data))
    // {
    //     LOGE("Write chat msg to db error, senderid: %d, targetid: %d, chatmsg: %s, client: %s", m_userinfo.userid, targetid, data.c_str(), conn->peerAddress().toIpPort().c_str());
    // }

    // ChatServer &imserver = Singleton<ChatServer>::Instance();
    // MsgCacheManager &msgCacheMgr = Singleton<MsgCacheManager>::Instance();
    // // 单聊消息
    // if (targetid < GROUPID_BOUBDARY)
    // {
    //     // 先看目标用户是否在线
    //     std::list<std::shared_ptr<ChatSession>> targetSessions;
    //     imserver.getSessionsByUserId(targetSessions, targetid);
    //     // 目标用户不在线，缓存这个消息
    //     if (targetSessions.empty())
    //     {
    //         msgCacheMgr.addChatMsgCache(targetid, outbuf);
    //     }
    //     else
    //     {
    //         for (auto &iter : targetSessions)
    //         {
    //             if (iter)
    //                 iter->send(outbuf);
    //         }
    //     }
    // }
    // // 群聊消息
    // else
    // {
    //     std::list<User> friends;
    //     userMgr.getFriendInfoByUserId(targetid, friends);
    //     std::string strUserInfo;
    //     bool useronline = false;
    //     for (const auto &iter : friends)
    //     {
    //         // 排除群成员中的自己
    //         if (iter.userid == m_userinfo.userid)
    //             continue;

    //         // 先看目标用户是否在线
    //         std::list<std::shared_ptr<ChatSession>> targetSessions;
    //         imserver.getSessionsByUserId(targetSessions, iter.userid);
    //         // 目标用户不在线，缓存这个消息
    //         if (targetSessions.empty())
    //         {
    //             msgCacheMgr.addChatMsgCache(iter.userid, outbuf);
    //             continue;
    //         }
    //         else
    //         {
    //             for (auto &iter2 : targetSessions)
    //             {
    //                 if (iter2)
    //                     iter2->send(outbuf);
    //             }
    //         }
    //     }
    // }
}

void ChatSession::makeUpUserListInfo(std::string &listInfo, const std::shared_ptr<TcpConnection> &conn)
{
    UserManager& userManager = Singleton<UserManager>::Instance();
    list<User> users = userManager.getUserListInfo();

    /*
    {
    "userListInfo": [
    {
    "userid": 1,
    "username": "张xx"
    },
    {
    "userid": 2,
    "username": "张xx"
    }
    ]
    }
    */

    using json = nlohmann::json;
    json user_array;

    for (auto &usr : users)
    {
        json u;
        u["userid"] = usr.userid;
        u["username"] = usr.username;
        user_array.push_back(u);
    }

    listInfo = user_array.dump();
}

bool ChatSession::modifyChatMsgLocalTimeToServerTime(const std::string &chatInputJson, std::string &chatOutputJson)
{
    // /*
    //     消息格式：
    //     {
    //         "msgType": 1, //消息类型 0未知类型 1文本 2窗口抖动 3文件
    //         "time": 2434167,
    //         "clientType": 0,		//0未知 1pc端 2苹果端 3安卓端
    //         "font":["fontname", fontSize, fontColor, fontBold, fontItalic, fontUnderline],
    //         "content":
    //         [
    //             {"msgText": "text1"},
    //             {"msgText": "text2"},
    //             {"faceID": 101},
    //             {"faceID": 102},
    //             {"pic": ["name", "server_path", 400, w, h]},
    //             {"remotedesktop": 1},
    //             {"shake": 1},
    //             {"file":["name", "server_path", 400, onlineflag]}		//onlineflag为0是离线文件，不为0为在线文件
    //         ]
    //     }
    // */
    // if (chatInputJson.empty())
    //     return false;

    // Json::CharReaderBuilder b;
    // Json::CharReader *reader(b.newCharReader());
    // Json::Value jsonRoot;
    // JSONCPP_STRING errs;
    // bool ok = reader->parse(chatInputJson.c_str(), chatInputJson.c_str() + chatInputJson.length(), &jsonRoot, &errs);
    // if (!ok || errs.size() != 0)
    // {
    //     LOGE("parse chatInputJson json failed, userid: %d, chatInputJson: %s", m_userinfo.userid, chatInputJson.c_str());
    //     delete reader;
    //     return false;
    // }
    // delete reader;

    // unsigned int now = (unsigned int)time(NULL);
    // // if (JsonRoot["time"].isNull())
    // jsonRoot["time"] = now;

    // // Json::FastWriter writer;
    // // chatOutputJson = writer.write(JsonRoot);
    // Json::StreamWriterBuilder streamWriterBuilder;
    // // 消除json中的\t和\n符号
    // streamWriterBuilder.settings_["indentation"] = "";
    // chatOutputJson = Json::writeString(streamWriterBuilder, jsonRoot);

    return true;
}

// void ChatSession::enableHearbeatCheck()
// {
//     std::shared_ptr<TcpConnection> conn = getConnectionPtr();
//     if (conn)
//     {
//         // 每15秒钟检测一下是否有掉线现象
//         m_checkOnlineTimerId = conn->getLoop()->runEvery(15000000, std::bind(&ChatSession::checkHeartbeat, this, conn));
//     }
// }

// void ChatSession::disableHeartbeatCheck()
// {
//     std::shared_ptr<TcpConnection> conn = getConnectionPtr();
//     if (conn)
//     {
//         LOGI("remove check online timerId, userid: %d, clientType: %d, client address: %s", m_userinfo.userid, m_userinfo.clienttype, conn->peerAddress().toIpPort().c_str());
//         conn->getLoop()->cancel(m_checkOnlineTimerId, true);
//     }
// }

// void ChatSession::checkHeartbeat(const std::shared_ptr<TcpConnection> &conn)
// {
//     if (!conn)
//         return;

//     // LOGI("check heartbeat, userid: %d, clientType: %d, client address: %s", m_userinfo.userid, m_userinfo.clienttype, conn->peerAddress().toIpPort().c_str());

//     if (time(NULL) - m_lastPackageTime < MAX_NO_PACKAGE_INTERVAL)
//         return;

//     conn->forceClose();
//     // LOGI("in max no-package time, no package, close the connection, userid: %d, clientType: %d, client address: %s", m_userinfo.userid, m_userinfo.clienttype, conn->peerAddress().toIpPort().c_str());
// }