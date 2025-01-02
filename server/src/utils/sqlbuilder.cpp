#include <string>
#include <vector>
#include <utils/sqlbuilder.hpp>
#include <utils/logger.hpp>
#include <stdexcept>

SQLBuilder &SQLBuilder::select(const std::vector<std::string> &columns)
{
    try
    {
        if (columns.empty())
        {
            Logger::error({"No columns specified for SELECT query"});
            throw std::invalid_argument("No columns specified for SELECT query");
        }

        selectClause = "SELECT ";
        for (const auto &column : columns)
        {
            selectClause += column + ", ";
        }
        selectClause = selectClause.substr(0, selectClause.length() - 2);
        return *this;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Error building SELECT clause: " + std::string(e.what())});
        throw;
    }
}

SQLBuilder &SQLBuilder::insert(const std::string &table, const std::vector<std::string> &columns, const std::vector<std::string> &values)
{
    try
    {
        if (table.empty())
        {
            Logger::error({"No table specified for INSERT query"});
            throw std::invalid_argument("No table specified for INSERT query");
        }
        if (columns.empty())
        {
            Logger::error({"No columns specified for INSERT query"});
            throw std::invalid_argument("No columns specified for INSERT query");
        }
        if (values.empty())
        {
            Logger::error({"No values specified for INSERT query"});
            throw std::invalid_argument("No values specified for INSERT query");
        }

        insertClause = "INSERT INTO " + table + " (";
        for (const auto &column : columns)
        {
            insertClause += column + ", ";
        }
        insertClause = insertClause.substr(0, insertClause.length() - 2);
        insertClause += ") VALUES (";
        for (const auto &value : values)
        {
            insertClause += "'" + value + "', ";
        }
        insertClause = insertClause.substr(0, insertClause.length() - 2);
        insertClause += ")";

        return *this;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Error building INSERT clause: " + std::string(e.what())});
        throw;
    }
}

SQLBuilder &SQLBuilder::deleteFrom(const std::string &table)
{
    try
    {
        if (table.empty())
        {
            Logger::error({"No table specified for DELETE query"});
            throw std::invalid_argument("No table specified for DELETE query");
        }
        deleteClause = "DELETE FROM " + table;
        return *this;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Error building DELETE clause: " + std::string(e.what())});
        throw;
    }
}

SQLBuilder &SQLBuilder::update(const std::string &table)
{
    try
    {
        if (table.empty())
        {
            Logger::error({"No table specified for UPDATE query"});
            throw std::invalid_argument("No table specified for UPDATE query");
        }
        updateClause = "UPDATE " + table;
        return *this;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Error building UPDATE clause: " + std::string(e.what())});
        throw;
    }
}

SQLBuilder &SQLBuilder::set(const std::vector<std::pair<std::string, std::string>> &updates)
{
    try
    {
        if (updates.empty())
        {
            Logger::error({"No updates specified for SET clause"});
            throw std::invalid_argument("No updates specified for SET clause");
        }

        setClause = " SET ";
        for (const auto &update : updates)
        {
            setClause += update.first + " = '" + update.second + "', ";
        }
        setClause = setClause.substr(0, setClause.length() - 2);
        return *this;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Error building SET clause: " + std::string(e.what())});
        throw;
    }
}

SQLBuilder &SQLBuilder::from(const std::string &table)
{
    try
    {
        if (table.empty())
        {
            Logger::error({"No table specified for FROM clause"});
            throw std::invalid_argument("No table specified for FROM clause");
        }

        fromClause = " FROM " + table;
        return *this;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Error building FROM clause: " + std::string(e.what())});
        throw;
    }
}

SQLBuilder &SQLBuilder::where(const std::vector<where_condition> &conditions)
{
    try
    {
        if (!conditions.empty())
        {
            whereConditions.push_back(conditions);
        }
        return *this;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Error building WHERE clause: " + std::string(e.what())});
        throw;
    }
}

SQLBuilder &SQLBuilder::orderBy(const std::string &columns)
{
    try
    {
        if (columns.empty())
        {
            Logger::error({"No columns specified for ORDER BY clause"});
            throw std::invalid_argument("No columns specified for ORDER BY clause");
        }
        orderByClause = " ORDER BY " + columns;
        return *this;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Error building ORDER BY clause: " + std::string(e.what())});
        throw;
    }
}

