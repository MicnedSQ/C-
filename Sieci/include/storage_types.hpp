//
// Created by MicnedSQ on 14.12.2021.
//

#ifndef SIECI_STORAGE_TYPES_HPP
#define SIECI_STORAGE_TYPES_HPP

#include <list>

#include "package.hpp"

enum PackageQueueType
{
    FIFO,
    LIFO
};

class IPackageStockpile
{
public:
    IPackageStockpile() = default;

    using const_iterator = std::list<Package>::const_iterator;

    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;

    virtual const_iterator begin() = 0;
    virtual const_iterator end() = 0;

    virtual void push(Package&& package) = 0;

    virtual std::size_t size() = 0;

    virtual bool empty() = 0;

    virtual ~IPackageStockpile() = default;
};

class IPackageQueue : public IPackageStockpile
{
public:
    virtual Package pop() = 0;

    virtual PackageQueueType get_queue_type() = 0;

    virtual ~IPackageQueue() = default;
};

class PackageQueue : public IPackageQueue
{
public:
    explicit PackageQueue(PackageQueueType type) : queue_type_(type) {};

    PackageQueueType get_queue_type() override { return queue_type_; };

    const_iterator cbegin() const override { return list_of_products_.cbegin(); }
    const_iterator cend() const override { return list_of_products_.cend(); }

    const_iterator begin() override { return list_of_products_.begin(); }
    const_iterator end() override { return list_of_products_.end(); }

    void push(Package&& package) override { list_of_products_.emplace_back(std::move(package)); };

    std::size_t size() override { return list_of_products_.size(); };

    bool empty() override { return list_of_products_.empty(); };

    Package pop() override;

private:
    std::list<Package> list_of_products_;
    PackageQueueType queue_type_;
};

#endif //SIECI_STORAGE_TYPES_HPP
