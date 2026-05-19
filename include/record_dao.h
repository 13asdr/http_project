#pragma once

#include <mysql.h>

#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>

#include "db_connect.h"
#include "logger.h"
#include "PreparedStmt.h"

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
            input.buffer = (void *)&_parameter;
            break;
        case MYSQL_TYPE_DOUBLE:
            input.buffer_type = MYSQL_TYPE_DOUBLE;
            input.buffer = (void *)&_parameter;
            break;
        }
        return input;
    }

}

struct Limit
{
    int page;
    int page_size;
};

struct Record
{
    int id;
    double amount;
    std::string note;
    std::string type;
    std::string time;
    std::string category;
    int user_id;
};

class RecordDao;

struct RecordBindBufferInput
{
    std::vector<MYSQL_BIND> input;
    RecordBindBufferInput(const Record &_record)
    {
        input.reserve(6);
        input[0].buffer_type = MYSQL_TYPE_DOUBLE;
        input[0].buffer = (void *)&_record.amount;

        input[1].buffer_type = MYSQL_TYPE_STRING;
        input[1].buffer = (void *)_record.note.c_str();
        input[1].buffer_length = static_cast<unsigned long>(_record.note.length());

        input[2].buffer_type = MYSQL_TYPE_STRING;
        input[2].buffer = (void *)_record.type.c_str();
        input[2].buffer_length = static_cast<unsigned long>(_record.type.length());

        input[3].buffer_type = MYSQL_TYPE_STRING;
        input[3].buffer = (void *)_record.time.c_str();
        input[3].buffer_length = static_cast<unsigned long>(_record.time.length());

        input[4].buffer_type = MYSQL_TYPE_STRING;
        input[4].buffer = (void *)_record.category.c_str();
        input[4].buffer_length = static_cast<unsigned long>(_record.category.length());

        input[5].buffer_type = MYSQL_TYPE_LONG;
        input[5].buffer = (void *)&_record.user_id;
    }

    void push_back(int &_parameter)
    {
        input.push_back(createBindHelper::create_input_bind<int>(_parameter, MYSQL_TYPE_LONG));
    }
};

struct RecordBindBufferOutput
{
    char note[1024];
    char type[256];
    char time[256];
    char category[256];
    unsigned long note_len;
    unsigned long type_len;
    unsigned long time_len;
    unsigned long category_len;

    int id;
    double amount;
    int user_id;

    MYSQL_BIND output[7] = {};

    RecordBindBufferOutput()
    {
        memset(output, 0, sizeof(output));

        output[0].buffer_type = MYSQL_TYPE_LONG;
        output[0].buffer = (void *)&id;
        output[1].buffer_type = MYSQL_TYPE_DOUBLE;
        output[1].buffer = (void *)&amount;
        output[2].buffer_type = MYSQL_TYPE_STRING;
        output[2].buffer = note;
        output[2].buffer_length = sizeof(note);
        output[2].length = &note_len;
        output[3].buffer_type = MYSQL_TYPE_STRING;
        output[3].buffer = type;
        output[3].buffer_length = sizeof(type);
        output[3].length = &type_len;
        output[4].buffer_type = MYSQL_TYPE_STRING;
        output[4].buffer = time;
        output[4].buffer_length = sizeof(time);
        output[4].length = &time_len;
        output[5].buffer_type = MYSQL_TYPE_STRING;
        output[5].buffer = category;
        output[5].buffer_length = sizeof(category);
        output[5].length = &category_len;
        output[6].buffer_type = MYSQL_TYPE_LONG;
        output[6].buffer = (void *)&user_id;
    }

    Record to_record() const
    {
        Record record;
        record.id = id;
        record.amount = amount;
        record.note = std::string(note, note_len);
        record.type = std::string(type, type_len);
        record.time = std::string(time, time_len);
        record.category = std::string(category, category_len);
        record.user_id = user_id;
        return record;
    }
};

class RecordDao
{
public:
    explicit RecordDao(DbConnect &_db);
    ~RecordDao();

    bool add(const Record &_record);
    std::vector<Record> list_order_by_time_and_id(int _user_id, Limit _limit);
    std::vector<Record> list_order_by_id(int _user_id);
    std::vector<Record> list_by_month(const std::string &_month_type, int _user_id, Limit _limit);
    std::map<std::string, double> stat_by_category(int _user_id);
    std::vector<Record> search(const std::string &_keyword, int _user_id, Limit _limit);
    std::vector<Record> filter(const std::string &_keyword, const std::string &_month_type, int _user_id, Limit _limit);
    int count_records(int _user_id, const std::string &_month_type = "", const std::string &_keyword = "");
    bool update(int _id, const Record &_record, int _user_id);
    bool remove(int _id, int _user_id);

private:
    DbConnect &db;

    void row_to_record(MYSQL_ROW &_row, Record &_record);
};
