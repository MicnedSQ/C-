//
// Created by MicnedSQ on 14.12.2021.
//

#ifndef SIECI_HELPERS_HPP
#define SIECI_HELPERS_HPP

#include <functional>
#include <random>

#include "types.hpp"

extern std::random_device rd;
extern std::mt19937 rng;

extern double default_probability_generator();

extern ProbabilityGenerator probability_generator;

#endif //SIECI_HELPERS_HPP
