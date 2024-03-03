/**
 *  消息缓存类， MsgCacheManager.cpp
 *  zhangyl 2017.03.16
 **/
#include "../base/AsyncLog.h"
#include "MsgCacheManager.h"

MsgCacheManager::MsgCacheManager()
{
}

MsgCacheManager::~MsgCacheManager()
{
}

void MsgCacheManager::init(const char *dbServer, const char *dbUserName, const char *dbPassword, const char *dbName, const char *redisUrl)
{
    m_strDbServer = dbServer;
    m_strDbUserName = dbUserName;
    if (dbPassword != NULL)
        m_strDbPassword = dbPassword;
    m_strDbName = dbName;
    m_strRedisUrl = redisUrl;
}

bool MsgCacheManager::loadMsgFromDbAndStoreInRedis()
{
    std::unique_ptr<CDatabaseMysql> pConn;
    pConn.reset(new CDatabaseMysql());
    if (!pConn->initialize(m_strDbServer, m_strDbUserName, m_strDbPassword, m_strDbName))
    {
        // LOGF("UserManager::LoadUsersFromDb failed, please check params: dbserver: %s, dbusername: %s, , dbpassword: %s, dbname: %s",
        //            m_strDbServer.c_str(), m_strDbUserName.c_str(), m_strDbPassword.c_str(), m_strDbName.c_str());
        printf("MsgCacheManager::loadMsgFromDbAndStoreInRedis() failed, please check params: dbserver: %s, dbusername: %s, , dbpassword: %s, dbname: %s",
               m_strDbServer.c_str(), m_strDbUserName.c_str(), m_strDbPassword.c_str(), m_strDbName.c_str());
        return false;
    }

    std::unique_ptr<QueryResult> res_ptr(pConn->query("SELECT t_chatmsg.f_msg_id, t_chatmsg.f_senderid, t_user.f_username, t_chatmsg.f_msgcontent, t_chatmsg.f_create_time FROM t_chatmsg JOIN t_user ON t_chatmsg.f_senderid = t_user.f_user_id"));
    if (nullptr == res_ptr)
    {
        // LOGI("UserManager::_Query error, dbname: %s", m_strDbName.c_str());
        printf("MsgCacheManager::loadMsgFromDbAndStoreInRedis() Query error");
        return false;
    }

    std::list<std::string> msgs;
    const std::vector<Row> &Rows = res_ptr->getRows();

    for (auto &Row : Rows)
    {
        ChatMsg msg;
        std::vector<std::string> strValues = Row.getStrValues();
        msg.msgid = convertFromString<int>(strValues[0]);
        msg.senderid = convertFromString<int>(strValues[1]);
        msg.senderusername = strValues[2];
        msg.msgcontent = strValues[3];
        msg.sendtime_str = strValues[4];
        
        using json = nlohmann::json; 
        json j;
        j["msgid"] = msg.msgid;
        j["senderid"] = msg.senderid;
        j["senderusername"] = msg.senderusername;
        j["msgcontent"] = msg.msgcontent;
        j["sendtime_str"] = msg.sendtime_str;
        msgs.push_back(j.dump());
        printf("msgid: %d, senderid: %d, senderusername: %s, msgcontent: %s, sendtime_str: %s\n", msg.msgid, msg.senderid, msg.senderusername.c_str(), msg.msgcontent.c_str(), msg.sendtime_str.c_str());
    }

    using namespace sw::redis;
    // auto redis = Redis("tcp://127.00.1:6379");
    auto redis = Redis(m_strRedisUrl);
    // 判断列表是否存在
    if (redis.exists("414chatmsgs"))
    {
        // 删除列表中的所有元素
        redis.del("414chatmsgs");
        std::cout << "414chatmsgs delete sucess\n"
                  << std::endl;
    }

    redis.rpush("414chatmsgs", msgs.begin(), msgs.end());

    return true;
}

bool MsgCacheManager::addChatMsgCache(int32_t userid, const std::string &cache)
{
    // std::lock_guard<std::mutex> guard(m_mtChatMsgCache);
    // ChatMsgCache c;
    // c.userid = userid;
    // c.chatmsg.append(cache.c_str(), cache.length());
    // m_listChatMsgCache.push_back(c);
    // LOGI("append chat msg to cache, userid: %d, m_listChatMsgCache.size(): , cache length: %d", userid, m_listChatMsgCache.size(), cache.length());
    // //TODO: 存盘或写入数据库以防止程序崩溃丢失

    return true;
}

// void MsgCacheManager::getChatMsgCache(int32_t userid, std::list<ChatMsgCache>& cached)
// {
//     // std::lock_guard<std::mutex> guard(m_mtChatMsgCache);
//     // for (auto iter = m_listChatMsgCache.begin(); iter != m_listChatMsgCache.end(); )
//     // {
//     //     if (iter->userid == userid)
//     //     {
//     //         cached.push_back(*iter);
//     //         iter = m_listChatMsgCache.erase(iter);
//     //     }
//     //     else
//     //     {
//     //         iter++;
//     //     }
//     // }

//     // LOGI("get chat msg cache, userid: %d, m_listChatMsgCache.size(): %d, cached size: %d", userid, m_listChatMsgCache.size(), cached.size());
// }