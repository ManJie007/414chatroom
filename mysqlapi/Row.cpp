#include "Row.h"

Row::Row(MYSQL_ROW row, unsigned long *valueLengths, size_t fieldCounts)
{
    for (size_t i = 0; i < fieldCounts; i++)
    {
        m_strValues.push_back(std::string(row[i], valueLengths[i]));
        m_uLens.push_back(valueLengths[i]);
    }
    
}

// Field::Field() : m_iType(DB_TYPE_UNKNOWN)
// {
//     m_bNULL = false;
// }

// Field::Field(Field &f)
// {
//     m_strValue = f.m_strValue;
//     m_strFieldName = f.m_strFieldName;

//     m_iType = f.getType();
// }

// Field::Field(const char *value, enum Field::DataTypes type) : m_iType(type)
// {
//     m_strValue = value;
// }

// Field::~Field()
// {
// }

// void Field::setValue(const char *value, size_t uLen)
// {
//     //m_strValue = value;
//     m_strValue.assign(value, uLen);
// }