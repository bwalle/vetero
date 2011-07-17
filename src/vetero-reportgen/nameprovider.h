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
#ifndef VETERO_REPORTGEN_NAMEPROVIDER_H_
#define VETERO_REPORTGEN_NAMEPROVIDER_H_

#include <string>

#include <libbw/datetime.h>

#include "vetero_reportgen.h"


namespace vetero {
namespace reportgen {

/* NameProvider {{{ */

/**
 * \brief Provides the names of plots and index files
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup report
 */
class NameProvider
{
    public:
        /// Extension for HTML files including the dot (<tt>".xhtml"</tt>)
        static const std::string HTML_EXTENSION;

        /// Extension for compressed SVG files including the dot (<tt>".svgz"</tt>)
        static const std::string SVG_EXTENSION;

        /// The directory index page
        static const std::string INDEX_HTML;

    public:
        /**
         * \brief Creates a new NameProvider
         *
         * The VeteroReportgen object is needed to get access to the configuration
         * object which is needed to get the main report directory.
         *
         * \param[in] reportgen the reportgen object
         */
        NameProvider(const VeteroReportgen &reportgen);

        /**
         * \brief D'tor
         */
        virtual ~NameProvider();

    public:
        /**
         * \brief Returns the directory where the daily diagrams and index page(s) are
         *
         * \param[in] date the date for which the report has to be generated (the time
         *            in the bw::Datetime object doesn't matter)
         * \return a full path to a directory (it's not checked if the directories exist)
         */
        std::string dailyDir(const bw::Datetime &date) const;

        /**
         * \brief Returns the link to the daily directory
         *
         * \param[in] date the date for which the report has to be generated (the time
         *            in the bw::Datetime object doesn't matter)
         * \return a link relative to the document root, including a trailing <tt>"/"</tt>
         */
        std::string dailyDirLink(const bw::Datetime &date) const;

        /**
         * \brief Returns the full path name for the index page for the daily report
         *
         * \param[in] date the date for which the report has to be generated (the time
         *            in the bw::Datetime object doesn't matter)
         * \return a full path (it's not checked if the directories exist)
         */
        std::string dailyIndex(const bw::Datetime &date) const;

        /**
         * \brief Returns the full path name for the index page for the monthly report
         *
         * \param[in] date the date for which the report has to be generated (the time
         *            in the bw::Datetime object doesn't matter)
         * \param[in] type the type of the diagram, e.g. <tt>"wind"</tt>
         * \return a full path (it's not checked if the directories exist)
         */
        std::string dailyDiagram(const bw::Datetime &date, const std::string &type) const;

        /**
         * \brief Returns the link to the dail diagram
         *
         * \param[in] date the date for which the report has to be generated (the time
         *            in the bw::Datetime object doesn't matter)
         * \param[in] type the type of the diagram, e.g. <tt>"wind"</tt>
         * \return a link relative to the document root, including a trailing <tt>"/"</tt>
         */
        std::string dailyDiagramLink(const bw::Datetime &date, const std::string &type) const;

        /**
         * \brief Returns the directory where the monthly diagrams and index page(s) are
         *
         * \param[in] date the date for which the report has to be generated (the time
         *            in the bw::Datetime object doesn't matter)
         * \return a full path to a directory (it's not checked if the directories exist)
         */
        std::string monthlyDir(const bw::Datetime &date) const;

        /**
         * \brief Returns the link to the monthly directory
         *
         * \param[in] date the date for which the report has to be generated (the time
         *            in the bw::Datetime object doesn't matter)
         * \return a link relative to the document root, including a trailing <tt>"/"</tt>
         */
        std::string monthlyDirLink(const bw::Datetime &date) const;

        /**
         * \brief Returns the full path name for the index page for the monthly report
         *
         * \param[in] date the date for which the report has to be generated (the time
         *            in the bw::Datetime object doesn't matter)
         * \return a full path (it's not checked if the directories exist)
         */
        std::string monthlyIndex(const bw::Datetime &date) const;

        /**
         * \brief Returns the full path name for the index page for the monthly report
         *
         * \param[in] date the date for which the report has to be generated (the time
         *            in the bw::Datetime object doesn't matter)
         * \param[in] type the type of the diagram, e.g. <tt>"wind"</tt>
         * \return a full path (it's not checked if the directories exist)
         */
        std::string monthlyDiagram(const bw::Datetime &date, const std::string &type) const;

        /**
         * \brief Returns the link to the monthly diagram
         *
         * \param[in] date the date for which the report has to be generated (the time
         *            in the bw::Datetime object doesn't matter)
         * \param[in] type the type of the diagram, e.g. <tt>"wind"</tt>
         * \return a link relative to the document root, including a trailing <tt>"/"</tt>
         */
        std::string monthlyDiagramLink(const bw::Datetime &date, const std::string &type) const;

        /**
         * \brief Returns the directory in which the index page resides
         *
         * \return a full path (it's not checked if the directories exist)
         */
        std::string indexDir() const;

        /**
         * \brief Returns the name of the index page
         *
         * \return a full path (it's not checked if the directories exist)
         */
        std::string indexPage() const;

    private:
        const VeteroReportgen &m_reportgen;
};

/* }}} */

} // end namespace daemon
} // end namespace reportgen

#endif // VETERO_REPORTGEN_NAMEPROVIDER_H_
