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

#include <sys/ioctl.h>

#include "consoleprogress.h"

namespace vetero {
namespace common {

const int TITLE_WIDTH = 20;
const int PERCENT_WIDTH = 6;

ConsoleProgress::ConsoleProgress(const std::string &title)
{
    int totalWidth = 80;

    struct ttysize ts;
    int ret = ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
    if (ret >= 0)
        totalWidth = ts.ts_cols;

    // 1 space between title and bar, 1 space between percentage and title
    // and two characters for the '|'
    m_barWidth = totalWidth - 1 - TITLE_WIDTH - 1 - PERCENT_WIDTH - 2;

    reset(title);
}

ConsoleProgress::~ConsoleProgress()
{
    finished();
}

void ConsoleProgress::progressed(double total, double now)
{
    if (m_first)
        m_first = false;
    else
        dprintf(STDOUT_FILENO, "\r");

    int percentage = 0;
    double percent = 0.0;
    if (total >= 0.01) { // avoid division by 0
        percent = now / total;
        percentage = int(percent * m_barWidth);
    }

    std::string bar;
    bar += '|';

    for (size_t i = 0; i < m_barWidth; i++) {
        if (i == percentage)
            bar += '>';
        else if (i < percentage)
            bar += '=';
        else
            bar += '-';
    }

    bar += '|';

    dprintf(STDOUT_FILENO, "%-*s %s %5.1lf%%", TITLE_WIDTH, m_title.c_str(), bar.c_str(), percent*100.0);
}

void ConsoleProgress::reset(const std::string &newTitle)
{
    m_title = newTitle;

    if (m_title.size() > TITLE_WIDTH)
        m_title.erase(TITLE_WIDTH-1);

    m_first = true;
}

void ConsoleProgress::finished()
{
    progressed(100, 100);
}

} // end namespace common
} // end namespace vetero
