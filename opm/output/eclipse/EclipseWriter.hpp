/*
  Copyright (c) 2013 Andreas Lauser
  Copyright (c) 2013 Uni Research AS
  Copyright (c) 2014 IRIS AS

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPM_ECLIPSE_WRITER_HPP
#define OPM_ECLIPSE_WRITER_HPP

#include <opm/output/OutputWriter.hpp>
#include <opm/output/eclipse/Summary.hpp>
#include <opm/core/wells.h> // WellType

#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/NNC.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Well.hpp>
#include <opm/parser/eclipse/EclipseState/SummaryConfig/SummaryConfig.hpp>
#include <opm/parser/eclipse/Units/UnitSystem.hpp>

#include <ert/ecl/ecl_util.h>

#include <string>
#include <vector>
#include <array>
#include <memory>


namespace Opm {

class SimulationDataContainer;
class WellState;

/*!
 * \brief A class to write the reservoir state and the well state of a
 *        blackoil simulation to disk using the Eclipse binary format.
 */
class EclipseWriter : public OutputWriter
{
public:
    /*!
     * \brief Sets the common attributes required to write eclipse
     *        binary files using ERT.
     */
    EclipseWriter(Opm::EclipseStateConstPtr eclipseState,
                  int numCells,
                  const int* compressedToCartesianCellIdx);

    /**
     * Write the static eclipse data (grid, PVT curves, etc) to disk.
     *
     * If NNC is given, writes TRANNNC keyword.
     */
    virtual void writeInit( time_t current_time, double start_time, const NNC& nnc = NNC() ) override;

    /*!
     * \brief Write a reservoir state and summary information to disk.
     *
     *
     * The reservoir state can be inspected with visualization tools like
     * ResInsight.
     *
     * The summary information can then be visualized using tools from
     * ERT or ECLIPSE. Note that calling this method is only
     * meaningful after the first time step has been completed.
     *
     * \param[in] timer          The timer providing time step and time information
     * \param[in] reservoirState The thermodynamic state of the reservoir
     * \param[in] wellState      The production/injection data for all wells
     */
    virtual void writeTimeStep( int report_step,
                                time_t current_posix_time,
                                double seconds_elapsed,
                                data::Solution,
                                data::Wells,
                                bool isSubstep);


    static int eclipseWellTypeMask(WellType wellType, WellInjector::TypeEnum injectorType);
    static int eclipseWellStatusMask(WellCommon::StatusEnum wellStatus);
    static ert_ecl_unit_enum convertUnitTypeErtEclUnitEnum(UnitSystem::UnitType unit);

private:
    Opm::EclipseStateConstPtr eclipseState_;
    out::Summary summary_;
    int numCells_;
    std::array<int, 3> cartesianSize_;
    const int* compressedToCartesianCellIdx_;
    std::vector< int > gridToEclipseIdx_;
    const double* conversion_table_;
    bool enableOutput_;
    std::string outputDir_;
    std::string baseName_;
    int ert_phase_mask_;

    void init(Opm::EclipseStateConstPtr eclipseState);
};

typedef std::shared_ptr<EclipseWriter> EclipseWriterPtr;
typedef std::shared_ptr<const EclipseWriter> EclipseWriterConstPtr;

} // namespace Opm


#endif // OPM_ECLIPSE_WRITER_HPP
