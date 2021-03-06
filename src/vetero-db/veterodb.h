/* {{{
 * (c) 2012, Bernhard Walle <bernhard@bwalle.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>. }}}
 */
#ifndef VETERO_VETERO_DB_VETERODB_H_
#define VETERO_VETERO_DB_VETERODB_H_

#include "common/database.h"
#include "common/veteroapplication.h"

namespace vetero {
namespace db {

class VeteroDb : public common::VeteroApplication
{
public:
    enum Action {
        NoAction,
        ExecuteSql,
        RegenerateMetadata,
        InteractiveSql
    };

public:
    VeteroDb();
    bool parseCommandLine(int argc, char *argv[]);
    void exec();

private:
    void execRegenerateMetadata();
    void execSql();
    void execInteractiveSql();
    void runSqlStatement(const std::string &stmt);
    void printResultPretty(const common::Database::Result &result);
    void printResultMachineReadable(const common::Database::Result &result);

private:
    std::string m_sql;
    Action m_action;
    std::string m_dbPath;
    bool m_machineReadable;
    vetero::common::Sqlite3Database m_database;
    bool m_readonly;
};

} // namespace db
} // namespace vetero

#endif // VETERO_VETERO_DB_VETERODB_H_
