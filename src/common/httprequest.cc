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

#define CURL_STRICTER
#include <curl/curl.h>

#include <stdexcept>

#include "httprequest.h"

namespace vetero::common {

HttpRequest::HttpRequest(const std::string &url)
{
    m_curl = curl_easy_init();
    if (!m_curl)
        throw std::bad_alloc();

    curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
}

HttpRequest::~HttpRequest()
{
    curl_easy_cleanup(m_curl);
}

void HttpRequest::perform()
{
    CURLcode res = curl_easy_perform(m_curl);
    if (res != CURLE_OK)
        throw HttpError( curl_easy_strerror(res) );
}

/* }}} */

} // vetero::common
