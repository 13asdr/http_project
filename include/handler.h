#ifndef HANDLER_H
#define HANDLER_H

#include "httplib.h"
#include "nlohmann/json.hpp"
#include "RecordDao.h"
#include <iostream>

class Handler
{
public:
    static void Add(RecordDao &dao, const httplib::Request &req, httplib::Response &res);
    static void List(RecordDao &dao, const httplib::Request &req, httplib::Response &res);
    static void StatByCategory(RecordDao &dao, const httplib::Request &req, httplib::Response &res);
    static void ListByMonth(RecordDao &dao, const httplib::Request &req, httplib::Response &res); // 按月份查询
    static void Search(RecordDao &dao, const httplib::Request &req, httplib::Response &res);      // 模糊查询备注关键词
    static void Filter(RecordDao &dao, const httplib::Request &req, httplib::Response &res);      // 模糊查询备注关键词 AND 月份筛选
    static void Update(RecordDao &dao, const httplib::Request &req, httplib::Response &res);      // 更新函数
    static void Remove(RecordDao &dao, const httplib::Request &req, httplib::Response &res);      // 删除记录
private:
    static void JsonToRecord(nlohmann::json &j, Record &r);
    static void RecordToJson(const Record &r, nlohmann::json &j);
};

#endif
