#include "Field.h"

Field::Field() : m_iType(DB_TYPE_UNKNOWN)
{

}

// Field::Field(Field &f)
// {
//     m_strFieldName = f.getName();
//     m_iType = f.getType();
// }

Field::Field(const char *name, enum Field::DataTypes type) : m_strFieldName(name), m_iType(type)
{
    toLowerString(m_strFieldName);
}