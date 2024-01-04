#pragma once

#include <algorithm>
#include <string>
#include <vector>

inline void toLowerString(std::string& str)
{
    for(size_t i = 0; i < str.size(); i++)
    {
        if(str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] = str[i] + ('a' - 'A');
        }
    }
}

class Field
{
    public:

        enum DataTypes
        {
            DB_TYPE_UNKNOWN = 0x00,
            DB_TYPE_STRING  = 0x01,
            DB_TYPE_INTEGER = 0x02,
            DB_TYPE_FLOAT   = 0x03,
            DB_TYPE_BOOL    = 0x04
        };

        Field();
        // Field(Field &f);
        Field(const char *name, enum DataTypes type = DataTypes::DB_TYPE_UNKNOWN);

        ~Field() = default;

        enum DataTypes getType() const { return m_iType; }
        void setType(enum DataTypes type) { m_iType = type; }

        const std::string& getName()
        {
            return m_strFieldName;
        }
        void setName(const std::string& strName)
        {
            m_strFieldName = strName;
            toLowerString(m_strFieldName);
        }

        std::string printType(enum DataTypes type)
        {
            switch (type)
            {
            case DataTypes::DB_TYPE_STRING:
                return "DB_TYPE_STRING";
                break;
            case DataTypes::DB_TYPE_INTEGER:
                return "DB_TYPE_INTEGER";
                break;
            case DataTypes::DB_TYPE_FLOAT:
                return "DB_TYPE_FLOAT";
                break;
            case DataTypes::DB_TYPE_BOOL:
                return "DB_TYPE_BOOL";
                break;
            
            default:
                return "DB_TYPE_UNKNOWN";
                break;
            }
        }

        std::string toString()
        {
            std::string output;
            output += "| ";
            output += m_strFieldName;
            output += " ( ";
            output += printType(m_iType);
            output += " ) | ";

            return output; 
        }

    private:
        std::string          m_strFieldName;
        enum DataTypes       m_iType;

    
};
