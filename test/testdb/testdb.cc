#include "../../mysqlapi/DatabaseMysql.h"
#include <iostream>

int main(int argc, char const *argv[])
{
    std::string host = "localhost";
    std::string username = "manjie";
    std::string passd = "123456";
    std::string dbname = "414chatroom";
    CDatabaseMysql cDatabaseMysql;
    if (cDatabaseMysql.initialize(host, username, passd, dbname))
    {
        std::cout << "数据库连接成功！！！" << std::endl;       
        QueryResult *res =  cDatabaseMysql.query("SELECT * FROM users;");
        res->printResult();
    }else
    {
        std::cout << "数据库连接失败！！！";
    }
    return 0;
}
