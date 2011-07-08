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

#ifndef VETERO_REPORTGEN_CURRENTREPORTGENERATOR_H_
#define VETERO_REPORTGEN_CURRENTREPORTGENERATOR_H_

#include "common/dataset.h"
#include "common/error.h"
#include "reportgenerator.h"

namespace vetero {
namespace reportgen {

class VeteroReportgen;

/**
 * \brief Generates the current report in the background. Also invokes the website upload.
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup report
 */
class CurrentReportGenerator : public ReportGenerator {

    public:
        /**
         * \brief Creates a new instance
         *
         * \param[in] reportGenerator the current weather data
         */
        CurrentReportGenerator(VeteroReportgen *reportGenerator);

        /**
         * \brief Does the work.
         *
         * \exception common::ApplicationError if something failed
         */
        void generateReports()
        throw (common::ApplicationError);

    protected:
        /**
         * \brief Returns a rendered text of the current weather
         *
         * \param[in] currentWeather the current weather which should be rendered
         * \return the text
         */
        std::string renderText(const common::CurrentWeather &currentWeather) const;

        /**
         * \brief Finds the template file on disk
         *
         * \return the path to the template or an empty string if the template could not be found
         */
        std::string findTemplate() const;
};

} // end namespace vetero
} // end namespace reportgen

#endif /* VETERO_REPORTGEN_CURRENTREPORTGENERATOR_H_ */
