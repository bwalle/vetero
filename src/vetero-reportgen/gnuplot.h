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

#ifndef GNUPLOT_H_
#define GNUPLOT_H_

#include <sstream>
#include <string>
#include <vector>

#include "common/error.h"
#include "common/configuration.h"

/**
 * @file gnuplot.h
 * @brief Contains the Gnuplot diagram generator class
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup report
 */

namespace vetero {
namespace reportgen {

/**
 * @brief Generating diagrams with Gnuplot
 *
 * This class helps to create diagrams using Gnuplot. It doesn't abstract from Gnuplot, doesn't
 * create a API between C++ and Gnuplot, just provides an interface to plot simple diagrams
 * with Gnuplot.
 *
 * You have to set all Gnuplot commands using the operator<< syntax:
 *
 * @code
 * Gnuplot gp;
 * gp << "set bla 'fasel'\n";
 * @endcode
 *
 * This includes also the output format and output file. The data itself are passed as two-dimensional
 * vector (the one you get from the Database) to the plot() function. Since the file name must be used
 * in the commands, the placeholder Gnuplot::PLACEHOLDER must be used for this.
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup report
 */
class Gnuplot
{
    public:
        /**
         * @brief This is the placeholder used in Gnuplot commands which stands for the data file.
         */
        static std::string PLACEHOLDER;

        /**
         * @brief Convenience typedef
         */
        typedef std::vector<std::string> StringVector;

        /**
         * @brief Convenience typedef
         */
        typedef std::vector<StringVector> StringStringVector;

    public:
        /**
         * @brief C'tor
         *
         * @param[in] config the application's configuration
         */
        Gnuplot(const common::Configuration &config);

        /**
         * @brief Virtual D'tor
         */
        virtual ~Gnuplot();

    public:
        /**
         * @brief Returns the working directory set with setWorkingDirectory()
         *
         * @return the working directory
         */
        std::string workingDirectory() const;

        /**
         * @brief Sets the working directory for the Gnuplot call in plot()
         *
         * @note This is not thread safe as the working directory of the current process
         *       is modified and restored in plot().
         *
         * @param[in] workingdir the working directory. An empty string means the working
         *            directory remains unchanged.
         */
        void setWorkingDirectory(const std::string &workingdir);

        /**
         * @brief Returns the output file
         *
         * @return the name of the output file set with setOutputFile()
         */
        std::string outputFile() const;

        /**
         * @brief Sets the output file
         *
         * This function must be called before the <tt>plot</tt> command is written
         * to the Gnuplot stream.
         *
         * The output file must be SVG and will be automatically compressed in the
         * plot() function.
         *
         * @param[in] output the name of the output file
         */
        void setOutputFile(const std::string &output);

        /**
         * @brief Prints something to Gnuplot
         *
         * See the description of the class how to use that operator properly.
         *
         * @param[in] t the data element which should be appended to the Gnuplot command stream
         * @return a self reference
         */
        template<typename T>
        Gnuplot &operator<<(const T &t);

        /**
         * @brief Plots a diagram to @p filename with @p data
         *
         * The only Gnuplot command which is appended in this command is the output command which
         * is redirected to @p filename. The placeholder Gnuplot::PLACEHOLDER is replaced with the
         * name of the temporary file in which @p data is stored. You have to use Gnuplot::PLACEHOLDER
         * exactly once in the commands.
         *
         * @param[in] data the data which should be plot
         * @exception common::ApplicationError on error
         */
        void plot(const StringStringVector &data)
        throw (common::ApplicationError);

    protected:
        /**
         * @brief Stores the data @p data at @p fd
         *
         * The function just formats the data separated by tabs and newlines. The temporary file
         * is deleted automatically.
         *
         * @param[in] data a two-dimensional string array
         * @param[in] fd the file descriptor where the data is written to
         * @exception common::ApplicationError is writing failed
         */
        void storeData(int fd, const StringStringVector &data)
        throw (common::ApplicationError);

        /**
         * \brief Dumps the error information from the given file descriptor to the logging system
         *
         * \param[in] fd the file descriptor
         */
        void dumpError(int fd);

    private:
        const common::Configuration &m_config;
        std::stringstream m_stream;
        std::string m_workingDirectory;
        std::string m_outputFile;
};

// -------------------------------------------------------------------------------------------------
template<typename T>
Gnuplot &Gnuplot::operator<<(const T &t)
{
    m_stream << t;
    return *this;
}

/**
 * @brief Adds some weather-specific functions to the Gnuplot class
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 */
class WeatherGnuplot : public Gnuplot
{
    public:
        /**
         * @brief C'tor
         *
         * @param[in] config the application's configuration
         */
        WeatherGnuplot(const common::Configuration &config);

    public:
        /**
         * @brief Adds the y axis descriptions for a wind diagram
         */
        void addWindY();
};

} // namespace reportgen
} // namespace vetero


#endif /* GNUPLOT_H_ */
