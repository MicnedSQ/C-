//
// Created by MicnedSQ on 14.12.2021.
//
#include "storage_types.hpp"

Package PackageQueue::pop()
{
    switch (queue_type_)
    {
        case PackageQueueType::FIFO:
        {
            Package first = std::move(list_of_products_.front());
            list_of_products_.pop_front();
            return first;
        }

        case PackageQueueType::LIFO:
        {
            Package last = std::move(list_of_products_.back());
            list_of_products_.pop_back();
            return last;
        }
        default:
            return std::move(list_of_products_.back());
    }
}
