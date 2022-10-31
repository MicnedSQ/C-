//
// Created by MicnedSQ on 14.12.2021.
//
#include <sstream>

#include "factory.hpp"

bool Factory::has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors)
{
    if (node_colors[sender] == NodeColor::VERIFIED)
    {
        return true;
    }

    node_colors[sender] = NodeColor::VISITED;

    if (sender->receiver_preferences_.get_preferences().empty())
    {
        throw std::logic_error("Sender has no receivers");
    }

    bool sender_has_one_receiver = false;

    for (auto& receiver : sender->receiver_preferences_.get_preferences())
    {
        if (receiver.first->get_receiver_type() == ReceiverType::STOREHOUSE)
        {
            sender_has_one_receiver = true;
        }
        else
        {
            IPackageReceiver* receiver_ptr = receiver.first;
            auto worker_ptr = dynamic_cast<Worker*>(receiver_ptr);
            auto sendrecv_ptr = dynamic_cast<PackageSender*>(worker_ptr);

            if (sendrecv_ptr == sender)
            {
                continue;
            }

            sender_has_one_receiver = true;

            if (node_colors[sendrecv_ptr] == NodeColor::UNVISITED)
            {
                has_reachable_storehouse(sendrecv_ptr, node_colors);
            }
        }
    }

    node_colors[sender] = NodeColor::VERIFIED;

    if (sender_has_one_receiver)
    {
        return true;
    }
    else
    {
        throw std::logic_error("Sender has no receiver");
    }
}

bool Factory::is_consistent()
{
    std::map<const PackageSender*, NodeColor> color;

    for (auto& it : NodeCollection<Ramp>().get_container())
    {
        color.insert(std::make_pair(&it, NodeColor::UNVISITED));
    }

    for (auto& it : NodeCollection<Worker>().get_container())
    {
        color.insert(std::make_pair(&it, NodeColor::UNVISITED));
    }

    try
    {
        for (auto& it : NodeCollection<Ramp>().get_container())
        {
            has_reachable_storehouse(&it, color);
        }
    }
    catch (std::logic_error const&)
    {
        return false;
    }
    return true;
}

void Factory::do_deliveries(Time t)
{
    for (auto& it : NodeCollection<Ramp>())
    {
        it.deliver_goods(t);
    }
}

void Factory::do_package_passing()
{
    for (auto& it : NodeCollection<Ramp>())
    {
        it.send_package();
    }

    for (auto& it : NodeCollection<Worker>())
    {
        it.send_package();
    }
}

void Factory::do_work(Time t)
{
    for (auto& it : NodeCollection<Worker>())
    {
        it.do_work(t);
    }
}

ParsedLineData parse_line(std::string line)
{
    std::vector<std::string> tokens;
    std::string token_1;
    std::string token_2;

    ParsedLineData p_line;

    std::istringstream token_stream(line);
    char delimiter = ' ';

    std::getline(token_stream, token_2, delimiter);
    if (token_2 == "LOADING_RAMP")
    {
        p_line.element_type = ElementType::RAMP;
    }
    else if (token_2 == "WORKER")
    {
        p_line.element_type = ElementType::WORKER;
    }
    else if (token_2 == "STOREHOUSE")
    {
        p_line.element_type = ElementType::STOREHOUSE;
    }
    else if (token_2 == "LINK")
    {
        p_line.element_type = ElementType::LINK;
    }

    while (std::getline(token_stream, token_1, delimiter))
    {
        tokens.push_back(token_1);
    }

    for (auto& token : tokens)
    {
        p_line.parameters.insert(std::make_pair(token, "="));
    }
    return p_line;
}

Factory load_factory_structure(std::istream& is) // do sprawdzenia
{
    Factory factory;

    std::string line;

    while (std::getline(is, line))
    {
        if (line.empty() || line.rfind(';', 0))
        {
            continue;
        }

        switch (parse_line(line).element_type)
        {
            case ElementType::RAMP:
                factory.add_ramp(Ramp(std::stoi(parse_line(line).parameters["id"]), std::stoi(parse_line(line).parameters["delivery-interval"])));
                break;

            case ElementType::WORKER:
                if (parse_line(line).parameters["queue-type"] == "FIFO")
                {
                    factory.add_worker(Worker(std::stoi(parse_line(line).parameters["id"]), std::stoi(parse_line(line).parameters["precessing-time"]), std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
                }
                else if (parse_line(line).parameters["queue-type"] == "LIFO")
                {
                    factory.add_worker(Worker(std::stoi(parse_line(line).parameters["id"]), std::stoi(parse_line(line).parameters["precessing-time"]), std::make_unique<PackageQueue>(PackageQueueType::LIFO)));
                }
                break;

            case ElementType::STOREHOUSE:
                factory.add_storehouse(Storehouse(std::stoi(parse_line(line).parameters["id"])));
                break;

            case ElementType::LINK:
                std::string src = parse_line(line).parameters["src"];
                if(src.rfind("ramp", 0))
                {
                    std::string dest = parse_line(line).parameters["dest"];
                    if(dest.rfind("store", 0))
                    {
                        factory.find_ramp_by_id(std::stoi(&src.back()))->receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(std::stoi(dest))));
                    }
                    else if(dest.rfind("worker", 0))
                    {
                        factory.find_ramp_by_id(std::stoi(&src.back()))->receiver_preferences_.add_receiver((&(*factory.find_worker_by_id(std::stoi(dest)))));
                    }
                }
                else if (src.rfind("worker", 0))
                {
                    std::string dest = parse_line(line).parameters["dest"];
                    if(dest.rfind("store", 0))
                    {
                        factory.find_worker_by_id(std::stoi(&src.back()))->receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(std::stoi(dest))));
                    }
                    else if(dest.rfind("worker", 0))
                    {
                        factory.find_worker_by_id(std::stoi(&src.back()))->receiver_preferences_.add_receiver((&(*factory.find_worker_by_id(std::stoi(dest)))));
                    }
                }
                break;
        }
    }
    return factory;
}

void save_factory_structure(Factory& factory, std::ostream& os)
{
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); it++)
    {
        os << "LOADING_RAMP id=" << it->get_id() << " delivery-interval=" << it->get_delivery_interval() << std::endl;
    }

    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); it++)
    {
        os << "WORKER id=" << it->get_id() << " processing-time=" << it->get_processing_duration() << " queue-type=";
        if(it->get_queue()->get_queue_type() == PackageQueueType::FIFO)
        {
            os << "FIFO" << std::endl;
        }
        else
        {
            os << "LIFO" << std::endl;
        }
    }

    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); it++)
    {
        os << "STOREHOUSE id=" << it->get_id() << std::endl;
    }

    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); it++)
    {
        for (auto it_2 : it->receiver_preferences_)
        {
            os << "LINK src=ramp-" << it->get_id() << " dest=worker-" << it_2.first->get_id() << std::endl;
        }
    }

    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); it++)
    {
        for (auto it_2 : it->receiver_preferences_)
        {
            os << "LINK src=worker-" << it->get_id();
            if(it_2.first->get_receiver_type() == ReceiverType::STOREHOUSE)
            {
                os << " dest=store-" << it_2.first->get_id() << std::endl;
            }
            if(it_2.first->get_receiver_type() == ReceiverType::WORKER)
            {
                os << " dest=worker-" << it_2.first->get_id() << std::endl;
            }
        }
    }
}