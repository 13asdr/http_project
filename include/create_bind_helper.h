#pragma once
#include "mysql.h"

namespace createBindHelper
{
    inline MYSQL_BIND create_input_bind(const char *_parameter, enum_field_types _type)
    {
        MYSQL_BIND input{};
        input.buffer_type = MYSQL_TYPE_STRING;
        input.buffer = (void *)_parameter;
        input.buffer_length = static_cast<unsigned long>(strlen(_parameter));
        return input;
    }

    template <typename T>
    MYSQL_BIND create_input_bind(T &_parameter, enum_field_types _type)
    {
        MYSQL_BIND input{};
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

    inline MYSQL_BIND create_output_bind(char *_parameter, size_t _buffer_size, unsigned long &_length, enum_field_types _type)
    {
        MYSQL_BIND output{};
        output.buffer_type = MYSQL_TYPE_STRING;
        output.buffer = (void *)_parameter;
        output.buffer_length = static_cast<unsigned long>(_buffer_size);
        output.length = &_length;
        return output;
    }

    template <typename T>
    MYSQL_BIND create_output_bind(T &_parameter, enum_field_types _type)
    {
        MYSQL_BIND output{};
        switch (_type)
        {
        case MYSQL_TYPE_LONG:
            output.buffer_type = MYSQL_TYPE_LONG;
            break;
        case MYSQL_TYPE_DOUBLE:
            output.buffer_type = MYSQL_TYPE_DOUBLE;
            break;
        }
        output.buffer = (void *)&_parameter;
        output.buffer_length = sizeof(_parameter);
        return output;
    }

}