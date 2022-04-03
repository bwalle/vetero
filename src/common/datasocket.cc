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
#include <unistd.h>

#include "datasocket.h"
#include "error.h"

namespace vetero {
namespace common {

DataSocket::DataSocket(const std::string &host, int port)
{
    connect(host, port);
}

DataSocket::~DataSocket()
{
    try {
        close();
    } catch (...) {}
}

void DataSocket::connect(const std::string &host, int port)
{
    close();
    
    struct addrinfo hints, *res0;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;

    std::string portString = std::to_string(port);
    int error = getaddrinfo(host.c_str(), portString.c_str(), &hints, &res0);
    if (error) {
        freeaddrinfo(res0);
        throw NetworkAddressError("Unable to resolve host name \"" + host + "\"", error);
    }

    std::string cause;
    for (struct addrinfo *res = res0; res; res = res->ai_next) {
        int socket = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (socket < 0) {
            cause = "socket";
            continue;
        }

        if (::connect(socket, res->ai_addr, res->ai_addrlen) < 0) {
            cause = "connect";
            ::close(socket);
            continue;
        }

        m_socket = socket;
        break; /* okay we got one */
    }
    freeaddrinfo(res0);

    if (m_socket == INVALID_SOCKET)
        throw SystemError("Unable to " + cause + " to \"" + host + "\"");
}

ssize_t DataSocket::read(void *buf, size_t nbyte)
{
    ssize_t result = ::read(m_socket, buf, nbyte);
    if (result < 0)
        throw SystemError("Unable to read from socket");
    
    return result;
}

ssize_t DataSocket::write(const void *buf, size_t nbyte)
{
    ssize_t result = ::write(m_socket, buf, nbyte);
    if (result < 0)
        throw SystemError("Unable to write to the socket");

    return result;
}

void DataSocket::close()
{
    if (m_socket == INVALID_SOCKET)
        return;
    
    if (::close(m_socket) != 0) 
        throw SystemError("Unable to close socket");
    
    m_socket = INVALID_SOCKET;
}



} // end namespace common
} // end namespace vetero

