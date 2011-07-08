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

#ifndef REPORTGENERATOR_H_
#define REPORTGENERATOR_H_

#include "common/lockfile.h"
#include "vetero_reportgen.h"

namespace vetero {
namespace reportgen {

/**
 * @brief Base class for all report generator classes
 *
 * All report generators should inherit from ReportGenerator.
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup report
 */
class ReportGenerator
{
    public:
        /**
         * @brief C'tor
         *
         * Creates a new instance of ReportGenerator.
         *
         * @param[in] reportgen the application's main class. This object must be valid
         *            during the whole lifetime of ReportGenerator.
         */
        ReportGenerator(VeteroReportgen *reportgen);

        /**
         * @brief Virtual D'tor
         */
        virtual ~ReportGenerator();

    protected:
        /**
         * @brief Returns the pointer to the main class of the application.
         *
         * This is just the pointer set in the C'tor.
         *
         * @return the main class
         */
        VeteroReportgen *reportgen() const;

    public:
        /**
         * @brief Does the work.
         *
         * @exception common::ApplicationError if something failed
         */
        virtual void generateReports()
        throw (common::ApplicationError) = 0;

    private:
        VeteroReportgen *m_reportgen;
        common::LockFile m_lockfile;
};

} // end namespace reportgen
} // end namespace vetero

#endif /* REPORTGENERATOR_H_ */
