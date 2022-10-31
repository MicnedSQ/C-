//
// Created by MicnedSQ on 14.12.2021.
//

#ifndef SIECI_PACKAGE_HPP
#define SIECI_PACKAGE_HPP

#include "types.hpp"
#include <set>

class Package
{
public:
    Package();
    Package(ElementID id);
    Package(Package&&) = default;

    Package& operator=(Package &&other);

    ElementID get_id() const { return id_; };

    ~Package();

private:
    inline static std::set<ElementID> assigned_IDs;
    inline static std::set<ElementID> freed_IDs;
    ElementID id_;
};

#endif //SIECI_PACKAGE_HPP
