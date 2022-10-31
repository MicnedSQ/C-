//
// Created by MicnedSQ on 14.12.2021.
//
#include "nodes.hpp"

void Ramp::deliver_goods(Time t)
{
    bool work;

    if (t < start_creating_time_ + di_)
    {
        work = true;
    }
    else
    {
        work = false;
    }

    if (!work)
    {
        Package p;
        buffer_.emplace(std::move(p)); // do sprawdzenia
        start_creating_time_ = t;
    }
}

void Worker::do_work(Time t)
{
    bool work;

    if (t < start_processing_time_ + pd_)
    {
        work = true;
    }
    else
    {
        work = false;
    }

    if (!work)
    {
        buffer_.emplace(q_->pop()); // do sprawdzenia
        start_processing_time_ = t;
    }
}

void ReceiverPreferences::add_receiver(IPackageReceiver* r)
{
    if (preferences_t_.empty())
    {
        preferences_t_.insert(std::make_pair(r, 1.0f));
    }
    else
    {
        for (auto& receiver : preferences_t_)
        {
            receiver.second = 1.0f / float(preferences_t_.size() + 1);
        }
        preferences_t_.insert(std::make_pair(r, 1.0f / float(preferences_t_.size() + 1)));
    }

}

void ReceiverPreferences::remove_receiver(IPackageReceiver* r)
{
    preferences_t_.erase(r);
    for (auto& receiver : preferences_t_)
    {
        receiver.second = 1.0f / (float(preferences_t_.size()) - 1);
    }
}

IPackageReceiver* ReceiverPreferences::choose_receiver()
{
    if (preferences_t_.size() == 1)
    {
        return preferences_t_.begin()->first;
    }
    else
    {
        double sum = 0;
        for (auto& receiver : preferences_t_)
        {
            sum += receiver.second;
            if (pg_() > sum)
            {
                return receiver.first;
            }
        }
        return preferences_t_.end()->first;
    }
}