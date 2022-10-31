#include <iostream>
#include <set>
#include <algorithm>

#include "storage_types.hpp"
#include "types.hpp"
#include "config.hpp"

int main()
{
    PackageQueue q(PackageQueueType::FIFO);
    q.push(Package(1));
    q.push(Package(2));

    Package p(std::move(q.pop()));
    std::cout << p.get_id() << std::endl;

    p = q.pop();
    std::cout << p.get_id() << std::endl;

    return EXIT_SUCCESS;
}
