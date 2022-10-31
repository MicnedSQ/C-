//
// Created by MicnedSQ on 14.12.2021.
//

#ifndef SIECI_FACTORY_HPP
#define SIECI_FACTORY_HPP

#include "nodes.hpp"

enum class NodeColor { UNVISITED, VISITED, VERIFIED };

template <class Node>
class NodeCollection
{
public:
    using container_t = typename std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    const_iterator cbegin() const { return collection_.cbegin(); }
    const_iterator cend() const { return  collection_.cend(); }

    const_iterator begin() const { return collection_.begin(); }
    const_iterator end() const { return collection_.end(); }

    iterator begin() { return collection_.begin(); }
    iterator end() { return collection_.end(); }

    void add(Node&& node) { collection_.template emplace_back(std::move(node)); }

    void remove_by_id(ElementID id) { if (find_by_id(id) != collection_.end() ) { collection_.erase(find_by_id(id)); } }

    iterator find_by_id(ElementID id) { return std::find_if(begin(), end(), [id] (Node& elem) { return (id == elem.get_id()); }); }

    const_iterator find_by_id(ElementID id) const { return std::find_if(cbegin(), cend(), [id] (Node& elem) { return (elem.get_id() == id); }); }

    container_t get_container() { return collection_; }

private:
    container_t collection_;
};

class Factory
{
public:
    void add_ramp(Ramp&& ramp) { NodeCollection<Ramp>().add(std::move(ramp)); }

    void remove_ramp(ElementID id) { NodeCollection<Ramp>().remove_by_id(id); }

    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id) { return NodeCollection<Ramp>().find_by_id(id); }

    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const { return NodeCollection<Ramp>().find_by_id(id); }

    NodeCollection<Ramp>::const_iterator ramp_cbegin() const { return NodeCollection<Ramp>().cbegin(); }

    NodeCollection<Ramp>::const_iterator ramp_cend() const { return NodeCollection<Ramp>().cend(); }

    void add_worker(Worker&& worker) { NodeCollection<Worker>().add(std::move(worker)); }

    void remove_worker(ElementID id) { NodeCollection<Worker>().remove_by_id(id); }

    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id) { return NodeCollection<Worker>().find_by_id(id); }

    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const { return NodeCollection<Worker>().find_by_id(id); }

    NodeCollection<Worker>::const_iterator worker_cbegin() const { return NodeCollection<Worker>().cbegin(); }

    NodeCollection<Worker>::const_iterator worker_cend() const { return NodeCollection<Worker>().cend(); }

    void add_storehouse(Storehouse&& storehouse) { NodeCollection<Storehouse>().add(std::move(storehouse)); }

    void remove_storehouse(ElementID id) { NodeCollection<Storehouse>().remove_by_id(id); }

    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id) { return NodeCollection<Storehouse>().find_by_id(id); }

    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const { return NodeCollection<Storehouse>().find_by_id(id); }

    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const { return NodeCollection<Storehouse>().cbegin(); }

    NodeCollection<Storehouse>::const_iterator storehouse_cend() const { return NodeCollection<Storehouse>().cend(); }

    bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors);

    bool is_consistent();

    void do_deliveries(Time t);

    void do_package_passing();

    void do_work(Time t);

private:
    template<class Node>
    void remove_receiver(NodeCollection<Node>& collection, ElementID id)
    {
        for (auto& it : NodeCollection<Ramp>())
        {
            it.receiver_preferences_.remove_receiver(collection.find_by_id(id));
        }

        for (auto& it : NodeCollection<Worker>())
        {
            it.receiver_preferences_.remove_receiver(collection.find_by_id(id));
        }

        collection.remove_by_id(id);
    }
};

enum class ElementType { RAMP, WORKER, STOREHOUSE, LINK };

Factory load_factory_structure(std::istream& is);

void save_factory_structure(Factory& factory, std::ostream& os);

struct ParsedLineData
{
    std::map<std::string, std::string> parameters;

    ElementType element_type;
};

ParsedLineData parse_line(std::string line);

#endif //SIECI_FACTORY_HPP
