//
// Created by MicnedSQ on 14.12.2021.
//

#ifndef SIECI_SIMULATION_HPP
#define SIECI_SIMULATION_HPP

#include <utility>

#include "reports.hpp"

void simulate(Factory &factory, TimeOffset d, const std::function<void(Factory&, Time)>&);

#endif //SIECI_SIMULATION_HPP
