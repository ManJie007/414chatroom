#include "../../mysqlapi/DatabaseMysql.h"
#include "../../mysqlmgr/MysqlManager.h"
#include <iostream>
#include <sstream>

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
    // CMysqlManager manager;
    // manager.init(host.c_str(), username.c_str(), passd.c_str(), dbname.c_str());

    std::shared_ptr<CDatabaseMysql> m_poConn;
    m_poConn.reset(new CDatabaseMysql());
    if (!m_poConn->initialize(host, username, passd, ""))
    {
        std::cout << "initialize()函数执行失败..." << std::endl;
        return -1;
    }
    std::cout << "initialize()函数执行成功..." << std::endl;

    // isDBExist()
    bool exist = false;
    std::unique_ptr<QueryResult> res_ptr(m_poConn->query("SHOW DATABASES;"));
    if (nullptr == res_ptr)
    {
        std::cout << "SHOW DATABASES失败..." << std::endl;
        return -1;
    }

    res_ptr->printResult(std::cout);

    const std::vector<Row> &Rows = res_ptr->getRows();

    for (auto &Row : Rows)
    {
        for (auto &str : Row.getStrValues())
        {
            if (dbname == str)
            {
                std::cout << "数据库存在!!!" << std::endl;
                exist = true;
            }
        }
    }

    if (!exist)
    {
        std::cout << "数据库不存在!!!" << std::endl;
        // createDB()
        int nErrno = 0;

        std::stringstream ss;
        ss << "CREATE DATABASE " << dbname << ";";
        if (m_poConn->execute(ss.str().c_str(), nErrno))
        {
            bool exist2 = false;
            std::unique_ptr<QueryResult> res_ptr(m_poConn->query("SHOW DATABASES;"));
            if (nullptr == res_ptr)
            {
                std::cout << "SHOW DATABASES失败..." << std::endl;
                return -1;
            }

            res_ptr->printResult(std::cout);

            const std::vector<Row> &Rows = res_ptr->getRows();

            for (auto &Row : Rows)
            {
                for (auto &str : Row.getStrValues())
                {
                    if (dbname == str)
                    {
                        std::cout << "数据库存在!!!" << std::endl;
                        exist2 = true;
                    }
                }
            }
            if (exist2)
            {
                // LOGI << "CMysqlManager::_CreateDB, create database " <<
                //	m_strDataBase << " success";
                std::cout << "创建数据库成功！！！" << std::endl;
            }
            else
            {
                std::cout << "创建数据库失败！！！" << std::endl;
            }
        }
        // else
        // {
        //     // LOGE << "CMysqlManager::_CreateDB, create database " << m_strDataBase << " failed("
        //     //	<< nErrno << ")";
        //     return false;
        // }
        // return false;
    }

    // 再次确定是否可以连接上数据库
    m_poConn.reset(new CDatabaseMysql());
    if (!m_poConn->initialize(host, username, passd, dbname))
    {
        // LOG_FATAL << "CMysqlManager::Init failed, please check params(" << m_strHost << ", " << m_strUser
        //	<< ", " << m_strPassword << ", " << m_strDataBase << ")";

        std::cout << "连接数据库失败！！！" << std::endl;
        return -1;
    }
    std::cout << "连接数据库成功！！！" << std::endl;

    ////////////////////// 2. 初始化数据库 /////////////////////////
    std::ifstream file("Data.sql");
    if (!file.is_open())
    {
        std::cout << "Error: Unable to open file Data.sql"
                  << std::endl;
        return false;
    }

    std::string query;
    std::string line;
    while (std::getline(file, line))
    {
        query += line;
        if (line.back() == ';')
        {
            std::cout << query << std::endl;
            if (!m_poConn->execute(query.c_str()))
            {
                // std::cerr << "Error: Query execution failed: " << mysql_error(connection) << std::endl;
                file.close();
                std::cout << "Error: Unable to execute file Data.sql"
                          << std::endl;
                return -1;
            }
            query.clear();
        }
    }

    file.close();

    std::unique_ptr<QueryResult> res_ptr_2(m_poConn->query("SELECT * FROM t_user;"));
    if (nullptr == res_ptr)
    {
        std::cout << "SELECT * FROM t_user;失败..." << std::endl;
        return -1;
    }

    res_ptr_2->printResult(std::cout);

    m_poConn.reset();

    return 0;
}
