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
#ifndef VETERO_COMMON_DATASOCKET_H_
#define VETERO_COMMON_DATASOCKET_H_

#include <string>

namespace vetero {
namespace common {

/* DataSocket {{{ */

/**
 * @brief Simple TCP socket
 * 
 * This is a simple TCP socket implementation for Unix, just what we need, without
 * any type hierarchy and not copyable.
 * 
 */
class DataSocket
{
public:
    /**
     * @brief Constant for the invalid socket file descriptor.
     */
    const int INVALID_SOCKET = -1;

public:
    /**
     * @brief Construct a new Data Socket object
     */
    DataSocket() = default;

    /**
     * @brief Construct a new Data Socket object
     * 
     * @param[in] host a hostname or an IP address
     * @param[in] port the port number
     * @exception NetworkAddressError if the name resolution fails
     * @exception SystemError any other error
     */
    DataSocket(const std::string &host, int port);

    /**
     * @brief Destroy the Data Socket object
     */
    ~DataSocket();

    /**
     * @brief Delete the copy constructor
     */
    DataSocket(const DataSocket &) = delete;

    /**
     * @brief Delete assignment operator
     */
    DataSocket &operator=(const DataSocket &) = delete;

    /**
     * @brief Move constructor
     */
    DataSocket(DataSocket &&other) : m_socket(other.m_socket) {
        other.m_socket = INVALID_SOCKET;
    }

    /**
     * @brief Movement operator
     */
    DataSocket &operator=(DataSocket &&other) {
        this->m_socket = other.m_socket;
        other.m_socket = INVALID_SOCKET;
        return *this;
    }

public:
    /**
     * @brief Creates a TCP connection to @host and @port 
     * 
     * @param[in] host a hostname or an IP address
     * @param[in] port the port number
     * @exception NetworkAddressError if the name resolution fails
     * @exception SystemError any other error
     */
    void connect(const std::string &host, int port);

    /**
     * @brief Waits for data available for read
     *
     * Blocks until data is ready to be read.
     *
     * @retval true data is available
     * @retval false timeout reached
     * @exception SystemError on any errro
     */
    bool waitForRead(int timeout_ms);

    /**
     * @brief Reads from the socket
     * 
     * If @c timeout_ms is non-zero, times out when no data is available after
     * @c timeout_ms milliseconds. Then a value of 0 is returend.
     *
     * @param[out] buf the output buffer
     * @param[in] nbyte length of the buffer
     * @param[in] timeout_ms timeout if non-zero, waits only @c timeout_ms milliseconds.
     * @return ssize_t the number of bytes read
     * @exception SystemError on any errro
     */
    ssize_t read(void *buf, size_t nbyte, int timeout_ms=0);

    /**
     * @brief Writes to the socket
     * 
     * @param[in] buf the data buffer
     * @param[in] nbyte length of the buffer
     * @return ssize_t the number of bytes written
     * @exception SystemError on any errro
     */
     ssize_t write(const void *buf, size_t nbyte);

    /**
     * @brief Closes the socket
     * 
     * If there's no open socket, the function does nothing. The socket is also closed in the
     * destructor.
     */
    void close();

private:
    int m_socket = INVALID_SOCKET;
};

/* }}} */

} // end namespace common
} // end namespace vetero

#endif // DATASOCKET_H_
