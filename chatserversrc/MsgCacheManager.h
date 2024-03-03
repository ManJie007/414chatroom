/** 
 *  消息缓存类， MsgCacheManager.h
 *  zhangyl 2017.03.16
 **/
#pragma once
#include <list>
#include <stdint.h>
#include <string>
#include <mutex>
#include <memory>
#include <list>
#include "sw/redis++/redis++.h"
#include "../mysqlapi/DatabaseMysql.h"
#include "../utils/ConvertStringUtil.h"
#include "../json/json.hpp"


struct ChatMsg
{
    int32_t     msgid;
    int32_t senderid;
    std::string senderusername;
    std::string msgcontent;
    std::string sendtime_str;
};

class MsgCacheManager final
{
public:
    MsgCacheManager();
    ~MsgCacheManager();

    MsgCacheManager(const MsgCacheManager& rhs) = delete;
    MsgCacheManager& operator =(const MsgCacheManager& rhs) = delete;

    bool init(const char* dbServer, const char* dbUserName, const char* dbPassword, const char* dbName, const char *redisUrl);
    bool loadMsgFromDbAndStoreInRedis();

    std::list<std::string> loadMsgsFromRedis();

    bool addChatMsgCache(int32_t userid, const std::string& cache);
    // void getChatMsgCache(int32_t userid, std::list<ChatMsgCache>& cached);


private:
    std::string              m_strDbServer;
    std::string              m_strDbUserName;
    std::string              m_strDbPassword;
    std::string              m_strDbName;
    std::string              m_strRedisUrl;

    // std::list<ChatMsg>         m_listChatMsgCache;      //聊天消息缓存
    // std::mutex                      m_mtChatMsgCache;
};
