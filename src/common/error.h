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
#ifndef VETERO_COMMON_ERROR_H_
#define VETERO_COMMON_ERROR_H_

#include <string>
#include <stdexcept>
#include <cstring>

namespace vetero {
namespace common {

/* ApplicationError {{{ */

/**
 * \class ApplicationError
 * \brief General exception class in the Vetero application
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup common
 */
class ApplicationError : public std::runtime_error {

    public:
        /**
         * \brief Constructor
         *
         * Creates a new ApplicationError.
         *
         * \param[in] string the error string
         */
        ApplicationError(const std::string &string)
            : std::runtime_error(string) {}

        /**
         * \brief Destructor
         */
        virtual ~ApplicationError()
        throw () {}
};

/* }}} */
/* SystemError {{{ */

/**
 * \class SystemError
 *
 * \brief Exception class for system errors
 *
 * A system error is an error triggered by the operating system with an appropriate
 * \c errno value.
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup common
 */
class SystemError : public ApplicationError {
    public:

        /**
         * \brief Constructor
         *
         * Creates a new instance of SystemError.
         *
         * \param[in] msg the error string
         * \param[in] err the system errno value (positive), e.g. \c EINVAL.
         */
        SystemError(const std::string &msg, int err=errno)
            : ApplicationError(msg)
        {
            m_errorstring = msg + " (" + std::strerror(err) + ")";
        }

        virtual ~SystemError()
        throw () {}

        /**
         * \brief Formats the error.
         *
         * Overwritten member function (std::runtime_error::what()).
         *
         * \return the error message as pointer to an internal string buffer. The string is
         *         only valid until this SystemError is freed.
         */
        virtual const char *what() const throw ()
        {
            return m_errorstring.c_str();
        }

    private:
        std::string m_errorstring;
};

/* }}} */
/* SystemError {{{ */

/**
 * \class SystemError
 *
 * \brief Exception class for system errors
 *
 * A network error is an error triggered by the operating system's network name resolution stack
 * with an appropriate \c errno value.
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup common
 */
class NetworkAddressError : public ApplicationError {
    public:

        /**
         * \brief Constructor
         *
         * Creates a new instance of SystemError.
         *
         * \param[in] msg the error string
         * \param[in] err the error value of getaddrinfo()
         */
        NetworkAddressError(const std::string &msg, int err);

        /**
         * \brief Formats the error.
         *
         * Overwritten member function (std::runtime_error::what()).
         *
         * \return the error message as pointer to an internal string buffer. The string is
         *         only valid until this SystemError is freed.
         */
        virtual const char *what() const throw ()
        {
            return m_errorstring.c_str();
        }

    private:
        std::string m_errorstring;
};

/* }}} */
/* DatabaseError {{{ */

/**
 * \class DatabaseError
 * \brief Exception class used in the database code.
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup common
 */
class DatabaseError : public ApplicationError {

    public:
        /**
         * \brief Constructor
         *
         * Creates a new DatabaseError.
         *
         * \param[in] string the error string
         */
        DatabaseError(const std::string &string)
            : ApplicationError(string) {}

        /**
         * \brief Destructor
         */
        virtual ~DatabaseError()
        throw () {}
};

/* }}} */
/* HttpError {{{ */

/**
 * \class HttpError
 * \brief Exception class for http used in the database code.
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup common
 */
class HttpError : public ApplicationError {

    public:
        /**
         * \brief Constructor
         *
         * Creates a new DatabaseError.
         *
         * \param[in] string the error string
         */
        HttpError(const std::string &string)
            : ApplicationError(string) {}

        /**
         * \brief Destructor
         */
        virtual ~HttpError()
        throw () {}
};

/* }}} */


} // end namespace vetero
} // end namespace common

#endif // VETERO_COMMON_ERROR_H_
