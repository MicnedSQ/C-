//
// Created by MicnedSQ on 14.12.2021.
//

#include "simulation.hpp"

void simulate(Factory &factory, TimeOffset d, const std::function<void(Factory&, Time)>&) {
    for (std::size_t t = 1; t != d + 1; t++)
    {
        factory.do_deliveries(t);

        factory.do_package_passing();

        factory.do_work(t);
    }
}
