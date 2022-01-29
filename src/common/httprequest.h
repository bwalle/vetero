/* {{{
 * (c) 2021, Bernhard Walle <bernhard@bwalle.de>
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
#ifndef VETERO_HTTPREQUEST_H_
#define VETERO_HTTPREQUEST_H_

#include <string>

#include "error.h"

typedef struct Curl_easy CURL;

namespace vetero::common {

/* HttpRequest {{{ */

class HttpRequest {

public:
    HttpRequest(const std::string &url);
    ~HttpRequest();

    void perform();

private:
    CURL *m_curl = nullptr;
};

/* }}} */


} // vetero::common


#endif // VETERO_HTTPREQUEST_H_