#include <ostream>
#include "reports.hpp"
#include "factory.hpp"

bool SpecificTurnsReportNotifier::should_generate_report(Time t)
{
    if (turns_.find(t) != turns_.end()) {
        return true;
    }
    return false;
}

bool IntervalReportNotifier::should_generate_report(Time t)
{
    if (!(t - 1 % to_)) {
        return true;
    }
    return false;
}

void generate_structure_report(const Factory& f,std::ostream& oss) {
    oss << " == LOADING RAMPS ==\n" << std::endl;
    for(auto it = f.ramp_cbegin(); it != f.ramp_cend(); ++it) {
        oss << "LOADING RAMP #" << it->get_id() << std::endl;
        oss << "    Delivery interval: " << it->get_delivery_interval() << std::endl;
        oss << "    Receivers:" << std::endl;
        for(auto is : it->receiver_preferences_.get_preferences()) {
            if(is.first->get_receiver_type() == ReceiverType::WORKER) {
                oss << "    worker #" << is.first->get_id() << std::endl;
            } else {
                oss << "    storehouse #" << is.first->get_id() << std::endl;
            }
        }
    }
    oss << std::endl;

    oss << "\n == WORKERS ==\n" << std::endl;
    for(auto it = f.worker_cbegin(); it != f.worker_cend(); ++it) {
        oss << "WORKER #" << it->get_id() << std::endl;
        oss << "    Processing time: " << it->get_processing_duration() << std::endl;
        if((it->get_queue()->get_queue_type()) == PackageQueueType::FIFO) {
            oss << "    Queue type: FIFO" << std::endl;
        } else {
            oss << "    Queue type: LIFO" << std::endl;
        }
        oss << "    Receivers:" << std::endl;
        for(auto is : it->receiver_preferences_.get_preferences()) {
            if(is.first->get_receiver_type() == ReceiverType::WORKER) {
                oss << "    worker #" << is.first->get_id() << std::endl;
            } else {
                oss << "    storehouse #" << is.first->get_id() << std::endl;
            }
        }
    }
    oss << std::endl;


    oss << "\n == STOREHOUSES ==\n" << std::endl;
    for(auto it = f.storehouse_cbegin(); it != f.storehouse_cend(); ++it) {
        oss << "STOREHOUSES #" << it->get_id() << std::endl;

    }
    oss << std::endl;

}


void generate_simulation_turn_report(const Factory& f,std::ostream& oss, Time t){
    oss << "=== [ Turn: " << std::to_string(t) << " ] ===\n" << std::endl;
    oss << " == WORKERS ==\n" << std::endl;
    for(auto it = f.worker_cbegin(); it != f.worker_cend(); ++it) {
        oss << "WORKER #" << it->get_id() << std::endl;
        oss << "    PBuffer: ";
        if(!it->get_processing_buffer()) {
            oss << "(empty)" << std::endl;

        } else {
            oss << "#" << it->get_processing_buffer()->get_id() << " (pt = " << it->get_processing_duration() << ")" << std::endl;
        }
        oss << "    Queue:";
        if(it->get_queue()->empty()) {
            oss << " (empty)" << std::endl;
        } else {
            for (auto iq = it->get_queue()->cbegin(); iq != it->get_queue()->cend(); iq++) {
                oss << " #" << iq->get_id() << std::endl;
            }
        oss << "    SBuffer:";
        if(it->get_sending_buffer()) {
            oss << " #" << it->get_sending_buffer()->get_id() << std::endl;
        } else {
            oss << " (empty)" << std::endl;
        }

    }
    oss << std::endl;

    oss << "\n == STOREHOUSES ==\n" << std::endl;
    for(auto is = f.storehouse_cbegin(); is != f.storehouse_cend(); ++is) {
        oss << "STOREHOUSES #" << it->get_id() << std::endl;
        oss << "    Stock: " << it->get_id();
        oss << "#" << it->cbegin()->get_id() << std::endl;
        for(auto iss = it->cbegin(); iss != it->cend(); iss++){
            oss << ", #" << iss->get_id() << std::endl;
        }

    }
    oss << std::endl;

}}
