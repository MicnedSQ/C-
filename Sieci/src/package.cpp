//
// Created by MicnedSQ on 14.12.2021.
//
#include <algorithm>

#include "package.hpp"

Package::Package()
{
    if (!freed_IDs.empty())
    {
        id_ = *freed_IDs.end();
        assigned_IDs.insert(id_);
    }
    else if (!assigned_IDs.empty())
    {
        id_ = *std::max_element(assigned_IDs.begin(), assigned_IDs.end()) + 1;
        assigned_IDs.insert(id_);
    }
    else
    {
        id_ = 1;
        assigned_IDs.insert(id_);
    }
}

Package::Package(ElementID id)
{
    // sprawdzanie czy id jest w assigned_IDs
    if (assigned_IDs.find(id) != assigned_IDs.end())
    {
        id_ = id;
        assigned_IDs.insert(id_);
    }
    else
    {
        throw std::invalid_argument("Id is in the assigned_IDs, try another one");
    }
}

Package &Package::operator=(Package &&other)
{
    assigned_IDs.erase(id_);
    freed_IDs.insert(id_);
    id_ = other.id_;
    return *this;
}

Package::~Package()
{
    assigned_IDs.erase(id_);
    freed_IDs.insert(id_);
}
