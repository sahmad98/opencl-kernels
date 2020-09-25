#include <boost/compute.hpp>
#include <iostream>

namespace compute = ::boost::compute;
int main() {
    const auto pfs = compute::system::platforms();
    const auto p_num = compute::system::platform_count();
    std::cout << "Found " << p_num << " platforms\n";
    for (const auto& pf : pfs) {
        std::cout << "\tPlatformName: " << pf.name() << "\n";
        std::cout << "\t\t'-> Vendor : " << pf.vendor() << "\n";
        std::cout << "\t\t'-> ID     : " << pf.id() << "\n";
        std::cout << "\t\t'-> Profile: " << pf.profile() << "\n";
        std::cout << "\t\t'-> Version: " << pf.version() << "\n";
    }

    compute::device default_dev = compute::system::default_device();
    std::cout << "Default Device: " << default_dev.name() 
        << " " << default_dev.vendor() << "\n";


    return 0;
}
