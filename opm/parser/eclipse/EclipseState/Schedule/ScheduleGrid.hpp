/*
  Copyright 2021 Equinor ASA.

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
#ifndef SCHEDULE_GRID
#define SCHEDULE_GRID

#include <opm/parser/eclipse/EclipseState/Schedule/CompletedCells.hpp>

namespace Opm {

class EclipseGrid;

class ScheduleGrid {
public:
    ScheduleGrid(const EclipseGrid& ecl_grid, CompletedCells& completed_cells);
    explicit ScheduleGrid(CompletedCells& completed_cells);

    std::size_t getActiveIndex(std::size_t i, std::size_t j, std::size_t k) const;
    bool isCellActive(std::size_t i, std::size_t j, std::size_t k) const;
    std::size_t getGlobalIndex(std::size_t i, std::size_t j, std::size_t k) const;
    double getCellDepth(std::size_t i, std::size_t j, std::size_t k) const;
    std::array<double, 3> getCellDimensions(std::size_t i, std::size_t j, std::size_t k) const;

private:

    const CompletedCells::Cell& get_cell(std::size_t i, std::size_t j, std::size_t k) const;

    const EclipseGrid* grid{nullptr};
    CompletedCells& cells;
};



}
#endif

