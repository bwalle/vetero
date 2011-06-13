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

#include <cstdlib>
#include <cerrno>
#include <cstdio>
#include <limits.h>

#include <libbw/stringutil.h>

#include "gnuplot.h"

namespace vetero {
namespace reportgen {

/* Gnuplot {{{ */

// -------------------------------------------------------------------------------------------------
std::string Gnuplot::PLACEHOLDER = "@TEMPFILE@";

// -------------------------------------------------------------------------------------------------
Gnuplot::Gnuplot(const common::Configuration &config)
    : m_config(config)
{
    *this << "set locale '" << m_config.getLocale() << "'\n";
    *this << "set terminal svg size 1000 400 font 'Arial,9'\n";
    *this << "set lmargin 10\n";
    *this << "set rmargin 10\n";
}

// -------------------------------------------------------------------------------------------------
Gnuplot::~Gnuplot()
{}

// -------------------------------------------------------------------------------------------------
std::string Gnuplot::workingDirectory() const
{
    return m_workingDirectory;
}

// -------------------------------------------------------------------------------------------------
void Gnuplot::setWorkingDirectory(const std::string &workingdir)
{
    m_workingDirectory = workingdir;
}

// -------------------------------------------------------------------------------------------------
void Gnuplot::plot(const StringStringVector &data)
    throw (common::ApplicationError)
{
    char tempname[] = "/tmp/vetero-plot-XXXXXX";
    int tempfd = mkstemp(tempname);
    if (tempfd < 0)
        throw common::SystemError("Unable to create temporary file", errno);

    char oldWorkingdir[PATH_MAX] = "";
    try {
        storeData(tempfd, data);
        std::string gnuplotCommands = m_stream.str();

        // replace the placeholder
        std::string::size_type tempPos = -1;
        do {
            tempPos = gnuplotCommands.find(PLACEHOLDER, tempPos+1);
            if (tempPos != std::string::npos)
                gnuplotCommands.replace(tempPos, PLACEHOLDER.size(), tempname);
        } while (tempPos != std::string::npos);

        // change to the working directory as popen() doesn't have the possibility to set
        // the working directory and gnuplot doesn't work with absolute paths
        if (!m_workingDirectory.empty()) {
            getcwd(oldWorkingdir, sizeof(oldWorkingdir));

            int ret = chdir(m_workingDirectory.c_str());
            if (ret != 0)
                throw common::SystemError("Unable to change working directory to '" +
                                          m_workingDirectory + "'", errno);
        }

        std::FILE *gnuplotProcess = popen("gnuplot", "w");
        if (!gnuplotProcess)
            throw common::SystemError("Unable to execute 'gnuplot'", errno);

        if (fputs(gnuplotCommands.c_str(), gnuplotProcess) == EOF)
            throw common::SystemError("Unable to write to gnuplot", errno);

        int ret = pclose(gnuplotProcess);
        if (ret != 0)
            throw common::ApplicationError("Unable to generate diagram, Gnuplot terminated with " +
                                           bw::str(WEXITSTATUS(ret)));
    } catch (...) {
        unlink(tempname);
        close(tempfd);
        if (*oldWorkingdir)
            chdir(oldWorkingdir);
        throw;
    }

    unlink(tempname);
    close(tempfd);
    if (*oldWorkingdir)
        chdir(oldWorkingdir);
}

// -------------------------------------------------------------------------------------------------
void Gnuplot::storeData(int fd, const StringStringVector &data)
    throw (common::ApplicationError)
{
    std::stringstream ss;

    bool firstLine = true;
    for (StringStringVector::const_iterator lineIter = data.begin(); lineIter != data.end(); ++lineIter) {
        const StringVector &line = *lineIter;

        if (firstLine)
            firstLine = false;
        else
            ss << "\n";

        bool firstCol = true;
        for (StringVector::const_iterator colIter = line.begin(); colIter != line.end(); ++colIter) {
            if (firstCol)
                firstCol = false;
            else
                ss << "\t";
            ss << *colIter;
        }
    }

    std::string contents = ss.str();

    ssize_t ret = write(fd, contents.c_str(), contents.size());
    if (ret != contents.size())
        throw common::SystemError("Unable to write all bytes to the temporary file", errno);
}

/* }}} */
/* WeatherGnuplot {{{ */

// -------------------------------------------------------------------------------------------------
WeatherGnuplot::WeatherGnuplot(const common::Configuration &config)
    : Gnuplot(config)
{}

// -------------------------------------------------------------------------------------------------
void WeatherGnuplot::addWindY()
{
    *this << "set ylabel \"Windgeschwindigkeit [km/h]\"\n";
    *this << "set y2label \"Windstärke [Beaufort]\"\n";
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
