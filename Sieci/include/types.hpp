//
// Created by MicnedSQ on 14.12.2021.
//

#ifndef SIECI_TYPES_HPP
#define SIECI_TYPES_HPP

#include <functional>
#include <iostream>
#include <random>

using ElementID = std::size_t;

using Time = std::size_t;

using TimeOffset = std::size_t;

using ProbabilityGenerator = std::function<double()>;

#endif //SIECI_TYPES_HPP
