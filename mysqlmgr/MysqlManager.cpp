#include "MysqlManager.h"
#include <sstream>
#include "../base/AsyncLog.h"
#include "../base/Singleton.h"

CMysqlManager::CMysqlManager()
{
	m_strCharactSet = "utf8";

	// 先做一个简单的聊天室软件
	//  初始化表
	//  1. t_user
	{
		STableInfo info;
		// name type desc?
		info.m_strName = "t_user";
		info.m_mapField["f_id"] = {"f_id", "bigint(20) NOT NULL AUTO_INCREMENT COMMENT '自增ID'", "bigint(20)"};
		info.m_mapField["f_user_id"] = {"f_user_id", "bigint(20) NOT NULL COMMENT '用户ID'", "bigint(20)"};
		info.m_mapField["f_username"] = {"f_username", "varchar(64) NOT NULL COMMENT '用户名'", "varchar(64)"};
		// info.m_mapField["f_nickname"] = {"f_nickname", "varchar(64) NOT NULL COMMENT '用户昵称'", "varchar(64)"};
		info.m_mapField["f_password"] = {"f_password", "varchar(64) NOT NULL COMMENT '用户密码'", "varchar(64)"};
		// info.m_mapField["f_facetype"] = {"f_facetype", "int(10) DEFAULT 0 COMMENT '用户头像类型'", "int(10)"};
		// info.m_mapField["f_customface"] = {"f_customface", "varchar(32) DEFAULT NULL COMMENT '自定义头像名'", "varchar(32)"};
		// info.m_mapField["f_customfacefmt"] = {"f_customfacefmt", "varchar(6) DEFAULT NULL COMMENT '自定义头像格式'", "varchar(6)"};
		// info.m_mapField["f_gender"] = {"f_gender", "int(2)  DEFAULT 0 COMMENT '性别'", "int(2)"};
		// info.m_mapField["f_birthday"] = {"f_birthday", "bigint(20)  DEFAULT 19900101 COMMENT '生日'", "bigint(20)"};
		// info.m_mapField["f_signature"] = {"f_signature", "varchar(256) DEFAULT NULL COMMENT '地址'", "varchar(256)"};
		// info.m_mapField["f_address"] = {"f_address", "varchar(256) DEFAULT NULL COMMENT '地址'", "varchar(256)"};
		// info.m_mapField["f_phonenumber"] = {"f_phonenumber", "varchar(64) DEFAULT NULL COMMENT '电话'", "varchar(64)"};
		// info.m_mapField["f_mail"] = {"f_mail", "varchar(256) DEFAULT NULL COMMENT '邮箱'", "varchar(256)"};
		// info.m_mapField["f_owner_id"] = {"f_owner_id", "bigint(20) DEFAULT 0 COMMENT '群账号群主userid'", "bigint(20)"};
		info.m_mapField["f_teaminfo"] = {"f_teaminfo", "blob default null comment '好友分组信息'", "blob"};
		// info.m_mapField["f_register_time"] = {"f_register_time", "datetime NOT NULL COMMENT '注册时间'", "datetime"};
		info.m_mapField["f_remark"] = {"f_remark", "varchar(64) NULL COMMENT '备注'", "varchar(64)"};
		info.m_mapField["f_update_time"] = {"f_update_time", "timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间'", "timestamp"};

		info.m_strKeyString = "PRIMARY KEY (f_user_id), INDEX f_user_id (f_user_id), KEY  f_id  ( f_id )";
		m_vecTableInfo.push_back(info);

		// info.m_mapField["AccountID"] = { "AccountID", "integer NOT NULL AUTO_INCREMENT PRIMARY KEY", "int(11)" };
		// info.m_mapField["MobilePhone"] = { "MobilePhone", "bigint NOT NULL", "bigint(20)" };
		// info.m_mapField["NickName"] = { "NickName", "char(65) not null", "char(65)" };
		// info.m_mapField["PassWord"] = { "PassWord", "char(65) not null", "char(65)" };
		////info.m_mapField["Friend"] = { "Friend", "blob not null", "blob" };
		//      info.m_mapField["Friend"] = { "Friend", "blob default null", "blob" };
		// info.m_mapField["PersonInfo"] = { "PersonInfo", "blob default null", "blob" };
		//
		// info.m_strKeyString = "UNIQUE KEY(MobilePhone)";
		// m_vecTableInfo.push_back(info);
	}

	// 2. t_user_relationship
	// {
	// 	STableInfo info;
	// 	// name type desc?
	// 	info.m_strName = "t_user_relationship";
	// 	info.m_mapField["f_id"] = {"f_id", "bigint(20) NOT NULL AUTO_INCREMENT COMMENT '自增ID'", "bigint(20)"};
	// 	info.m_mapField["f_user_id1"] = {"f_user_id1", "bigint(20) NOT NULL COMMENT '用户ID'", "bigint(20)"};
	// 	info.m_mapField["f_user_id2"] = {"f_user_id2", "bigint(20) NOT NULL COMMENT '用户ID'", "bigint(20)"};
	// 	info.m_mapField["f_user1_teamname"] = {"f_user1_teamname", "VARCHAR(32) NOT NULL DEFAULT 'My Friends' COMMENT '用户2在用户1的好友分组名称'", "VARCHAR(32)"};
	// 	info.m_mapField["f_user2_teamname"] = {"f_user2_teamname", "VARCHAR(32) NOT NULL DEFAULT 'My Friends' COMMENT '用户1在用户2的好友分组名称'", "VARCHAR(32)"};
	// 	info.m_mapField["f_user1_markname"] = {"f_user1_markname", "VARCHAR(32) COMMENT '用户2在用户1的备注名称'", "VARCHAR(32)"},
	// 	info.m_mapField["f_user2_markname"] = {"f_user2_markname", "VARCHAR(32) COMMENT '用户1在用户2的备注名称'", "VARCHAR(32)"},
	// 	info.m_mapField["f_remark"] = {"f_remark", "varchar(64) NULL COMMENT '备注'", "varchar(64)"};
	// 	info.m_mapField["f_update_time"] = {"f_update_time", "timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间'", "timestamp"};

	// 	info.m_strKeyString = "PRIMARY KEY (f_id), INDEX f_id (f_id)";
	// 	m_vecTableInfo.push_back(info);

	// 	// info.m_mapField["AccountID"] = { "AccountID", "integer NOT NULL AUTO_INCREMENT PRIMARY KEY", "int(11)" };
	// 	// info.m_mapField["MobilePhone"] = { "MobilePhone", "bigint NOT NULL", "bigint(20)" };
	// 	// info.m_mapField["NickName"] = { "NickName", "char(65) not null", "char(65)" };
	// 	// info.m_mapField["PassWord"] = { "PassWord", "char(65) not null", "char(65)" };
	// 	////info.m_mapField["Friend"] = { "Friend", "blob not null", "blob" };
	// 	//      info.m_mapField["Friend"] = { "Friend", "blob default null", "blob" };
	// 	// info.m_mapField["PersonInfo"] = { "PersonInfo", "blob default null", "blob" };
	// 	//
	// 	// info.m_strKeyString = "UNIQUE KEY(MobilePhone)";
	// 	// m_vecTableInfo.push_back(info);
	// }

	// 3. t_chatmsg
	{
		STableInfo chat;
		chat.m_strName = "t_chatmsg";
		chat.m_mapField["f_id"] = {"f_id", "bigint(20) NOT NULL AUTO_INCREMENT COMMENT '自增ID'", "bigint(20)"};
		chat.m_mapField["f_senderid"] = {"f_senderid", "bigint(20) NOT NULL COMMENT '发送者id'", "bigint(20)"};
		// chat.m_mapField["f_targetid"] = {"f_targetid", "bigint(20) NOT NULL COMMENT '接收者id'", "bigint(20)"};
		chat.m_mapField["f_msgcontent"] = {"f_msgcontent", "BLOB NOT NULL COMMENT '聊天内容'", "BLOB"};
		chat.m_mapField["f_create_time"] = {"f_create_time", "timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间'", "timestamp"};
		// chat.m_mapField["f_remark"] = {"f_remark", "varchar(64) NULL COMMENT '备注'", "varchar(64)"};

		chat.m_strKeyString = "PRIMARY KEY (f_id), INDEX f_id (f_id)";
		m_vecTableInfo.push_back(chat);
	}
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

	////////////////////// 2. 检查库中表是否正确 /////////////////////////
	for (size_t i = 0; i < m_vecTableInfo.size(); i++)
	{
		STableInfo table = m_vecTableInfo[i];
		if (!checkTable(table))
		{
			// LOG_FATAL << "CMysqlManager::Init, table check failed : " << table.m_strName;
			return false;
		}
	}
	////////////////////// 2. 检查库中表是否正确 /////////////////////////

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

bool CMysqlManager::checkTable(const STableInfo &table)
{
	if (nullptr == m_poConn)
	{
		return false;
	}

	std::stringstream ss;
	ss << "DESC " << table.m_strName << ";";
	int nErrno = 0;
	if (m_poConn->execute(ss.str().c_str(), nErrno))
	{
		// 表存在 删除表 创建表
		std::stringstream ss2;
		ss2 << "DROP TABLE IF EXISTS " << table.m_strName << ";";
		if (m_poConn->execute(ss2.str().c_str()))
		{
			if (createTable(table))
			{
				return true;
			}
		}
		return false;
	}
	else
	{
		// 创建表
		//  if (nErrno == 1146)
		//  {
		if (createTable(table))
		{
			return true;
		}
		return false;
		// }
	}

	return true;
}

bool CMysqlManager::createTable(const STableInfo &table)
{
	if (table.m_mapField.size() == 0)
	{
		// LOGE << "CMysqlManager::_CreateTable, table info not valid, " << table.m_strName;
		return false;
	}

	std::stringstream ss;
	ss << "CREATE TABLE IF NOT EXISTS " << table.m_strName << " (";

	for (std::map<std::string, STableField>::const_iterator it = table.m_mapField.begin();
		 it != table.m_mapField.end(); ++it)
	{
		if (it != table.m_mapField.begin())
		{
			ss << ", ";
		}

		STableField field = it->second;
		ss << field.m_strName << " " << field.m_strType;
	}

	if (table.m_strKeyString != "")
	{
		ss << ", " << table.m_strKeyString;
	}

	ss << ")default charset = utf8, ENGINE = InnoDB;";
	if (m_poConn->execute(ss.str().c_str()))
	{
		return true;
	}

	// LOGE("Create table error, sql: %s", ss.str().c_str());
	return false;
}