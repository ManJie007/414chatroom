#!/bin/bash
g++ testdb.cc ../../mysqlapi/DatabaseMysql.cpp ../../mysqlapi/QueryResult.cpp ../../mysqlapi/Field.cpp ../../mysqlapi/Row.cpp -o testdb -std=c++11 -L/usr/local/mysql/lib -lmysqlclient