SQLBuilder &SQLBuilder::limit(const std::string &limit)
{
    try
    {
        if (limit.empty())
        {
            Logger::error({"No limit specified for LIMIT clause"});
            throw std::invalid_argument("No limit specified for LIMIT clause");
        }
        limitClause = " LIMIT " + limit;
        return *this;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Error building LIMIT clause: " + std::string(e.what())});
        throw;
    }
}

SQLBuilder &SQLBuilder::query(const std::string &sql)
{
    try
    {
        if (sql.empty())
        {
            Logger::error({"No query specified for custom query"});
            throw std::invalid_argument("No query specified for custom query");
        }

        customQuery = sql;
        isCustomQuery = true;
        return *this;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Error building custom query: " + std::string(e.what())});
        throw e;
    }
}

SQLBuilder &SQLBuilder::returning(const std::vector<std::string> &column)
{
    try
    {
        if (column.empty())
        {
            Logger::error({"No column specified for RETURNING clause"});
            throw std::invalid_argument("No column specified for RETURNING clause");
        }
        returningClause = " RETURNING ";
        for (const auto &col : column)
        {
            returningClause += col + ", ";
        }
        returningClause = returningClause.substr(0, returningClause.length() - 2);
        return *this;
    }
    catch (const std::exception &e)
    {
        Logger::error({"Error building RETURNING clause: " + std::string(e.what())});
        throw;
    }
}

std::string SQLBuilder::build() const
{
    try
    {
        Logger::debug({"Building SQL query..."});

        if (isCustomQuery)
        {
            Logger::debug({"Using custom query: " + customQuery});
            return customQuery;
        }

        std::string sql;
        if (!insertClause.empty())
        {
            sql = insertClause;
        }
        else if (!deleteClause.empty())
        {
            sql = deleteClause;
        }
        else if (!updateClause.empty())
        {
            sql = updateClause;
            if (setClause.empty())
            {
                throw std::runtime_error("SET clause is required for update queries");
            }
            sql += setClause;
        }
        else
        {
            if (selectClause.empty())
            {
                throw std::runtime_error("SELECT clause is required for select queries");
            }
            if (fromClause.empty())
            {
                throw std::runtime_error("FROM clause is required for select queries");
            }

            sql = selectClause + fromClause;
            if (!orderByClause.empty())
            {
                sql += orderByClause;
            }
            if (!limitClause.empty())
            {
                sql += limitClause;
            }
        }

        if (!returningClause.empty())
        {
            sql += returningClause;
        }

        if (!whereConditions.empty())
        {
            sql += " WHERE ";
            bool firstGroup = true;

            for (const auto &conditionGroup : whereConditions)
            {
                if (!firstGroup)
                {
                    sql += " AND ";
                }
                firstGroup = false;

                bool firstCondition = true;
                for (const auto &condition : conditionGroup)
                {
                    if (!firstCondition)
                    {
                        sql += " AND ";
                    }
                    firstCondition = false;

                    if (condition.op == "LIKE")
                    {
                        sql += condition.column + " " + condition.op + " '%" + condition.value + "%'";
                    }
                    else if (condition.op == "IN")
                    {
                        std::string values = "(";
                        for (const auto &value : condition.values)
                        {
                            values += value + ", ";
                        }
                        values = values.substr(0, values.length() - 2) + ")";
                        sql += condition.column + " " + condition.op + " " + values;
                    }
                    else
                    {
                        if (condition.value == "true" || condition.value == "false")
                        {
                            sql += condition.column + " " + condition.op + " " + condition.value;
                        }
                        else
                        {
                            sql += condition.column + " " + condition.op + " '" + condition.value + "'";
                        }
                    }
                }
            }
        }
        Logger::debug({"Built SQL query: " + sql});

        return sql + ";";
    }
    catch (const std::exception &e)
    {
        Logger::error({"Error building SQL query: " + std::string(e.what())});
        throw e;
    }
}
