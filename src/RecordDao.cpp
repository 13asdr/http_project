#include "RecordDao.h"
#include <iostream>

RecordDao::RecordDao(DBconnect &db) : db(db) {}

RecordDao::~RecordDao() {}

bool RecordDao::add(const Record &record)
{
    std::ostringstream sql;

    sql << "INSERT INTO records (amount, note, type, time) VALUES ("
        << record.amount << ","
        << "'" << record.note << "',"
        << "'" << record.type << "',"
        << "'" << record.time << "')";

    return db.execute(sql.str());
}

std::vector<Record> RecordDao::list()
{
    std::vector<Record> result;
    MYSQL_RES *res = db.query("SELECT id, amount, note, type, time FROM records ORDER BY time DESC");

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
        result.push_back(r);
    }
    mysql_free_result(res);
    return result;
}

std::vector<Record> RecordDao::listByMonth(const std::string &month_type)
{
    std::vector<Record> result;

    std::ostringstream sql;
    sql << "SELECT id, amount, note, type, time FROM records "
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
        result.push_back(r);
    }
    mysql_free_result(res);
    return result;
}

bool RecordDao::remove(int id)
{
    std::ostringstream sql;
    sql << "DELETE FROM records WHERE id = " << id;

    return db.execute(sql.str());
}