#ifndef HANDLER_H
#define HANDLER_H

#include "httplib.h"
#include "nlohmann/json.hpp"
#include "RecordDao.h"
#include <iostream>

class Handler
{
public:
    using Request = httplib::Request;
    using Response = httplib::Response;
    using Json = nlohmann::json;

    static void Add(RecordDao &dao, const Request &req, Response &res);
    static void List(RecordDao &dao, const Request &req, Response &res);
    static void StatByCategory(RecordDao &dao, const Request &req, Response &res);
    static void ListByMonth(RecordDao &dao, const Request &req, Response &res); // 按月份查询
    static void Search(RecordDao &dao, const Request &req, Response &res);      // 模糊查询备注关键词
    static void Filter(RecordDao &dao, const Request &req, Response &res);      // 模糊查询备注关键词 AND 月份筛选
    static void Update(RecordDao &dao, const Request &req, Response &res);      // 更新函数
    static void Remove(RecordDao &dao, const Request &req, Response &res);      // 删除记录
    static void Export(RecordDao &dao, const Request &req, Response &res);      // 导出记录

private:
    static void JsonToRecord(Json &j, Record &r);
    static void RecordToJson(const Record &r, Json &j);
};

#endif
