#include "MysqlManager.h"
#include <sstream>
#include "../base/AsyncLog.h"
#include "../base/Singleton.h"

CMysqlManager::CMysqlManager()
{
}

CMysqlManager::~CMysqlManager()
{
}

bool CMysqlManager::init(const char *host, const char *user, const char *pwd, const char *dbname)
{
	m_strHost = host;
	m_strUser = user;
	// 数据库密码可能为空
	if (pwd != NULL)
		m_strPassword = pwd;
	m_strDataBase = dbname;

	// 注意：检查数据库是否存在时，需要将数据库名称设置为空
	m_poConn.reset(new CDatabaseMysql());
	if (!m_poConn->initialize(m_strHost, m_strUser, m_strPassword, ""))
	{
		// LOG_FATAL << "CMysqlManager::Init failed, please check params(" << m_strHost << ", " << m_strUser << ", " << m_strPassword << ")";
		return false;
	}

	////////////////////// 1. 检查库是否存在 /////////////////////////
	if (!isDBExist())
	{
		if (!createDB())
		{
			return false;
		}
	}

	// 再次确定是否可以连接上数据库
	m_poConn.reset(new CDatabaseMysql());
	if (!m_poConn->initialize(m_strHost, m_strUser, m_strPassword, m_strDataBase))
	{
		// LOG_FATAL << "CMysqlManager::Init failed, please check params(" << m_strHost << ", " << m_strUser
		//	<< ", " << m_strPassword << ", " << m_strDataBase << ")";
		return false;
	}

	////////////////////// 2. 初始化数据库 /////////////////////////
	if (!initData("../etc/Data.sql"))
	// if (!initData("Data.sql"))
	{
		return false;
	}
	
	m_poConn.reset();
	return true;
}

bool CMysqlManager::isDBExist()
{
	if (NULL == m_poConn)
	{
		return false;
	}

	std::unique_ptr<QueryResult> res_ptr(m_poConn->query("SHOW DATABASES;"));
	if (nullptr == res_ptr)
	{
		// LOGI << "CMysqlManager::_IsDBExist, no database(" << m_strDataBase << ")";
		return false;
	}

	const std::vector<Row> &Rows = res_ptr->getRows();

	for (auto &Row : Rows)
	{
		for (auto &str : Row.getStrValues())
		{
			if (m_strDataBase == str)
			{
				// std::cout << "数据库存在!!!" << std::endl;
				return true;
			}
		}
	}

	return false;
}

bool CMysqlManager::createDB()
{
	if (nullptr == m_poConn)
	{
		return false;
	}

	int nErrno = 0;

	std::stringstream ss;
	ss << "CREATE DATABASE " << m_strDataBase << ";";
	if (m_poConn->execute(ss.str().c_str(), nErrno))
	{
		if (isDBExist())
		{
			// LOGI << "CMysqlManager::_CreateDB, create database " <<
			//	m_strDataBase << " success";
			// std::cout << "创建数据库成功！！！" << std::endl;
			return true;
		}
	}
	else
	{
		// LOGE << "CMysqlManager::_CreateDB, create database " << m_strDataBase << " failed("
		//	<< nErrno << ")";
		return false;
	}
	return false;
}

bool CMysqlManager::initData(const char *filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Error: Unable to open file " << filename << std::endl;
		return false;
	}

	std::string query;
	std::string line;
	while (std::getline(file, line))
	{
		query += line;
		if (line.back() == ';')
		{
			if (!m_poConn->execute(query.c_str()))
			{
				// std::cerr << "Error: Query execution failed: " << mysql_error(connection) << std::endl;
				file.close();
				return false;
			}
			query.clear();
		}
	}

	file.close();
	return true;
}
