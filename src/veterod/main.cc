/* {{{
 * (c) 2010, Bernhard Walle <bernhard@bwalle.de>
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

#include <libbw/log/errorlog.h>

#include "veterod.h"

int main(int argc, char *argv[])
{
    vetero::daemon::Veterod veterod;

    try {
        if (!veterod.parseCommandLine(argc, argv))
            return EXIT_SUCCESS;
        veterod.installSignalhandlers();
        veterod.readConfiguration();
        veterod.exec();
    } catch (const vetero::common::ApplicationError &err) {
        BW_ERROR_CRIT("%s", err.what());
        return EXIT_FAILURE;
    } catch (const std::exception &err) {
        BW_ERROR_CRIT("Standard exception: %s", err.what());
        return EXIT_FAILURE;
    } catch (...) {
        BW_ERROR_CRIT("Unknown exception caught.");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
