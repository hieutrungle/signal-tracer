#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include "Cl/cl.hpp"

int main() {
    cl::Platform default_platform = all_platforms[0];
    std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";
}