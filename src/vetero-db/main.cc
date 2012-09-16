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

#include "veterodb.h"

int main(int argc, char *argv[])
{
    vetero::db::VeteroDb veterodb;

    try {
        if (!veterodb.parseCommandLine(argc, argv))
            return EXIT_SUCCESS;
        veterodb.exec();
    } catch (const vetero::common::ApplicationError &err) {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const std::exception &err) {
        std::cerr << "Standard exception: " << err.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown exception caught." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
