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

#ifndef VETERO_COMMON_CONSOLEPROGRESS_H_
#define VETERO_COMMON_CONSOLEPROGRESS_H_

#include <string>

#include "dbaccess.h"

namespace vetero {
namespace common {

class ConsoleProgress : public ProgressNotifier {

public:
    ConsoleProgress(const std::string &title);
    ~ConsoleProgress();

public:
    void progressed(double total, double now);
    void finished();
    void reset(const std::string &newTitle);

private:
    std::string m_title;
    int m_barWidth;
    bool m_first;
};

} // end namespace common
} // end namespace vetero

#endif // VETERO_COMMON_CONSOLEPROGRESS_H_
