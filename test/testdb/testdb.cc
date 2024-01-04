#include "../../mysqlapi/DatabaseMysql.h"
#include "../../mysqlmgr/MysqlManager.h"
#include <iostream>

int main(int argc, char const *argv[])
{
    std::string host = "localhost";
    std::string username = "manjie";
    std::string passd = "123456";
    std::string dbname = "414chatroom";
    // CDatabaseMysql cDatabaseMysql;
    // if (cDatabaseMysql.initialize(host, username, passd, dbname))
    // {
    //     std::cout << "数据库连接成功！！！" << std::endl;       
    //     // QueryResult *res =  cDatabaseMysql.query("SELECT * FROM users;");
    //     QueryResult *res =  cDatabaseMysql.query("SHOW DATABASES;");
    //     res->printResult();
    // }else
    // {
    //     std::cout << "数据库连接失败！！！";
    // }
    CMysqlManager manager;
    manager.init(host.c_str(), username.c_str(), passd.c_str(), dbname.c_str());

    return 0;
}
