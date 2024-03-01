/**
 *  服务器主服务类，IMServer.cpp
 *  zhangyl 2017.03.09
 **/
#include "ChatServer.h"

#include "../net/InetAddress.h"
#include "../base/AsyncLog.h"
#include "../base/Singleton.h"
#include "ChatSession.h"
#include "UserManager.h"

ChatServer::ChatServer()
{
    m_logPackageBinary = false;
}

bool ChatServer::init(const char *ip, short port, EventLoop *loop)
{
    InetAddress addr(ip, port);
    m_server.reset(new TcpServer(loop, addr, "414CHATSERVER", TcpServer::kReusePort));
    m_server->setConnectionCallback(std::bind(&ChatServer::onConnected, this, std::placeholders::_1));
    // 启动侦听
    m_server->start(6);

    return true;
}

void ChatServer::uninit()
{
    if (m_server)
        m_server->stop();
}

void ChatServer::enableLogPackageBinary(bool enable)
{
    m_logPackageBinary = enable;
}

bool ChatServer::isLogPackageBinaryEnabled()
{
    return m_logPackageBinary;
}

void ChatServer::onConnected(std::shared_ptr<TcpConnection> conn)
{
    if (conn->connected())
    {
        // LOGD("client connected: %s", conn->peerAddress().toIpPort().c_str());
        printf("client connected: %s\n", conn->peerAddress().toIpPort().c_str());
        ++m_sessionId;
        std::shared_ptr<ChatSession> spSession(new ChatSession(conn, m_sessionId));
        conn->setMessageCallback(std::bind(&ChatSession::onRead, spSession.get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        std::lock_guard<std::mutex> guard(m_sessionMutex);
        m_sessions.push_back(spSession);
    }
    else
    {
        onDisconnected(conn);
    }
}

void ChatServer::onDisconnected(const std::shared_ptr<TcpConnection> &conn)
{
    // 是否有用户下线
    UserManager &userManager = Singleton<UserManager>::Instance();

    std::lock_guard<std::mutex> guard(m_sessionMutex);
    for (auto iter = m_sessions.begin(); iter != m_sessions.end();)
    {
        if ((*iter)->getConnectionPtr() == nullptr)
        {
            // LOGE("connection is NULL");
            printf("connection is nullptr\n");
        }

        // 通过比对connection对象找到对应的session
        if ((*iter)->getConnectionPtr() == conn)
        {
            // //该Session不是之前被踢下线的有效Session，才认为是正常下线，才给其好友推送其下线消息
            if ((*iter)->isSessionValid())
            {
                for (auto &ptr : m_sessions)
                {
                    // TODO 给其他连接发送该用户下线通知
                    if ((*iter)->getUserId() != ptr->getUserId())
                    {
                        // iter2->sendUserStatusChangeMsg(offlineUserId, 2);

                        // LOGI("SendUserStatusChangeMsg to user(userid=%d): user go offline, offline userid = %d", iter3->getUserId(), offlineUserId);
                        printf("SendUserStatusChangeMsg to user(userid=%d): user go offline, offline userid = %d\n", ptr->getUserId(), (*iter)->getUserId());
                    }
                }
            }
            else
            {
                // LOGI("Session is invalid, userid=%d", (*iter)->getUserId());
                printf("Session is invalid, userid=%d\n", (*iter)->getUserId());
            }

            // 停掉该Session的掉线检测
            //(*iter)->DisableHeartbaetCheck();
            // 用户下线
            iter = m_sessions.erase(iter);
            //  LOGI("client disconnected: %s", conn->peerAddress().toIpPort().c_str());
            printf("client disconnected: %s\n", conn->peerAddress().toIpPort().c_str());
            continue;
        }

        ++iter;
    }

    // LOGI("current online user count: %d", (int)m_sessions.size());
    printf("current online user count: %d\n", (int)m_sessions.size());
}

void ChatServer::getSessions(std::list<std::shared_ptr<ChatSession>> &sessions)
{
    std::lock_guard<std::mutex> guard(m_sessionMutex);
    sessions = m_sessions;
}

bool ChatServer::getSessionByUserId(std::shared_ptr<ChatSession> &session, int32_t userid)
{
    std::lock_guard<std::mutex> guard(m_sessionMutex);
    // std::shared_ptr<ChatSession> tmpSession;
    for (const auto &iter : m_sessions)
    {
        // tmpSession = iter;
        if (iter->getUserId() == userid)
        {
            // session = tmpSession;
            session = iter;
            return true;
        }
    }

    return false;
}

bool ChatServer::getSessionsByUserId(std::list<std::shared_ptr<ChatSession>> &sessions, int32_t userid)
{
    std::lock_guard<std::mutex> guard(m_sessionMutex);
    std::shared_ptr<ChatSession> tmpSession;
    for (const auto &iter : m_sessions)
    {
        tmpSession = iter;
        if (iter->getUserId() == userid)
        {
            sessions.push_back(tmpSession);
            return true;
        }
    }

    return false;
}

int32_t ChatServer::getUserStatusByUserId(int32_t userid)
{
    std::lock_guard<std::mutex> guard(m_sessionMutex);
    for (const auto &iter : m_sessions)
    {
        if (iter->getUserId() == userid)
        {
            return iter->getUserStatus();
        }
    }

    return 0;
}

int32_t ChatServer::getUserClientTypeByUserId(int32_t userid)
{
    std::lock_guard<std::mutex> guard(m_sessionMutex);
    bool bMobileOnline = false;
    int clientType = CLIENT_TYPE_UNKOWN;
    for (const auto &iter : m_sessions)
    {
        if (iter->getUserId() == userid)
        {
            clientType = iter->getUserClientType();
            // 电脑在线直接返回电脑在线状态
            if (clientType == CLIENT_TYPE_PC)
                return clientType;
            else if (clientType == CLIENT_TYPE_ANDROID || clientType == CLIENT_TYPE_IOS)
                bMobileOnline = true;
        }
    }

    // 只有手机在线才返回手机在线状态
    if (bMobileOnline)
        return clientType;

    return CLIENT_TYPE_UNKOWN;
}