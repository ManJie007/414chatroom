/** 
 *  管理所有的用户信息，初始信息从数据库中加载, UserManager.h
 **/

#pragma once
#include <stdint.h>
#include <string>
#include <list>
#include <mutex>
#include <set>
#include <atomic>

using namespace std;

enum FRIEND_OPERATION
{
    FRIEND_OPERATION_ADD,
    FRIEND_OPERATION_DELETE
};

//用户或者群
struct User
{
    int32_t        userid;
    string         username;
    string         password;
};

class UserManager final
{
public:
    UserManager();
    ~UserManager();

    bool init(const char* dbServer, const char* dbUserName, const char* dbPassword, const char* dbName);

    UserManager(const UserManager& rhs) = delete;
    UserManager& operator=(const UserManager& rhs) = delete;

    bool addUser(User& u);
    bool makeFriendRelationshipInDB(int32_t smallUserid, int32_t greaterUserid);
    bool releaseFriendRelationshipInDBAndMemory(int32_t smallUserid, int32_t greaterUserid);
    bool updateUserRelationshipInMemory(int32_t userid, int32_t target, FRIEND_OPERATION operation);
    bool addFriendToUser(int32_t userid, int32_t friendid);
    bool deleteFriendToUser(int32_t userid, int32_t friendid);

    bool isFriend(int32_t userid, int32_t friendid);
    
    //TODO: 这类都是更新用户信息的接口，将来可以考虑统一起来
    bool updateUserInfoInDb(int32_t userid, const User& newuserinfo);
    bool modifyUserPassword(int32_t userid, const std::string& newpassword);
    //将内存和数据库中的某个用户的分组信息改成新的newteaminfo
    bool updateUserTeamInfoInDbAndMemory(int32_t userid, const std::string& newteaminfo);
    bool deleteTeam(int32_t userid, const std::string& deletedteamname);
    bool modifyTeamName(int32_t userid, const std::string& newteamname, const std::string& oldteamname);
    
    //更新用户好友备注名
    bool updateMarknameInDb(int32_t userid, int32_t friendid, const std::string& newmarkname);
    //移动好友至其他分组
    bool moveFriendToOtherTeam(int32_t userid, int32_t friendid, const std::string& newteamname);

    bool addGroup(const char* groupname, int32_t ownerid, int32_t& groupid);

    //聊天消息入库
    bool saveChatMsgToDb(int32_t senderid, int32_t targetid, const std::string& chatmsg);

    //TODO: 当用户越来越多，并发量越来越多的时候，这个系列的函数效率高吗？
    bool getUserInfoByUsername(const std::string& username, User& u);
    bool getUserInfoByUserId(int32_t userid, User& u);
    bool getUserInfoByUserId(int32_t userid, User*& u);
    bool getFriendInfoByUserId(int32_t userid, std::list<User>& friends);
    //获取好友的备注名
    bool getFriendMarknameByUserId(int32_t userid1, int32_t friendid, std::string& markname);
    bool getTeamInfoByUserId(int32_t userid, std::string& teaminfo);
    

    list<User> getUserListInfo();

private:
    bool loadUsersFromDb();

private:
    list<User>          m_allCachedUsers;
    mutex               m_mutex;

    string              m_strDbServer;
    string              m_strDbUserName;
    string              m_strDbPassword;
    string              m_strDbName;
};