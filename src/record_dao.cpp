#include "record_dao.h"

RecordDao::RecordDao(DbConnect &_db) : db(_db) {}

RecordDao::~RecordDao() {}

bool RecordDao::add(const Record &_record)
{
    constexpr const char *sql = "INSERT INTO records (amount, note, type, time, category, user_id) VALUES (?, ?, ?, ?, ?, ?)";
    PreparedStmt stmt(db.get_conn(), sql);

    RecordBindBufferInput input_buffer(_record);
    if (!stmt.bind_param(input_buffer.input.data()))
    {
        return false;
    }
    return stmt.execute();
}

std::vector<Record> RecordDao::list_order_by_time_and_id(int _user_id, Limit _limit)
{
    constexpr const char *sql = "SELECT id, amount, note, type, time, category, user_id FROM records WHERE user_id = ? ORDER BY time DESC, id DESC LIMIT ? OFFSET ?";
    PreparedStmt stmt(db.get_conn(), sql);

    std::vector<MYSQL_BIND> input;
    input.reserve(3);
    int page_size = _limit.page_size;
    int offset = (_limit.page - 1) * _limit.page_size;
    input.push_back(createBindHelper::create_input_bind(_user_id, MYSQL_TYPE_LONG));
    input.push_back(createBindHelper::create_input_bind(page_size, MYSQL_TYPE_LONG));
    input.push_back(createBindHelper::create_input_bind(offset, MYSQL_TYPE_LONG));

    if (!stmt.bind_param(input.data()) || !stmt.execute())
    {
        return {};
    }

    RecordBindBufferOutput output_buffer;
    if (!stmt.bind_result(output_buffer.output.data()))
    {
        return {};
    }

    return stmt.fetch_all<Record>([&](MYSQL_STMT *stmt) -> Record
                                  { return output_buffer.to_record(); });
}

std::vector<Record> RecordDao::list_order_by_id(int _user_id)
{
    constexpr const char *sql = "SELECT id, amount, note, type, time, category, user_id FROM records WHERE user_id = ? ORDER BY id ASC";
    PreparedStmt stmt(db.get_conn(), sql);

    MYSQL_BIND input[1] = {};
    input[0] = createBindHelper::create_input_bind(_user_id, MYSQL_TYPE_LONG);

    if (!stmt.bind_param(input) || !stmt.execute())
    {
        return {};
    }
    RecordBindBufferOutput output_buffer;
    if (!stmt.bind_result(output_buffer.output.data()))
    {
        return {};
    }
    return stmt.fetch_all<Record>([&](MYSQL_STMT *stmt) -> Record
                                  { return output_buffer.to_record(); });
}

std::vector<Record> RecordDao::list_by_month(const std::string &_month_type, int _user_id, Limit _limit)
{
    constexpr const char *sql = "SELECT id, amount, note, type, time, category, user_id FROM records WHERE DATE_FORMAT(time, '%Y-%m') = ? AND user_id = ? ORDER BY time DESC LIMIT ? OFFSET ?";

    PreparedStmt stmt(db.get_conn(), sql);

    std::vector<MYSQL_BIND> input;
    input.reserve(4);

    const char *month_type = _month_type.c_str();
    input.push_back(createBindHelper::create_input_bind(month_type, MYSQL_TYPE_STRING));

    input.push_back(createBindHelper::create_input_bind(_user_id, MYSQL_TYPE_LONG));

    int page_size = _limit.page_size;
    int offset = (_limit.page - 1) * _limit.page_size;
    input.push_back(createBindHelper::create_input_bind(page_size, MYSQL_TYPE_LONG));
    input.push_back(createBindHelper::create_input_bind(offset, MYSQL_TYPE_LONG));

    if (!stmt.bind_param(input.data()) || !stmt.execute())
    {
        return {};
    }

    RecordBindBufferOutput output_buffer;
    if (!stmt.bind_result(output_buffer.output.data()))
    {
        return {};
    }

    return stmt.fetch_all<Record>([&](MYSQL_STMT *stmt) -> Record
                                  { return output_buffer.to_record(); });
}

