#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define CL_HPP_TARGET_OPENCL_VERSION 200
#include "CL/opencl.hpp"
#include "utils.hpp"
#include <iostream>
#include <vector>

// kernel calculates for each element C=A+B
std::string kernel_code =
"   void kernel simple_add(global const int* A, global const int* B, global int* C){ "
"       C[get_global_id(0)]=A[get_global_id(0)]+B[get_global_id(0)];                 "
"   }                                                                               ";

int main() {

    // host data
    uint size = 100000000;
    int* h_a = new int[size];
    int* h_b = new int[size];
    int* h_c = new int[size];
    for (uint i = 0; i < size; i++) {
        h_a[i] = rand() % size;
        h_b[i] = rand() % size;
    }

    //If there are no opencl platforms -  all_platforms == 0 and the program exits. 

    //One of the key features of OpenCL is its portability. So, for instance, there might be situations
    // in which both the CPU and the GPU can run OpenCL code. Thus, 
    // a good practice is to verify the OpenCL platforms to choose on which the compiled code run.

    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);
    if (all_platforms.size() == 0) {
        std::cerr << " No OpenCL platforms found.\n";
        return EXIT_FAILURE;
    }

    //We are going to use the platform of id == 0
    cl::Platform default_platform = all_platforms[0];
    std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

    //An OpenCL platform might have several devices. 
    //The next step is to ensure that the code will run on the first device of the platform, 
    //if found.
    std::vector<cl::Device> all_devices;
    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if (all_devices.size() == 0) {
        std::cerr << "No divices found.\n";
        return EXIT_FAILURE;
    }

    cl::Device default_device = all_devices[0];
    std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";

    // A context manages all the OpenCL objects, memory, command queue, kernels, etc.
    cl::Context context({ default_device });



    // device buffers
    cl::Buffer d_a(context, CL_MEM_READ_WRITE, sizeof(int) * size);
    cl::Buffer d_b(context, CL_MEM_READ_WRITE, sizeof(int) * size);
    cl::Buffer d_c(context, CL_MEM_READ_WRITE, sizeof(int) * size);

    // create a queue to which we will push commands for the device.
    cl::CommandQueue queue(context, default_device);

    // push write commands to queue
    queue.enqueueWriteBuffer(d_a, CL_TRUE, 0, sizeof(int) * size, h_a);
    queue.enqueueWriteBuffer(d_b, CL_TRUE, 0, sizeof(int) * size, h_b);


    // source code for the kernel
    cl::Program::Sources sources;
    sources.push_back({ kernel_code.c_str(), kernel_code.length() });

    // create a program from the source code
    cl::Program program(context, sources);

    if (program.build({ default_device }) != CL_SUCCESS) {
        std::cerr << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
        return EXIT_FAILURE;
    }


    Utils::Timer timer;
    // create the kernel
    cl::Kernel kernel(program, "simple_add");
    cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer> simple_add(kernel);

    // execute the kernel
    cl::NDRange global(size);

    simple_add(cl::EnqueueArgs(queue, global), d_a, d_b, d_c).wait();

    timer.execution_time();


    queue.enqueueReadBuffer(d_c, CL_TRUE, 0, sizeof(int) * size, h_c);

    timer.execution_time();

    // std::cout << "Result: \n";
    // for (int i = 0; i < size; i++) {
    //     std::cout << h_c[i] << " ";
    // }

    timer.reset();

    for (uint i = 0; i < size; i++) {
        h_c[i] = h_a[i] + h_b[i];
    }
    timer.execution_time();

    delete [] h_a;
    delete [] h_b;
    delete [] h_c;

    return 0;
}