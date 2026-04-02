#pragma once
#include <string>
#include <vector>
#include <mysql.h>
#include <sstream>
#include "DBconnect.h"

struct Record
{
    int id;
    double amount;
    std::string note;
    std::string type;
    std::string time;
};

class RecordDao
{
public:
    RecordDao(DBconnect &db);
    ~RecordDao();

    // 添加
    bool add(const Record &record);

    // 查询记录
    std::vector<Record> list();
    std::vector<Record> listByMonth(const std::string &month_type);

    bool remove(int id);

private:
    DBconnect &db;
};