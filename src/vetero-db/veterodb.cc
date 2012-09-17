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

#include <sstream>
#include <iterator>
#include <numeric>
#include <algorithm>
#include <memory>

#include <unistd.h>

#include <libbw/optionparser.h>
#include <libbw/log/debug.h>
#include <libbw/log/errorlog.h>
#include <libbw/completion.h>
#include <libbw/fileutils.h>

#include "common/dbaccess.h"
#include "common/consoleprogress.h"
#include "veterodb.h"
#include "config.h"

namespace vetero {
namespace db {

VeteroDb::VeteroDb()
    : common::VeteroApplication("vetero-db"),
      m_dbPath("vetero.db"),
      m_action(NoAction),
      m_readonly(false)
{}

bool VeteroDb::parseCommandLine(int argc, char *argv[])
{
    bw::OptionParser op;

    op.addOption("help", 'h', bw::OT_FLAG,
                 "Prints a help message and exits.");
    op.addOption("version", 'v', bw::OT_FLAG,
                 "Prints the version and exits.");
    op.addOption("database", 'd', bw::OT_STRING,
                 "Use the specified path as database instead of '" + m_dbPath + "'.");
    op.addOption("readonly", 'r', bw::OT_FLAG,
                 "Open the database readonly.");
    op.addOption("regenerate-metadata", 'M', bw::OT_FLAG,
                 "Regenerate all cached values in the database. This may take some time.");

    // do the parsing
    if (!op.parse(argc, argv))
        return false;

    // evaluate options
    if (op.getValue("help").getFlag()) {
        op.printHelp(std::cerr, "veterod " GIT_VERSION);
        return false;
    } else if (op.getValue("version").getFlag()) {
        std::cerr << "veterod " << GIT_VERSION << std::endl;
        return false;
    }

    // readonly?
    if (op.getValue("readonly"))
        m_readonly = true;

    // actions
    if (op.getValue("regenerate-metadata"))
        m_action = RegenerateMetadata;

    // database path
    if (op.getValue("database"))
        m_dbPath = op.getValue("database").getString();

    std::vector<std::string> args = op.getArgs();

    if (m_action == NoAction) {
        if (args.empty())
            m_action = InteractiveSql;
        else {
            m_action = ExecuteSql;
            std::ostringstream oss;
            std::copy(args.begin(), args.end(), std::ostream_iterator<std::string>(oss));
            m_sql = oss.str();
        }
    }

    setupErrorLogging("stderr");

    return true;
}

void VeteroDb::execRegenerateMetadata()
{
    BW_DEBUG_INFO("Regenerating metadata.");

    bool showProgress = isatty(STDIN_FILENO);

    std::auto_ptr<common::ConsoleProgress> progressNotifier;
    common::DbAccess dbAccess(&m_database);

    dbAccess.deleteStatistics();

    if (showProgress) {
        progressNotifier.reset(new common::ConsoleProgress(""));
        dbAccess.setProgressNotifier(progressNotifier.get());
    }

    if (showProgress)
        progressNotifier->reset("Day statistics");
    dbAccess.updateDayStatistics();

    if (showProgress)
        progressNotifier->reset("Month statistics");
    dbAccess.updateMonthStatistics();
}

void VeteroDb::execSql()
{
    if (m_sql.empty())
        throw common::ApplicationError("No SQL specified");

    runSqlStatement(m_sql);
}

void VeteroDb::execInteractiveSql()
{
    std::auto_ptr<bw::LineReader> lineReader(bw::LineReader::defaultLineReader("(vetero-db) "));
    std::string historyPath = bw::FileUtils::join(bw::FileUtils::homeDirectory(), ".vetero-db.history");

    try {
        lineReader->readHistory(historyPath);
    } catch (const bw::Error &error) {
        BW_ERROR_STREAM_WARNING("Unable to read history file '" << historyPath << "': " << error.what());
    }

    for (;;) {
        std::string line = lineReader->readLine();
        if (line == "exit" || line == "quit")
            break;
        else if (lineReader->eof()) {
            std::cout << std::endl;
            break;
        }

        try {
            runSqlStatement(line);
        } catch (const common::DatabaseError &dbError) {
            std::cerr << "Unable to execute SQL: " << dbError.what() << std::endl;
        }
    }

    try {
        lineReader->writeHistory(historyPath);
    } catch (const bw::Error &error) {
        BW_ERROR_STREAM_WARNING("Unable to read write file '" << historyPath << "': " << error.what());
    }

}

void VeteroDb::runSqlStatement(const std::string &stmt)
{
    common::Database::Result result = m_database.executeSqlQuery("%s", stmt.c_str());

    if (result.data.empty())
        return;

    std::vector<size_t> columnWidths(result.data.front().size());

    // count the size

    for (size_t col = 0; col < result.columnNames.size(); ++col)
        columnWidths[col] = std::max(columnWidths[col], result.columnNames[col].size());

    for (size_t line = 0; line < result.data.size(); ++line)
        for (size_t col = 0; col < result.data.front().size(); ++col)
            columnWidths[col] = std::max(columnWidths[col], result.data[line][col].size());


    // print it

    size_t totalWidth = std::accumulate(columnWidths.begin(), columnWidths.end(), 0);
    std::cout << "total=" << totalWidth << std::endl;
    totalWidth += 3 * columnWidths.size() + 1; // separator
    std::string line(totalWidth, '-');

    std::cout << line << std::endl;
    std::cout << "| ";
    for (size_t col = 0; col < result.columnNames.size(); ++col)
        std::cout << std::left << std::setw(columnWidths[col]) << result.columnNames[col] << " | ";
    std::cout << "\n" << line << std::endl;

    for (size_t line = 0; line < result.data.size(); ++line) {
        std::cout << "| ";
        for (size_t col = 0; col < result.data.front().size(); ++col)
            std::cout << std::setw(columnWidths[col]) << result.data[line][col] << " | ";
        std::cout << std::endl;
    }
    std::cout << line << std::endl;

}

void VeteroDb::exec()
{
    try {
        int flags = 0;
        if (m_readonly)
            flags |= common::Sqlite3Database::FLAG_READONLY;
        m_database.open(m_dbPath, flags);
    } catch (const vetero::common::DatabaseError &err) {
        throw common::ApplicationError("Unable to open DB: " + std::string(err.what()) );
    }

    switch (m_action) {
        case RegenerateMetadata:
            execRegenerateMetadata();
            break;

        case InteractiveSql:
            execInteractiveSql();
            break;

        case ExecuteSql:
            execSql();
            break;

        default:
            throw common::ApplicationError("No action specified.");
    }
}

} // namespace db
} // namespace vetero
