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
            insertClause += value + ", ";
        }
        insertClause = insertClause.substr(0, insertClause.length() - 2);
        insertClause += ")";

        Logger::debug({"Built INSERT clause: " + insertClause});
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
        if (conditions.empty())
        {
            return *this; // Make where clause optional
        }

        whereClause = " WHERE ";
        for (const auto &condition : conditions)
        {
            if (condition.op == "LIKE")
            {
                whereClause += condition.column + " " + condition.op + " '%" + condition.value + "%' AND ";
            }
            else if (condition.op == "IN")
            {
                std::string values = "(";
                for (const auto &value : condition.values)
                {
                    values += value + ", ";
                }
                values = values.substr(0, values.length() - 2) + ")";
                whereClause += condition.column + " " + condition.op + " " + values + " AND ";
            }
            else
            {
                whereClause += condition.column + " " + condition.op + " '" + condition.value + "' AND ";
            }
        }
        whereClause = whereClause.substr(0, whereClause.length() - 5);
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

SQLBuilder &SQLBuilder::returning(const std::string &column)
{
    try
    {
        if (column.empty())
        {
            Logger::error({"No column specified for RETURNING clause"});
            throw std::invalid_argument("No column specified for RETURNING clause");
        }
        returningClause = " RETURNING " + column;
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
            Logger::debug({"Using INSERT query: " + sql});
        }
        else if (!deleteClause.empty())
        {
            sql = deleteClause;
            if (!whereClause.empty())
            {
                sql += whereClause;
            }
            Logger::debug({"Using DELETE query: " + sql});
        }
        else
        {
            // SELECT query
            if (selectClause.empty())
            {
                throw std::runtime_error("SELECT clause is required for select queries");
            }
            if (fromClause.empty())
            {
                throw std::runtime_error("FROM clause is required for select queries");
            }

            sql = selectClause + fromClause;
            if (!whereClause.empty())
            {
                sql += whereClause;
            }
            if (!orderByClause.empty())
            {
                sql += orderByClause;
            }
            if (!limitClause.empty())
            {
                sql += limitClause;
            }
            Logger::debug({"Using SELECT query: " + sql});
        }

        if (!returningClause.empty())
        {
            sql += returningClause;
        }

        return sql + ";";
    }
    catch (const std::exception &e)
    {
        Logger::error({"Error building SQL query: " + std::string(e.what())});
        throw e;
    }
}