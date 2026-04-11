#include "RecordDao.h"
#include <iostream>

RecordDao::RecordDao(DBconnect &db) : db(db) {}

RecordDao::~RecordDao() {}

bool RecordDao::add(const Record &record)
{
    std::ostringstream sql;

    sql << "INSERT INTO records (amount, note, type, time, category, user_id) VALUES ("
        << record.amount << ","
        << "'" << record.note << "',"
        << "'" << record.type << "',"
        << "'" << record.time << "',"
        << "'" << record.category << "',"
        << record.user_id << ")";

    return db.execute(sql.str());
}

std::vector<Record> RecordDao::list_order_by_time(int user_id)
{
    std::vector<Record> result;
    MYSQL_RES *res = db.query("SELECT id, amount, note, type, time, category ,user_id FROM records WHERE user_id = " + std::to_string(user_id) + " ORDER BY time DESC");

    if (!res)
    {
        return result;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        Record r;
        RowTORecord(row, r);
        result.push_back(r);
    }
    mysql_free_result(res);
    return result;
}

std::vector<Record> RecordDao::list_order_by_id(int user_id)
{
    std::vector<Record> result;
    MYSQL_RES *res = db.query("SELECT id, amount, note, type, time, category ,user_id FROM records WHERE user_id = " + std::to_string(user_id) + " ORDER BY id ASC");

    if (!res)
    {
        return result;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        Record r;
        RowTORecord(row, r);
        result.push_back(r);
    }
    mysql_free_result(res);
    return result;
}

std::vector<Record> RecordDao::listByMonth(const std::string &month_type, int user_id)
{
    std::vector<Record> result;

    std::ostringstream sql;
    sql << "SELECT id, amount, note, type, time, category ,user_id FROM records "
        << "WHERE DATE_FORMAT(time, '%Y-%m') = '" << month_type << "' AND user_id = " << user_id << " "
        << "ORDER BY time DESC";

    MYSQL_RES *res = db.query(sql.str());
    if (!res)
        return result;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        Record r;
        RowTORecord(row, r);
        result.push_back(r);
    }
    mysql_free_result(res);
    return result;
}

std::map<std::string, double> RecordDao::statByCategory(int user_id)
{
    std::map<std::string, double> result;
    MYSQL_RES *res = db.query("SELECT category, SUM(amount) AS total FROM records where type = '支出' AND user_id = " + std::to_string(user_id) + " GROUP BY category");
    if (!res)
        return result;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        result.try_emplace(row[0], std::stod(row[1]));
    }
    mysql_free_result(res);
    return result;
}

std::vector<Record> RecordDao::search(const std::string &keyword, int user_id)
{
    std::vector<Record> result;
    std::ostringstream sql;
    sql << "SELECT id,amount, note, type, time, category ,user_id FROM records "
        << "WHERE note LIKE '%" << keyword << "%' AND user_id = " << user_id << " "
        << "ORDER BY time DESC ";

    MYSQL_RES *res = db.query(sql.str());
    if (!res)
        return result;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        Record r;
        RowTORecord(row, r);
        result.push_back(r);
    }
    mysql_free_result(res);
    return result;
}

std::vector<Record> RecordDao::filter(const std::string &keyword, const std::string &month_type, int user_id)
{
    std::vector<Record> result;
    std::ostringstream sql;
    sql << "SELECT id,amount, note, type, time, category ,user_id FROM records "
        << "where 1=1 ";

    if (!keyword.empty())
    {
        sql << "AND note LIKE '%" << keyword << "%' ";
    }
    if (!month_type.empty())
    {
        sql << "AND DATE_FORMAT(time, '%Y-%m') = '" << month_type << "' ";
    }
    sql << "AND user_id = " << user_id << " ORDER BY time DESC ";

    MYSQL_RES *res = db.query(sql.str());
    if (!res)
        return result;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        Record r;
        RowTORecord(row, r);
        result.push_back(r);
    }
    mysql_free_result(res);
    return result;
}

bool RecordDao::update(int id, const Record &record , int user_id)
{
    std::ostringstream sql;
    sql << "UPDATE records set amount = " << record.amount << ","
        << "note = '" << record.note << "',"
        << "type = '" << record.type << "',"
        << "time = '" << record.time << "',"
        << "category = '" << record.category << "',"
        << "user_id = " << user_id << " "
        << "WHERE id = " << id << " AND user_id = " << user_id << " ";

    return db.execute(sql.str());
}

bool RecordDao::remove(int id, int user_id)
{
    std::ostringstream sql;
    sql << "DELETE FROM records WHERE id = " << id << " AND user_id = " << user_id << " ";

    return db.execute(sql.str());
}

void RecordDao::RowTORecord(MYSQL_ROW &row, Record &record)
{
    record.id = std::stoi(row[0]);
    record.amount = std::stod(row[1]);
    record.note = row[2];
    record.type = row[3];
    record.time = row[4];
    record.category = row[5];
    record.user_id = std::stoi(row[6]);
}
