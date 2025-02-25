#include <vector>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseConfig.hpp>
#include <opm/parser/eclipse/EclipseState/InitConfig/InitConfig.hpp>
#include <opm/parser/eclipse/EclipseState/SummaryConfig/SummaryConfig.hpp>
#include <opm/parser/eclipse/EclipseState/SimulationConfig/SimulationConfig.hpp>
#include <opm/parser/eclipse/EclipseState/Tables/TableManager.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Schedule.hpp>


#include "export.hpp"


void python::common::export_EclipseConfig(py::module& module)
{
    py::class_< EclipseConfig >( module, "EclipseConfig" )
        .def( "init",            py::overload_cast<>(&EclipseConfig::init, py::const_));

    // Note: In the below class we std::shared_ptr as the holder type, see:
    //
    //  https://pybind11.readthedocs.io/en/stable/advanced/smart_ptrs.html
    //
    // this makes it possible to share the returned object with e.g. and
    //   opm.simulators.BlackOilSimulator Python object
    //
    py::class_< SummaryConfig, std::shared_ptr<SummaryConfig> >( module, "SummaryConfig")
        .def(py::init([](const Deck& deck, const EclipseState& state, const Schedule& schedule) {
            return SummaryConfig( deck, schedule, state.fieldProps(), state.aquifer() );
         }  )  )

        .def( "__contains__",    &SummaryConfig::hasKeyword );

    py::class_< InitConfig >( module, "InitConfig")
        .def( "hasEquil",           &InitConfig::hasEquil )
        .def( "restartRequested",   &InitConfig::restartRequested )
        .def( "getRestartStep"  ,   &InitConfig::getRestartStep );

    py::class_< IOConfig >( module, "IOConfig");

    py::class_< SimulationConfig >( module, "SimulationConfig")
        .def("hasThresholdPressure", &SimulationConfig::useThresholdPressure )
        .def("useCPR",               &SimulationConfig::useCPR )
        .def("hasDISGAS",            &SimulationConfig::hasDISGAS )
        .def("hasVAPOIL",            &SimulationConfig::hasVAPOIL )
        .def("hasVAPWAT",            &SimulationConfig::hasVAPWAT );
}

