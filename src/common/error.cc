/* {{{
 * (c) 2022, Bernhard Walle <bernhard@bwalle.de>
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

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "error.h"

namespace vetero {
namespace common {

NetworkAddressError::NetworkAddressError(const std::string &msg, int err)
    : ApplicationError(msg)
{
    m_errorstring = msg + " (" + gai_strerror(err) + ")";
}


} // end namespace common
} // end namespace vetero