std::map<std::string, double> RecordDao::stat_by_category(int _user_id)
{
    using map_type = std::pair<std::string, double>;

    constexpr const char *sql = "SELECT category, SUM(amount) AS total FROM records where type = '支出' AND user_id = ? GROUP BY category";

    PreparedStmt stmt(db.get_conn(), sql);
    MYSQL_BIND input[1] = {createBindHelper::create_input_bind(_user_id, MYSQL_TYPE_LONG)};
    if (!stmt.bind_param(input) || !stmt.execute())
    {
        return {};
    }

    std::map<std::string, double> result;
    char category[256];
    double total;
    unsigned long category_len;

    MYSQL_BIND output[2] = {};
    output[0].buffer_type = MYSQL_TYPE_STRING;
    output[0].buffer = (void *)category;
    output[0].buffer_length = sizeof(category);
    output[0].length = &category_len;
    output[1].buffer_type = MYSQL_TYPE_DOUBLE;
    output[1].buffer = (void *)&total;
    output[1].is_null = nullptr;

    if (!stmt.bind_result(output))
    {
        return {};
    }

    std::vector<map_type> stat_results = stmt.fetch_all<map_type>([&](MYSQL_STMT *stmt) -> map_type
                                                                  { return {std::string(category, category_len), total}; });

    for (const auto &[category, total] : stat_results)
    {
        result.try_emplace(category, total);
    }
    return result;
}

std::vector<Record> RecordDao::search(const std::string &_keyword, int _user_id, Limit _limit)
{

    constexpr const char *sql = "SELECT id, amount, note, type, time, category, user_id FROM records WHERE note LIKE ? AND user_id = ? ORDER BY time DESC LIMIT ? OFFSET ?";

    PreparedStmt stmt(db.get_conn(), sql);

    std::vector<MYSQL_BIND> input;
    input.reserve(4);

    std::string like_keyword = "%" + _keyword + "%";
    const char *keyword = like_keyword.c_str();
    input.push_back(createBindHelper::create_input_bind(keyword, MYSQL_TYPE_STRING));

    input.push_back(createBindHelper::create_input_bind(_user_id, MYSQL_TYPE_LONG));

    int page_size = _limit.page_size;
    int offset = (_limit.page - 1) * _limit.page_size;
    input.push_back(createBindHelper::create_input_bind(page_size, MYSQL_TYPE_LONG));
    input.push_back(createBindHelper::create_input_bind(offset, MYSQL_TYPE_LONG));

    if (!stmt.bind_param(input.data()) || !stmt.execute())
    {
        return {};
    }

    RecordBindBufferOutput output_buffer;
    if (!stmt.bind_result(output_buffer.output.data()))
    {
        return {};
    }

    return stmt.fetch_all<Record>([&](MYSQL_STMT *stmt) -> Record
                                  { return output_buffer.to_record(); });
}

