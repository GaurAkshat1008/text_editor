#pragma once
#include <string>
#include <vector>

struct where_condition
{
    std::string column;
    std::string value;
    std::string op;
    std::vector<std::string> values;
};

class SQLBuilder
{
private:
    std::string selectClause;
    std::string insertClause;
    std::string deleteClause;
    std::string updateClause;
    std::string setClause;
    std::string fromClause;
    std::string whereClause;
    std::string orderByClause;
    std::string limitClause;
    std::string customQuery;
    std::string returningClause;
    bool isCustomQuery;
    std::vector<std::vector<where_condition>> whereConditions; // Change from string to vector of conditions

public:
    SQLBuilder() : isCustomQuery(false)
    {
        whereConditions.clear();
    }

    SQLBuilder &select(const std::vector<std::string> &columns);
    SQLBuilder &insert(const std::string &table, const std::vector<std::string> &columns, const std::vector<std::string> &values);
    SQLBuilder &deleteFrom(const std::string &table);
    SQLBuilder &update(const std::string &table);
    SQLBuilder &set(const std::vector<std::pair<std::string, std::string>> &updates);
    SQLBuilder &from(const std::string &table);
    SQLBuilder &where(const std::vector<where_condition> &conditions);
    SQLBuilder &orderBy(const std::string &columns);
    SQLBuilder &limit(const std::string &limit);
    SQLBuilder &query(const std::string &sql);
    SQLBuilder &returning(const std::vector<std::string> &column);

    std::string build() const;
};