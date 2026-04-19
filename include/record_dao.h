#pragma once

#include <mysql.h>

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "db_connect.h"
#include "logger.h"

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
