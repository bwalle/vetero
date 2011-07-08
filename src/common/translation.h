/* {{{
 * (c) 2011, Bernhard Walle <bernhard@bwalle.de>
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
#ifndef VETERO_COMMON_TRANSLATION_H_
#define VETERO_COMMON_TRANSLATION_H_

#include <libintl.h>

/**
 * \brief Marks a runtime-translated string
 *
 * Common wrapper around gettext().
 *
 * \param[in] str the string that needs to be translated
 * \ingroup common
 */
#define _(str) gettext(str)

#endif // VETERO_COMMON_TRANSLATION_H_
