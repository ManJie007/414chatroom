#pragma once

// #include <assert.h>
#include <mysql/mysql.h>
#include <cstdint>
#include <vector>
#include <map>
#include <iostream>

#include "Field.h"
#include "Row.h"

class QueryResult
{
public:
    // typedef std::map<uint32_t, std::string> FieldNames;

    QueryResult(MYSQL_RES *result);
    ~QueryResult();

    void printResult(std::ostream &outputstream = std::cout);

    std::vector<Row> const &getRows() const { return m_Rows; }

    //     virtual bool nextRow();

    //     uint32_t getField_idx(const std::string& name) const
    //     {
    //         for(FieldNames::const_iterator iter = getFieldNames().begin(); iter != getFieldNames().end(); ++iter)
    //         {
    //             if(iter->second == name)
    //                 return iter->first;
    //         }

    //         //assert(false && "unknown field name");
    //         return uint32_t(-1);
    //     }

    //     Field* fetch() const { return m_CurrentRow; }

    //     const Field & operator [] (int index) const
    //     {
    //         return m_CurrentRow[index];
    //     }

    //     const Field& operator [] (const std::string &name) const
    //     {
    //         return m_CurrentRow[getField_idx(name)];
    //     }

    //     uint32_t getFieldCount() const { return m_FieldCount; }
    //     uint64_t getRowCount() const { return m_RowCount; }
    //     FieldNames const& getFieldNames() const {return m_FieldNames; }

    //     std::vector<std::string> const& getNames() const {return m_vtFieldNames;}

    // private:
    //     enum Field::DataTypes convertNativeType(enum_field_types mysqlType) const;
    // public:
    //     void endQuery();

private:
    MYSQL_RES *m_Result;
    size_t m_FieldCount;
    std::vector<Field> m_Fields;
    size_t m_RowCount;
    std::vector<Row> m_Rows;

    enum Field::DataTypes convertNativeType(enum_field_types mysqlType) const;
};
