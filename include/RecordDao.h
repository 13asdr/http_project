#pragma once
#include <string>
#include <vector>
#include <mysql.h>
#include <sstream>
#include <map>

#include "DBconnect.h"

struct Record
{
    int id;
    double amount;
    std::string note;
    std::string type;
    std::string time;
    std::string category; // 账单类别
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
    std::map<std::string, double> statByCategory(); // 查询每个类别别的总金额

    // 更新
    bool update(int id, const Record &record);

    bool remove(int id);

private:
    DBconnect &db;
};