#include "record_dao.h"

RecordDao::RecordDao(DbConnect &_db) : db(_db) {}

RecordDao::~RecordDao() {}

bool RecordDao::add(const Record &_record)
{
    std::ostringstream sql;

    sql << "INSERT INTO records (amount, note, type, time, category, user_id) VALUES ("
        << _record.amount << ","
        << "'" << _record.note << "',"
        << "'" << _record.type << "',"
        << "'" << _record.time << "',"
        << "'" << _record.category << "',"
        << _record.user_id << ")";

    return db.execute(sql.str());
}

std::vector<Record> RecordDao::list_order_by_time_and_id(int _user_id, Limit _limit)
{
    std::vector<Record> result;
    std::ostringstream sql;
    int offset = (_limit.page - 1) * _limit.page_size;

    sql << "SELECT id, amount, note, type, time, category, user_id FROM records WHERE user_id = " << _user_id
        << " ORDER BY time DESC, id DESC LIMIT " << _limit.page_size << " OFFSET " << offset;

    MYSQL_RES *res = db.query(sql.str());
    if (!res)
    {
        return result;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        Record record;
        row_to_record(row, record);
        result.push_back(record);
    }
    mysql_free_result(res);
    return result;
}

std::vector<Record> RecordDao::list_order_by_id(int _user_id)
{
    std::vector<Record> result;
    std::ostringstream sql;

    sql << "SELECT id, amount, note, type, time, category, user_id FROM records WHERE user_id = " << _user_id << " ORDER BY id ASC";

    MYSQL_RES *res = db.query(sql.str());
    if (!res)
    {
        return result;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        Record record;
        row_to_record(row, record);
        result.push_back(record);
    }
    mysql_free_result(res);
    return result;
}

std::vector<Record> RecordDao::list_by_month(const std::string &_month_type, int _user_id, Limit _limit)
{
    std::vector<Record> result;
    std::ostringstream sql;
    int offset = (_limit.page - 1) * _limit.page_size;

    sql << "SELECT id, amount, note, type, time, category, user_id FROM records "
        << "WHERE DATE_FORMAT(time, '%Y-%m') = '" << _month_type << "' AND user_id = " << _user_id << " "
        << "ORDER BY time DESC LIMIT " << _limit.page_size << " OFFSET " << offset;

    MYSQL_RES *res = db.query(sql.str());
    if (!res)
    {
        return result;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        Record record;
        row_to_record(row, record);
        result.push_back(record);
    }
    mysql_free_result(res);
    return result;
}

std::map<std::string, double> RecordDao::stat_by_category(int _user_id)
{
    std::map<std::string, double> result;
    MYSQL_RES *res = db.query("SELECT category, SUM(amount) AS total FROM records where type = '鏀嚭' AND user_id = " + std::to_string(_user_id) + " GROUP BY category");
    if (!res)
    {
        return result;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        result.try_emplace(row[0], std::stod(row[1]));
    }
    mysql_free_result(res);
    return result;
}

std::vector<Record> RecordDao::search(const std::string &_keyword, int _user_id, Limit _limit)
{
    std::vector<Record> result;
    std::ostringstream sql;
    int offset = (_limit.page - 1) * _limit.page_size;

    sql << "SELECT id, amount, note, type, time, category, user_id FROM records "
        << "WHERE note LIKE '%" << _keyword << "%' AND user_id = " << _user_id << " "
        << "ORDER BY time DESC LIMIT " << _limit.page_size << " OFFSET " << offset;

    MYSQL_RES *res = db.query(sql.str());
    if (!res)
    {
        return result;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        Record record;
        row_to_record(row, record);
        result.push_back(record);
    }
    mysql_free_result(res);
    return result;
}

std::vector<Record> RecordDao::filter(const std::string &_keyword, const std::string &_month_type, int _user_id, Limit _limit)
{
    std::vector<Record> result;
    std::ostringstream sql;
    int offset = (_limit.page - 1) * _limit.page_size;
    int page_size = _limit.page_size;

    sql << "SELECT id, amount, note, type, time, category, user_id FROM records WHERE 1=1 ";

    if (!_keyword.empty())
    {
        sql << "AND note LIKE '%" << _keyword << "%' ";
    }
    if (!_month_type.empty())
    {
        sql << "AND DATE_FORMAT(time, '%Y-%m') = '" << _month_type << "' ";
    }
    sql << "AND user_id = " << _user_id << " ORDER BY time DESC, id DESC LIMIT " << page_size << " OFFSET " << offset;

    MYSQL_RES *res = db.query(sql.str());
    if (!res)
    {
        return result;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        Record record;
        row_to_record(row, record);
        result.push_back(record);
    }
    mysql_free_result(res);
    return result;
}

int RecordDao::count_records(int _user_id, const std::string &_month_type, const std::string &_keyword)
{
    std::ostringstream sql;
    sql << "SELECT COUNT(*) FROM records WHERE 1=1 ";

    if (!_month_type.empty())
    {
        sql << "AND DATE_FORMAT(time, '%Y-%m') = '" << _month_type << "' ";
    }
    if (!_keyword.empty())
    {
        sql << "AND note LIKE '%" << _keyword << "%' ";
    }
    sql << "AND user_id = " << _user_id;

    MYSQL_RES *res = db.query(sql.str());
    if (!res)
    {
        return 0;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    int count = row ? std::stoi(row[0]) : 0;

    mysql_free_result(res);
    return count;
}

bool RecordDao::update(int _id, const Record &_record, int _user_id)
{
    std::ostringstream sql;
    sql << "UPDATE records set amount = " << _record.amount << ","
        << "note = '" << _record.note << "',"
        << "type = '" << _record.type << "',"
        << "time = '" << _record.time << "',"
        << "category = '" << _record.category << "',"
        << "user_id = " << _user_id << " "
        << "WHERE id = " << _id << " AND user_id = " << _user_id << " ";

    return db.execute(sql.str());
}

bool RecordDao::remove(int _id, int _user_id)
{
    std::ostringstream sql;
    sql << "DELETE FROM records WHERE id = " << _id << " AND user_id = " << _user_id << " ";

    return db.execute(sql.str());
}

void RecordDao::row_to_record(MYSQL_ROW &_row, Record &_record)
{
    _record.id = std::stoi(_row[0]);
    _record.amount = std::stod(_row[1]);
    _record.note = _row[2];
    _record.type = _row[3];
    _record.time = _row[4];
    _record.category = _row[5];
    _record.user_id = std::stoi(_row[6]);
}