std::vector<Record> RecordDao::filter(const std::string &_keyword, const std::string &_month_type, int _user_id, Limit _limit)
{
    std::ostringstream sql;
    sql << "SELECT id, amount, note, type, time, category, user_id FROM records WHERE 1=1 ";
    if (!_keyword.empty())
    {
        sql << "AND note LIKE ? ";
    }
    if (!_month_type.empty())
    {
        sql << "AND DATE_FORMAT(time, '%Y-%m') = ?";
    }
    sql << "AND user_id = ? ORDER BY time DESC, id DESC LIMIT ? OFFSET ?";

    PreparedStmt stmt(db.get_conn(), sql.str().c_str());

    std::vector<MYSQL_BIND> input;
    input.reserve(5);

    std::string keyword = "%" + _keyword + "%";
    if (!_keyword.empty())
    {
        const char *keyword_char = keyword.c_str();
        input.push_back(createBindHelper::create_input_bind(keyword_char, MYSQL_TYPE_STRING));
    }
    if (!_month_type.empty())
    {
        const char *month_type = _month_type.c_str();
        input.push_back(createBindHelper::create_input_bind(month_type, MYSQL_TYPE_STRING));
    }

    int user_id = _user_id;
    input.push_back(createBindHelper::create_input_bind(user_id, MYSQL_TYPE_LONG));

    int page_size = _limit.page_size;
    input.push_back(createBindHelper::create_input_bind(page_size, MYSQL_TYPE_LONG));

    int offset = (_limit.page - 1) * _limit.page_size;
    input.push_back(createBindHelper::create_input_bind(offset, MYSQL_TYPE_LONG));

    if (!stmt.bind_param(input.data()) || !stmt.execute())
    {
        return {};
    }

    RecordBindBufferOutput output_buffer;
    if (!stmt.bind_result(output_buffer.output.data()))
    {
        return {};
    }

    return stmt.fetch_all<Record>([&](MYSQL_STMT *) -> Record
                                  { return output_buffer.to_record(); });
}

int RecordDao::count_records(int _user_id, const std::string &_month_type, const std::string &_keyword)
{

    std::ostringstream sql;
    sql << "SELECT COUNT(*) FROM records WHERE 1=1 ";

    if (!_month_type.empty())
    {
        sql << "AND DATE_FORMAT(time, '%Y-%m') = ?";
    }
    if (!_keyword.empty())
    {
        sql << "AND note LIKE ?";
    }
    sql << "AND user_id = ?";
    PreparedStmt stmt(db.get_conn(), sql.str().c_str());

    std::vector<MYSQL_BIND> input;
    input.reserve(3);
    if (!_month_type.empty())
    {
        const char *month_type = _month_type.c_str();
        input.push_back(createBindHelper::create_input_bind(month_type, MYSQL_TYPE_STRING));
    }
    std::string keyword = "%" + _keyword + "%";
    if (!_keyword.empty())
    {
        const char *keyword_char = keyword.c_str();
        input.push_back(createBindHelper::create_input_bind(keyword_char, MYSQL_TYPE_STRING));
    }
    input.push_back(createBindHelper::create_input_bind(_user_id, MYSQL_TYPE_LONG));

    if (!stmt.bind_param(input.data()) || !stmt.execute())
    {
        return -1;
    }

    int count = 0;
    MYSQL_BIND output = createBindHelper::create_output_bind(count, MYSQL_TYPE_LONG);

    if (!stmt.bind_result(&output))
    {
        return -1;
    }

    if (!stmt.fetch_row())
    {
        return -1;
    }
    return count;
}

bool RecordDao::update(int _id, const Record &_record, int _user_id)
{

    constexpr const char *sql = "UPDATE records set amount = ? , note = ? , type = ? , time = ? , category = ? , user_id = ? WHERE id = ? AND user_id = ? ";

    PreparedStmt stmt(db.get_conn(), sql);

    RecordBindBufferInput input_bind(_record);
    input_bind.push_back(_id);
    input_bind.push_back(_user_id);

    if (!stmt.bind_param(input_bind.input.data()) || !stmt.execute())
    {
        return false;
    }

    return true;
}

bool RecordDao::remove(int _id, int _user_id)
{

    constexpr const char *sql = "DELETE FROM records WHERE id = ? AND user_id = ? ";
    PreparedStmt stmt(db.get_conn(), sql);

    std::vector<MYSQL_BIND> input;
    input.push_back(createBindHelper::create_input_bind<int>(_id, MYSQL_TYPE_LONG));
    input.push_back(createBindHelper::create_input_bind<int>(_user_id, MYSQL_TYPE_LONG));

    if (!stmt.bind_param(input.data()) || !stmt.execute())
    {
        return false;
    }
    return true;
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
