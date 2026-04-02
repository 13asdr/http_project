#include "RecordDao.h"
#include <iostream>

RecordDao::RecordDao(DBconnect &db) : db(db) {}

RecordDao::~RecordDao() {}

bool RecordDao::add(const Record &record)
{
    std::ostringstream sql;

    sql << "INSERT INTO records (amount, note, type, time, category) VALUES ("
        << record.amount << ","
        << "'" << record.note << "',"
        << "'" << record.type << "',"
        << "'" << record.time << "',"
        << "'" << record.category << "')";

    return db.execute(sql.str());
}

std::vector<Record> RecordDao::list()
{
    std::vector<Record> result;
    MYSQL_RES *res = db.query("SELECT id, amount, note, type, time, category FROM records ORDER BY time DESC");

    if (!res)
    {
        return result;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        Record r;
        r.id = std::stoi(row[0]);
        r.amount = std::stod(row[1]);
        r.note = row[2];
        r.type = row[3];
        r.time = row[4];
        r.category = row[5];
        result.push_back(r);
    }
    mysql_free_result(res);
    return result;
}

std::vector<Record> RecordDao::listByMonth(const std::string &month_type)
{
    std::vector<Record> result;

    std::ostringstream sql;
    sql << "SELECT id, amount, note, type, time, category FROM records "
        << "WHERE DATE_FORMAT(time, '%Y-%m') = '" << month_type << "' "
        << "ORDER BY time DESC";

    MYSQL_RES *res = db.query(sql.str());
    if (!res)
        return result;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        Record r;
        r.id = std::stoi(row[0]);
        r.amount = std::stod(row[1]);
        r.note = row[2];
        r.type = row[3];
        r.time = row[4];
        r.category = row[5];
        result.push_back(r);
    }
    mysql_free_result(res);
    return result;
}

std::map<std::string, double> RecordDao::statByCategory()
{
    std::map<std::string, double> result;
    MYSQL_RES *res = db.query("SELECT category, SUM(amount) AS total FROM records where type = '支出' GROUP BY category");
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

bool RecordDao::update(int id, const Record &record)
{
    std::ostringstream sql;
    sql << "UPDATE records set amount = " << record.amount << ","
        << "note = '" << record.note << "',"
        << "type = '" << record.type << "',"
        << "time = '" << record.time << "',"
        << "category = '" << record.category << "' "
        << "WHERE id = " << id;

    return db.execute(sql.str());
}

bool RecordDao::remove(int id)
{
    std::ostringstream sql;
    sql << "DELETE FROM records WHERE id = " << id;

    return db.execute(sql.str());
}