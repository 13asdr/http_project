#include "record_dao.h"

RecordDao::RecordDao(DbConnect &_db) : db(_db) {}

RecordDao::~RecordDao() {}

bool RecordDao::add(const Record &_record)
{
    std::ostringstream sql;
    Record escaped_record = escape_record(_record);

    sql << "INSERT INTO records (amount, note, type, time, category, user_id) VALUES ("
        << escaped_record.amount << ","
        << "'" << escaped_record.note << "',"
        << "'" << escaped_record.type << "',"
        << "'" << escaped_record.time << "',"
        << "'" << escaped_record.category << "',"
        << escaped_record.user_id << ")";

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
    std::string escaped_month_type = escape_string(_month_type);

    sql << "SELECT id, amount, note, type, time, category, user_id FROM records "
        << "WHERE DATE_FORMAT(time, '%Y-%m') = '" << escaped_month_type << "' AND user_id = " << _user_id << " "
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
    MYSQL_RES *res = db.query("SELECT category, SUM(amount) AS total FROM records where type = '支出' AND user_id = " + std::to_string(_user_id) + " GROUP BY category");
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
    std::string escaped_keyword = escape_string(_keyword);

    sql << "SELECT id, amount, note, type, time, category, user_id FROM records "
        << "WHERE note LIKE '%" << escaped_keyword << "%' AND user_id = " << _user_id << " "
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

    std::string escaped_keyword = escape_string(_keyword);
    std::string escaped_month_type = escape_string(_month_type);

    if (!escaped_keyword.empty())
    {
        sql << "AND note LIKE '%" << escaped_keyword << "%' ";
    }
    if (!escaped_month_type.empty())
    {
        sql << "AND DATE_FORMAT(time, '%Y-%m') = '" << escaped_month_type << "' ";
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

    std::string escaped_month_type = escape_string(_month_type);
    std::string escaped_keyword = escape_string(_keyword);

    if (!escaped_month_type.empty())
    {
        sql << "AND DATE_FORMAT(time, '%Y-%m') = '" << escaped_month_type << "' ";
    }
    if (!escaped_keyword.empty())
    {
        sql << "AND note LIKE '%" << escaped_keyword << "%' ";
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
    Record escaped_record = escape_record(_record);
    sql << "UPDATE records set amount = " << escaped_record.amount << ","
        << "note = '" << escaped_record.note << "',"
        << "type = '" << escaped_record.type << "',"
        << "time = '" << escaped_record.time << "',"
        << "category = '" << escaped_record.category << "',"
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

Record RecordDao::escape_record(const Record &_record)
{

    Record escaped_record;
    escaped_record.id = _record.id;
    escaped_record.amount = _record.amount;
    escaped_record.note = escape_string(_record.note);
    escaped_record.type = escape_string(_record.type);
    escaped_record.time = escape_string(_record.time);
    escaped_record.category = escape_string(_record.category);
    escaped_record.user_id = _record.user_id;
    return escaped_record;
}

std::string RecordDao::escape_string(const std::string &_input)
{
    return db.escape_string(_input);
}
