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

#ifndef VETERO_REPORTGEN_REPORTGENERATOR_H_
#define VETERO_REPORTGEN_REPORTGENERATOR_H_

#include "common/lockfile.h"
#include "vetero_reportgen.h"
#include "nameprovider.h"

namespace vetero {
namespace reportgen {

/**
 * \brief Base class for all report generator classes
 *
 * All report generators should inherit from ReportGenerator.
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup report
 */
class ReportGenerator
{
    public:
        /**
         * \brief C'tor
         *
         * Creates a new instance of ReportGenerator.
         *
         * \param[in] reportgen the application's main class. This object must be valid
         *            during the whole lifetime of ReportGenerator.
         */
        ReportGenerator(VeteroReportgen *reportgen);

        /**
         * \brief Virtual D'tor
         */
        virtual ~ReportGenerator();

    protected:
        /**
         * \brief Returns the pointer to the main class of the application.
         *
         * This is just the pointer set in the C'tor.
         *
         * \return the main class
         */
        VeteroReportgen *reportgen() const;

        /**
         * \brief Returns a reference to the name provider object
         *
         * The name provider is used to get the file names of all diagrams and HTML pages.
         *
         * \return the name provider
         */
        const NameProvider &nameProvider() const;

    public:
        /**
         * \brief Does the work.
         *
         * \exception common::ApplicationError if something failed
         */
        virtual void generateReports() = 0;

    private:
        VeteroReportgen *m_reportgen;
        NameProvider m_nameProvider;
        common::LockFile m_lockfile;
};

} // end namespace reportgen
} // end namespace vetero

#endif // VETERO_REPORTGEN_REPORTGENERATOR_H_
