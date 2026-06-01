#pragma once
#include "mysql.h"

namespace createBindHelper
{
    inline MYSQL_BIND create_input_bind(const char *_parameter, enum_field_types _type)
    {
        MYSQL_BIND input;
        input.buffer_type = MYSQL_TYPE_STRING;
        input.buffer = (void *)_parameter;
        input.buffer_length = static_cast<unsigned long>(strlen(_parameter));
        return input;
    }

    template <typename T>
    MYSQL_BIND create_input_bind(T &_parameter, enum_field_types _type)
    {
        MYSQL_BIND input;
        switch (_type)
        {
        case MYSQL_TYPE_LONG:
            input.buffer_type = MYSQL_TYPE_LONG;
            break;
        case MYSQL_TYPE_DOUBLE:
            input.buffer_type = MYSQL_TYPE_DOUBLE;
            break;
        }
        input.buffer = (void *)&_parameter;
        return input;
    }

    inline MYSQL_BIND create_output_bind(char *_parameter, unsigned long &_length, enum_field_types _type)
    {
        MYSQL_BIND input;
        input.buffer_type = MYSQL_TYPE_STRING;
        input.buffer = (void *)_parameter;
        input.buffer_length = sizeof(_parameter);
        input.length = &_length;
        return input;
    }

    template <typename T>
    MYSQL_BIND create_output_bind(T &_parameter, enum_field_types _type)
    {
        MYSQL_BIND input;
        switch (_type)
        {
        case MYSQL_TYPE_LONG:
            input.buffer_type = MYSQL_TYPE_LONG;
            break;
        case MYSQL_TYPE_DOUBLE:
            input.buffer_type = MYSQL_TYPE_DOUBLE;
            break;
        }
        input.buffer = (void *)&_parameter;
        input.buffer_length = sizeof(_parameter);
        return input;
    }

}