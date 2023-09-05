#define CL_HPP_TARGET_OPENCL_VERSION 300
#include "CL/opencl.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <vector>

cl::Device get_default_device() {
    /*
            Search for all the OpenCL platforms available and check if there are any.
        */
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    if (platforms.size() == 0) {
        std::cerr << " No OpenCL platforms found.\n";
        throw std::runtime_error("No OpenCL platforms found.");
    }

    /*
        Select the first platform and create a context with all the devices of that platform.
    */
    cl::Platform default_platform = platforms.front();
    std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

    std::vector<cl::Device> devices;
    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

    if (devices.size() == 0) {
        std::cerr << "No divices found.\n";
        throw std::runtime_error("No devices found.");
    }

    cl::Device default_device = devices.front();
    std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";

    return default_device;
}

int main() {

    // Select device
    auto device = get_default_device();

    // read OpenCL kernel file as a string
    std::filesystem::path current_path{ std::filesystem::current_path() };
    auto cl_dir_path = (current_path / "cl_source");
    auto cl_kernel_path = (cl_dir_path / "hello_world.cl").c_str();
    std::ifstream cl_kernel_file(cl_kernel_path);
    std::cout << "Reading kernel file: " << cl_kernel_path << std::endl;
    std::string cl_kernel_src(std::istreambuf_iterator<char>(cl_kernel_file), (std::istreambuf_iterator<char>()));

    // create a source for the program
    cl::Program::Sources sources;
    sources.push_back({ cl_kernel_src.c_str(), cl_kernel_src.length() + 1 });

    // create a context with the device
    cl::Context context(device);

    // create a program from the source code
    cl::Program program(context, sources);

    // build the program
    auto err = program.build();
    if (err != CL_SUCCESS) {
        std::cerr << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(device)
            << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
        return EXIT_FAILURE;
    }


    // create buffers on the device
    char buffer[16];
    cl::Buffer mem_buffer(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(buffer));
    cl::Kernel ocl_kernel(program, "hello_world", &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error creating kernel: " << err << std::endl;
        return EXIT_FAILURE;
    }

    // set kernel arguments
    ocl_kernel.setArg(0, mem_buffer);


    // create a queue to which we will push commands for the device.
    cl::CommandQueue queue(context, device);
    // enqueue kernel
    queue.enqueueNDRangeKernel(ocl_kernel, cl::NullRange, cl::NDRange(16), cl::NullRange);

    // read and print the result
    queue.enqueueReadBuffer(mem_buffer, CL_TRUE, 0, sizeof(buffer), buffer);
    std::cout << buffer << std::endl;

    return EXIT_SUCCESS;
}