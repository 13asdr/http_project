#pragma once
#include <string>
#include <vector>
#include <mysql.h>
#include <sstream>
#include <map>

#include "db_connect.h"
#include "logger.h"

struct Limit // 分页参数
{
    int page;
    int pageSize;
};

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
    RecordDao(DBconnect &_db);
    ~RecordDao();

    // 添加
    bool add(const Record &_record);

    // 分页查询记录
    // 查询记录
    std::vector<Record> list_order_by_timeAndId(int _user_id, Limit _l);                     // 时间排序查询所有记录
    std::vector<Record> list_order_by_id(int _user_id);                                      // id排序查询所有记录(专门用于导出)
    std::vector<Record> listByMonth(const std::string &_month_type, int _user_id, Limit _l); // 按月份查找
    std::map<std::string, double> statByCategory(int _user_id);                              // 查询每个类别别的总金额

    // 模糊查询备注关键词
    std::vector<Record> search(const std::string &_keyword, int _user_id, Limit _l);                                 // 搜索备注关键词,模糊查询
    std::vector<Record> filter(const std::string &_keyword, const std::string &_month_type, int _user_id, Limit _l); // 月份筛选加模糊查询备注关键词

    // 查询中记录数目
    int count_records(int _user_id, const std::string &_month_type = "", const std::string &_keyword = "");

    // 更新
    bool update(int _id, const Record &_record, int _user_id);

    bool remove(int _id, int _user_id);

private:
    DBconnect &db;

    void RowTORecord(MYSQL_ROW &_row, Record &_record);
};