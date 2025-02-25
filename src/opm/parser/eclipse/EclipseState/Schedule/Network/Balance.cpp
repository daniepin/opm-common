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


#include <opm/parser/eclipse/Parser/ParserKeywords/N.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Network/Balance.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Tuning.hpp>
#include <opm/parser/eclipse/Deck/DeckKeyword.hpp>

namespace Opm {
namespace Network {

Balance::Balance(const Tuning& tuning, const DeckKeyword& keyword) {
    using NB = ParserKeywords::NETBALAN;
    const auto& record = keyword[0];
    double interval = record.getItem<NB::TIME_INTERVAL>().getSIDouble(0);

    if (interval < 0) {
        this->calc_mode = CalcMode::NUPCOL;
        this->calc_interval = 0.;
    }
    else if (interval == 0) {
        this->calc_mode = CalcMode::TimeStepStart;
        this->calc_interval = interval;
    }
    else {
        this->calc_mode = CalcMode::TimeInterval;
        this->calc_interval = interval;
    }

    this->ptol = record.getItem<NB::PRESSURE_CONVERGENCE_LIMIT>().getSIDouble(0);
    this->m_pressure_max_iter = record.getItem<NB::MAX_ITER>().get<int>(0);

    this->m_thp_tolerance = record.getItem<NB::THP_CONVERGENCE_LIMIT>().get<double>(0);
    this->m_thp_max_iter = record.getItem<NB::MAX_ITER_THP>().get<int>(0);
    this->target_branch_balance_error = record.getItem<NB::TARGET_BALANCE_ERROR>().getSIDouble(0);
    this->max_branch_balance_error = record.getItem<NB::MAX_BALANCE_ERROR>().getSIDouble(0);

    auto tstep_item = record.getItem<NB::MIN_TIME_STEP>();
    if (tstep_item.defaultApplied(0))
        this->m_min_tstep = tuning.TSMINZ;
    else
        this->m_min_tstep = record.getItem<NB::MIN_TIME_STEP>().getSIDouble(0);
}

Balance::CalcMode Balance::mode() const {
    return this->calc_mode;
}

double Balance::interval() const {
    return this->calc_interval;
}

double Balance::pressure_tolerance() const {
    return this->ptol;
}

double Balance::thp_tolerance() const {
    return this->m_thp_tolerance;
}

std::size_t Balance::thp_max_iter() const {
    return this->m_thp_max_iter;
}

std::size_t Balance::pressure_max_iter() const {
    return this->m_pressure_max_iter;
}

double Balance::target_balance_error() const {
    return this->target_branch_balance_error;
}

double Balance::max_balance_error() const {
    return this->max_branch_balance_error;
}

double Balance::min_tstep() const {
    return this->m_min_tstep;
}

Balance Balance::serializeObject() {
    Balance balance;
    balance.calc_interval = 0.;
    balance.calc_mode = Balance::CalcMode::NUPCOL;
    balance.m_min_tstep = 123;
    balance.ptol = 0.25;
    balance.m_pressure_max_iter = 567;
    return balance;
}
bool Balance::operator==(const Balance& other) const {
    return this->calc_mode == other.calc_mode &&
           this->calc_interval == other.calc_interval &&
           this->ptol == other.ptol &&
           this->m_pressure_max_iter == other.m_pressure_max_iter &&
           this->m_thp_tolerance == other.m_thp_tolerance &&
           this->m_thp_max_iter == other.m_thp_max_iter &&
           this->target_branch_balance_error == other.target_branch_balance_error &&
           this->max_branch_balance_error == other.max_branch_balance_error &&
           this->m_min_tstep == other.m_min_tstep;
}

}
}
