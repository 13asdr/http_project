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
    int user_id;          // 用户id
};

class RecordDao
{
public:
    RecordDao(DBconnect &db);
    ~RecordDao();

    // 添加
    bool add(const Record &record);

    // 查询记录
    std::vector<Record> list_order_by_time(int user_id); // 查询所有记录
    std::vector<Record> list_order_by_id(int user_id);
    std::vector<Record> listByMonth(const std::string &month_type, int user_id); // 按月份查找
    std::map<std::string, double> statByCategory(int user_id);                   // 查询每个类别别的总金额

    // 模糊查询备注关键词
    std::vector<Record> search(const std::string &keyword, int user_id);                                // 搜索备注关键词,模糊查询
    std::vector<Record> filter(const std::string &keyword, const std::string &month_type, int user_id); // 月份筛选加模糊查询备注关键词

    // 更新
    bool update(int id, const Record &record , int user_id);

    bool remove(int id, int user_id);

private:
    DBconnect &db;

    void RowTORecord(MYSQL_ROW &row, Record &record);
};