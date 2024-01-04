#pragma once

#include <algorithm>
#include <string>
#include <mysql/mysql.h>
#include <vector>

class Row
{
public:
    Row() = default;
    // Row(Row &f);
    Row(MYSQL_ROW row, unsigned long *valueLengths, size_t fieldCounts = 0);

    ~Row() = default;

    std::string toString()
    {
        std::string output;
        output += "|";
        for (size_t i = 0; i < m_strValues.size(); i++)
        {
            output += "(" + m_strValues[i] + ")(" + std::to_string(m_uLens[i]) + ")";
        }
        output += "|\n";

        return output;
    }

    std::vector<std::string> const &getStrValues() const { return m_strValues; }

    // const std::string getString() const { return m_strValue; }
    // std::string getCppString() const
    // {
    //     return m_strValue; // std::string s = 0 have undefine result in C++
    // }

    // void setType(enum DataTypes type) { m_iType = type; }

    // void setName(const std::string &strName)
    // {
    //     m_strFieldName = strName;
    //     toLowerString(m_strFieldName);
    // }
    // const std::string &getName()
    // {
    //     return m_strFieldName;
    // }

private:
    std::vector<std::string> m_strValues;
    std::vector<size_t> m_uLens;
};