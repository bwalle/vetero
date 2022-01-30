/* {{{
 * (c) 2011, Bernhard Walle <bernhard@bwalle.de>
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

#include <iostream>
#include <cstdlib>
#include <vector>
#include <cerrno>
#include <cstdio>
#include <limits.h>
#include <unistd.h>

#include <libbw/stringutil.h>
#include <libbw/io/tempfile.h>
#include <libbw/log/errorlog.h>
#include <libbw/log/debug.h>

#include "common/translation.h"
#include "common/utils.h"
#include "gnuplot.h"

namespace vetero {
namespace reportgen {

/* Gnuplot {{{ */

std::string Gnuplot::PLACEHOLDER = "-";

Gnuplot::Gnuplot(const common::Configuration &config)
    : m_config(config),
      m_writeToFile(false)
{
    *this << "set locale '" << m_config.locale() << "'\n";
    *this << "set terminal svg size 1000 400 font 'Arial,9'\n";
    *this << "set lmargin 10\n";
    *this << "set rmargin 10\n";

    if (getenv("VETERO_GNUPLOT_FILE"))
        m_writeToFile = true;
}

Gnuplot::~Gnuplot()
{}

std::string Gnuplot::workingDirectory() const
{
    return m_workingDirectory;
}

void Gnuplot::setWorkingDirectory(const std::string &workingdir)
{
    m_workingDirectory = workingdir;
}

std::string Gnuplot::outputFile() const
{
    return m_outputFile;
}

void Gnuplot::setOutputFile(const std::string &output)
{
    m_outputFile = output;

    *this << "set output '" << m_outputFile << "'\n";
}

void Gnuplot::plot(const StringStringVector &data, int columns)
{
    if (data.empty() || data[0].empty()) {
        BW_ERROR_WARNING("Gnuplot: No data to plot for '%s'", m_outputFile.c_str());
        return;
    }

    try {
        std::string gnuplotCommands = m_stream.str();

        bw::io::TempFile errorTempfile("vetero-plot-error", bw::io::TempFile::DeleteOnExit);
        std::string gnuplotCommand("gnuplot");
        gnuplotCommand += " 2>" + errorTempfile.name();

        int (*fileCloseFunction)(FILE *fp);
        FILE *gnuplotFp;
        if (m_writeToFile) {
            std::string plotname = m_outputFile;
            plotname = bw::replace_char(plotname, '/', "_");
            plotname = bw::replace_char(plotname, '.', "_");

            std::string filename = "/tmp/vetero_" + plotname + ".plot";
            BW_DEBUG_INFO("Writing output to '%s'", filename.c_str());

            gnuplotFp = fopen(filename.c_str(), "w");
            fileCloseFunction = fclose;
        } else {
            gnuplotFp = popen(gnuplotCommand.c_str(), "w");
            fileCloseFunction = pclose;
        }

        if (!gnuplotFp)
            throw common::SystemError("Unable to execute 'gnuplot'", errno);

        if (fputs(gnuplotCommands.c_str(), gnuplotFp) == EOF)
            throw common::SystemError("Unable to write to gnuplot", errno);

        storeData(gnuplotFp, data, columns);

        int ret = fileCloseFunction(gnuplotFp);
        if (ret != 0) {
            dumpError(errorTempfile.nativeHandle());
            throw common::ApplicationError("Unable to generate diagram, Gnuplot terminated with " +
                                           bw::str(WEXITSTATUS(ret)));
        }
    } catch (const bw::IOError &err) {
        throw common::ApplicationError(err.what());
    }
    std::string outputfile = common::realpath(m_outputFile);

    common::compress_file(outputfile);
}

void Gnuplot::storeData(FILE *fp, const StringStringVector &data, int columns)
{
    if (data.empty()) {
        BW_ERROR_ERR("Attempting to plot empty data");
        return;
    }

    // since gnuplot cannot seek in stdin, we need to provide the data multiple times
    if (columns == 0)
        columns = data.front().size()-1;

    for (int i = 0; i < columns; i++) {

        StringStringVector::const_iterator lineIter;
        for (lineIter = data.begin(); lineIter != data.end(); ++lineIter) {
            const StringVector &line = *lineIter;

            for (int col = 0; col <= columns; col++) {
                if (col != 0) {
                    if (fputs("\t", fp) == EOF)
                        throw common::SystemError("Unable to write to the Gnuplot process", errno);
                }

                if (fputs(line[col].c_str(), fp) == EOF)
                    throw common::SystemError("Unable to write to the Gnuplot process", errno);
            }

            fputs("\n", fp);
        }
        // "e\n" is the separator for Gnuplot
        fputs("e\n", fp);
    }
}

void Gnuplot::dumpError(int fd)
{
    off_t ret = lseek(fd, 0, SEEK_SET);
    if (ret == (off_t)-1) {
        BW_ERROR_WARNING("%s: Unable to seek fd %d: %s", __func__, fd, std::strerror(errno));
        return;
    }

    std::string errors;
    char buffer[BUFSIZ];
    ssize_t nbytes;
    while ((nbytes = read(fd, buffer, BUFSIZ-1)) > 0) {
        buffer[nbytes] = '\0';
        errors += buffer;
    }

    std::vector<std::string> lines = bw::stringsplit(errors, "\n");
    std::vector<std::string>::const_iterator lineIter;
    for (lineIter = lines.begin(); lineIter != lines.end(); ++lineIter) {
        std::string stripped = bw::strip(*lineIter);
        if (stripped.empty() || stripped == "^")
            continue;
        BW_ERROR_WARNING("Error output of gnuplot: %s", lineIter->c_str());
    }
}

/* }}} */
/* WeatherGnuplot {{{ */

WeatherGnuplot::WeatherGnuplot(const common::Configuration &config)
    : Gnuplot(config)
{}

void WeatherGnuplot::addWindY()
{
    *this << "set ylabel '" << _("Wind speed [km/h])") << "'\n";
    *this << "set y2label '" << _("Wind strength [Beaufort]") << "'\n";
    *this << "set ytics nomirror\n";
    *this << "set y2tics ( '0'   0, "
                         " '1'   2, "
                         " '2'   6, "
                         " '3'  12, "
                         " '4'  20, "
                         " '5'  29, "
                         " '6'  39, "
                         " '7'  50, "
                         " '8'  62, "
                         " '9'  75, "
                         "'10'  89, "
                         "'11' 103, "
                         "'12' 117)\n";
    *this << "set grid xtics noytics y2tics\n";
}

/* }}} */

} // namespace reportgen
} // namespace vetero
