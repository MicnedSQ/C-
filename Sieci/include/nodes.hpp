//
// Created by MicnedSQ on 14.12.2021.
//

#ifndef SIECI_NODES_HPP
#define SIECI_NODES_HPP

#include "helpers.hpp"
#include "storage_types.hpp"
#include "config.hpp"

#include <map>
#include <memory>
#include <utility>

enum class ReceiverType
{
    WORKER,
    STOREHOUSE
};

class IPackageReceiver
{
public:
    virtual IPackageStockpile::const_iterator cbegin() const = 0;
    virtual IPackageStockpile::const_iterator cend() const = 0;

    virtual IPackageStockpile::const_iterator begin() const = 0;
    virtual IPackageStockpile::const_iterator end() const = 0;

    virtual void receive_package(Package&& package) = 0;

    virtual ElementID get_id() const = 0;

    virtual ReceiverType get_receiver_type() const = 0;

    virtual ~IPackageReceiver() = default;
};

class Storehouse : public IPackageReceiver
{
public:
    Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(LIFO)) : id_(id), d_(std::move(d)) {};

    IPackageStockpile::const_iterator cbegin() const override { return d_->cbegin(); }
    IPackageStockpile::const_iterator cend() const override { return d_->cend(); }

    IPackageStockpile::const_iterator begin() const override { return d_->begin(); }
    IPackageStockpile::const_iterator end() const override { return d_->end(); }

    void receive_package(Package&& package) override { d_->push(std::move(package)); } // nie wiadomo czy dobrze

    ElementID get_id() const override { return id_; };

    ReceiverType get_receiver_type() const override { return receiver_type_; }

private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> d_;
    ReceiverType receiver_type_ = ReceiverType::STOREHOUSE;
};

class ReceiverPreferences
{
public:
    explicit ReceiverPreferences(ProbabilityGenerator pg = probability_generator) : pg_(std::move(pg)) {}

    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    const_iterator cbegin() const { return preferences_t_.cbegin(); }
    const_iterator cend() const { return  preferences_t_.cend(); }

    const_iterator begin() const { return preferences_t_.begin(); }
    const_iterator end() const { return preferences_t_.end(); }

    void add_receiver(IPackageReceiver* r); // do sprawdzenia

    void remove_receiver(IPackageReceiver* r); // do sprawdzenia

    IPackageReceiver* choose_receiver(); // do sprawdzenia

    const preferences_t& get_preferences() const { return preferences_t_; }

protected:
    preferences_t preferences_t_;
    ProbabilityGenerator pg_;
};

class PackageSender : public ReceiverPreferences
{
public:
    PackageSender() = default;

    PackageSender(PackageSender&&) = default;

    void send_package() { choose_receiver()->receive_package(std::move(*buffer_)); }; // do sprawdzenia

    const std::optional<Package>& get_sending_buffer() const { return buffer_; };

    void push_package(Package&&) {};

    ReceiverPreferences receiver_preferences_;

protected:
    std::optional<Package> buffer_;
};

class Ramp : public PackageSender
{
public:
    Ramp(ElementID id, TimeOffset di) : id_(id), di_(di) {};

    Ramp(const Ramp&) : PackageSender() {}

    void deliver_goods(Time t);

    TimeOffset get_delivery_interval() const { return di_; };

    ElementID get_id() const { return id_; };
private:
    ElementID id_;
    TimeOffset di_;
    Time start_creating_time_;
};

class Worker : public PackageSender, public IPackageReceiver
{
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q) : id_(id), pd_(pd), q_(std::move(q)) {};

    Worker(const Worker&) : PackageSender() {}

    IPackageStockpile::const_iterator cbegin() const override { return q_->cbegin(); }
    IPackageStockpile::const_iterator cend() const override { return q_->cend(); }

    IPackageStockpile::const_iterator begin() const override { return q_->begin(); }
    IPackageStockpile::const_iterator end() const override { return q_->end(); }

    ElementID get_id() const override { return id_; };

    void receive_package(Package&& package) override { q_->push(std::move(package)); } // nie wiadomo czy dobrze

    void do_work(Time t);

    TimeOffset get_processing_duration() const { return pd_; };

    Time get_package_processing_start_time() { return start_processing_time_; }; // raczej zle

    ReceiverType get_receiver_type() const override { return receiver_type_; }

    IPackageQueue* get_queue() const { return q_.get(); }

    const std::optional<Package>& get_processing_buffer() const {return pb_;}

private:
    ElementID id_;
    TimeOffset pd_;
    std::unique_ptr<IPackageQueue> q_;
    Time start_processing_time_ = 0;
    ReceiverType receiver_type_ = ReceiverType::WORKER;
    std::optional<Package> pb_;
};

#endif //SIECI_NODES_HPP
