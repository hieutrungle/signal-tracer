#include "cl_utils.hpp"

namespace CLUtils {

    cl::Device get_default_device(const bool& verbose) {
        // Search for all the OpenCL platforms available and check if there are any.
        std::vector<cl::Platform> platforms;
        CHECKCL_ERROR(cl::Platform::get(&platforms));

        if (platforms.size() == 0) {
            std::cerr << " No OpenCL platforms found.\n";
            throw std::runtime_error("No OpenCL platforms found.");
        }

        // Select the first platform and create a context with all the devices of that platform.
        cl::Platform default_platform = platforms.front();
        std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

        std::vector<cl::Device> devices;
        default_platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

        if (devices.size() == 0) {
            std::cerr << "No divices found.\n";
            throw std::runtime_error("No devices found.");
        }
        cl::Device default_device = devices.front();

        if (verbose) {
            std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";
            std::cout << "Using device version: " << default_device.getInfo<CL_DEVICE_VERSION>() << "\n";
            std::cout << "Using device OpenCL C version: " << default_device.getInfo<CL_DEVICE_OPENCL_C_VERSION>() << "\n";
            // std::cout << "Using device extensions: " << default_device.getInfo<CL_DEVICE_EXTENSIONS>() << "\n";
            std::cout << "Using device max compute units: " << default_device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << "\n";
            std::cout << "Using device max work group size: " << default_device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << "\n";
            std::cout << "Using device max work item dimensions: " << default_device.getInfo<CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS>() << "\n";
            std::cout << "Using device max clock frequency: " << default_device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>() << "\n";
            std::cout << "Using device max mem alloc size: " << default_device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() << "\n";
            std::cout << "Using device global mem size: " << default_device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() << "\n";
            std::cout << "Using device local mem size: " << default_device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() << "\n";
            std::cout << "Using device image support: " << default_device.getInfo<CL_DEVICE_IMAGE_SUPPORT>() << "\n";

            // std::cout << "Using device max constant buffer size: " << default_device.getInfo<CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE>() << "\n";
            // std::cout << "Using device max constant args: " << default_device.getInfo<CL_DEVICE_MAX_CONSTANT_ARGS>() << "\n";
            // std::cout << "Using device max parameter size: " << default_device.getInfo<CL_DEVICE_MAX_PARAMETER_SIZE>() << "\n";
            // std::cout << "Using device max samplers: " << default_device.getInfo<CL_DEVICE_MAX_SAMPLERS>() << "\n";
            // std::cout << "Using device max read image args: " << default_device.getInfo<CL_DEVICE_MAX_READ_IMAGE_ARGS>() << "\n";
            // std::cout << "Using device max write image args: " << default_device.getInfo<CL_DEVICE_MAX_WRITE_IMAGE_ARGS>() << "\n";
            // std::cout << "Using device image2d max width: " << default_device.getInfo<CL_DEVICE_IMAGE2D_MAX_WIDTH>() << "\n";
            // std::cout << "Using device image2d max height: " << default_device.getInfo<CL_DEVICE_IMAGE2D_MAX_HEIGHT>() << "\n";
            // std::cout << "Using device image3d max width: " << default_device.getInfo<CL_DEVICE_IMAGE3D_MAX_WIDTH>() << "\n";
            // std::cout << "Using device image3d max height: " << default_device.getInfo<CL_DEVICE_IMAGE3D_MAX_HEIGHT>() << "\n";
            // std::cout << "Using device image3d max depth: " << default_device.getInfo<CL_DEVICE_IMAGE3D_MAX_DEPTH>() << "\n";


            // std::cout << "Using device image max buffer size: " << default_device.getInfo<CL_DEVICE_IMAGE_MAX_BUFFER_SIZE>() << "\n";
            // std::cout << "Using device image max array size: " << default_device.getInfo<CL_DEVICE_IMAGE_MAX_ARRAY_SIZE>() << "\n";
            // std::cout << "Using device max pipe args: " << default_device.getInfo<CL_DEVICE_MAX_PIPE_ARGS>() << "\n";
            // std::cout << "Using device max global variable size: " << default_device.getInfo<CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE>() << "\n";
            // std::cout << "Using device global variable preferred total size: " << default_device.getInfo<CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE>() << "\n";
            // std::cout << "Using device max on device events: " << default_device.getInfo<CL_DEVICE_MAX_ON_DEVICE_EVENTS>() << "\n";
            // std::cout << "Using device max on device queues: " << default_device.getInfo<CL_DEVICE_MAX_ON_DEVICE_QUEUES>() << "\n";
            // std::cout << "Using device queue on device max size: " << default_device.getInfo<CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE>() << "\n";
            // std::cout << "Using device queue on device max size: " << default_device.getInfo<CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE>() << "\n";
            // std::cout << "Using device queue on device preferred size: " << default_device.getInfo<CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE>() << "\n";
            // std::cout << "Using device svm capabilities: " << default_device.getInfo<CL_DEVICE_SVM_CAPABILITIES>() << "\n";
            // std::cout << "Using device preferred platform atomic alignment: " << default_device.getInfo<CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT>() << "\n";
            // std::cout << "Using device preferred global atomic alignment: " << default_device.getInfo<CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT>() << "\n";
            // std::cout << "Using device preferred local atomic alignment: " << default_device.getInfo<CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT>() << "\n";
        }

        return default_device;
    }

    bool CheckCL(cl_int result, const char* file, int line) {
        if (result == CL_SUCCESS) return true;
        if (result == CL_PLATFORM_NOT_FOUND_KHR) FatalError("Error: CL_PLATFORM_NOT_FOUND_KHR\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_DEVICE_NOT_FOUND) FatalError("Error: CL_DEVICE_NOT_FOUND\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_DEVICE_NOT_AVAILABLE) FatalError("Error: CL_DEVICE_NOT_AVAILABLE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_COMPILER_NOT_AVAILABLE) FatalError("Error: CL_COMPILER_NOT_AVAILABLE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_MEM_OBJECT_ALLOCATION_FAILURE) FatalError("Error: CL_MEM_OBJECT_ALLOCATION_FAILURE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_OUT_OF_RESOURCES) FatalError("Error: CL_OUT_OF_RESOURCES\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_OUT_OF_HOST_MEMORY) FatalError("Error: CL_OUT_OF_HOST_MEMORY\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_PROFILING_INFO_NOT_AVAILABLE) FatalError("Error: CL_PROFILING_INFO_NOT_AVAILABLE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_MEM_COPY_OVERLAP) FatalError("Error: CL_MEM_COPY_OVERLAP\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_IMAGE_FORMAT_MISMATCH) FatalError("Error: CL_IMAGE_FORMAT_MISMATCH\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_IMAGE_FORMAT_NOT_SUPPORTED) FatalError("Error: CL_IMAGE_FORMAT_NOT_SUPPORTED\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_BUILD_PROGRAM_FAILURE) FatalError("Error: CL_BUILD_PROGRAM_FAILURE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_MAP_FAILURE) FatalError("Error: CL_MAP_FAILURE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_MISALIGNED_SUB_BUFFER_OFFSET) FatalError("Error: CL_MISALIGNED_SUB_BUFFER_OFFSET\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST) FatalError("Error: CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_VALUE) FatalError("Error: CL_INVALID_VALUE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_DEVICE_TYPE) FatalError("Error: CL_INVALID_DEVICE_TYPE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_PLATFORM) FatalError("Error: CL_INVALID_PLATFORM\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_DEVICE) FatalError("Error: CL_INVALID_DEVICE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_CONTEXT) FatalError("Error: CL_INVALID_CONTEXT\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_QUEUE_PROPERTIES) FatalError("Error: CL_INVALID_QUEUE_PROPERTIES\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_COMMAND_QUEUE) FatalError("Error: CL_INVALID_COMMAND_QUEUE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_HOST_PTR) FatalError("Error: CL_INVALID_HOST_PTR\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_MEM_OBJECT) FatalError("Error: CL_INVALID_MEM_OBJECT\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_IMAGE_FORMAT_DESCRIPTOR) FatalError("Error: CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_IMAGE_SIZE) FatalError("Error: CL_INVALID_IMAGE_SIZE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_SAMPLER) FatalError("Error: CL_INVALID_SAMPLER\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_BINARY) FatalError("Error: CL_INVALID_BINARY\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_BUILD_OPTIONS) FatalError("Error: CL_INVALID_BUILD_OPTIONS\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_PROGRAM) FatalError("Error: CL_INVALID_PROGRAM\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_PROGRAM_EXECUTABLE) FatalError("Error: CL_INVALID_PROGRAM_EXECUTABLE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_KERNEL_NAME) FatalError("Error: CL_INVALID_KERNEL_NAME\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_KERNEL_DEFINITION) FatalError("Error: CL_INVALID_KERNEL_DEFINITION\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_KERNEL) FatalError("Error: CL_INVALID_KERNEL\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_ARG_INDEX) FatalError("Error: CL_INVALID_ARG_INDEX\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_ARG_VALUE) FatalError("Error: CL_INVALID_ARG_VALUE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_ARG_SIZE) FatalError("Error: CL_INVALID_ARG_SIZE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_KERNEL_ARGS) FatalError("Error: CL_INVALID_KERNEL_ARGS\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_WORK_DIMENSION) FatalError("Error: CL_INVALID_WORK_DIMENSION\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_WORK_GROUP_SIZE) FatalError("Error: CL_INVALID_WORK_GROUP_SIZE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_WORK_ITEM_SIZE) FatalError("Error: CL_INVALID_WORK_ITEM_SIZE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_GLOBAL_OFFSET) FatalError("Error: CL_INVALID_GLOBAL_OFFSET\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_EVENT_WAIT_LIST) FatalError("Error: CL_INVALID_EVENT_WAIT_LIST\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_EVENT) FatalError("Error: CL_INVALID_EVENT\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_OPERATION) FatalError("Error: CL_INVALID_OPERATION\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_GL_OBJECT) FatalError("Error: CL_INVALID_GL_OBJECT\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_BUFFER_SIZE) FatalError("Error: CL_INVALID_BUFFER_SIZE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_MIP_LEVEL) FatalError("Error: CL_INVALID_MIP_LEVEL\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_GLOBAL_WORK_SIZE) FatalError("Error: CL_INVALID_GLOBAL_WORK_SIZE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_PROPERTY) FatalError("Error: CL_INVALID_PROPERTY\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_IMAGE_DESCRIPTOR) FatalError("Error: CL_INVALID_IMAGE_DESCRIPTOR\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_COMPILER_OPTIONS) FatalError("Error: CL_INVALID_COMPILER_OPTIONS\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_LINKER_OPTIONS) FatalError("Error: CL_INVALID_LINKER_OPTIONS\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_DEVICE_PARTITION_COUNT) FatalError("Error: CL_INVALID_DEVICE_PARTITION_COUNT\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_PIPE_SIZE) FatalError("Error: CL_INVALID_PIPE_SIZE\n%s, line %i", file, line, "OpenCL error");
        if (result == CL_INVALID_DEVICE_QUEUE) FatalError("Error: CL_INVALID_DEVICE_QUEUE\n%s, line %i", file, line, "OpenCL error");
        return false;
    }

    std::string read_file(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + path);
        }
        std::string str_source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return str_source;
    }

    std::string read_file(const std::filesystem::path& path) {
        return read_file(path.string());
    }

    std::string read_file(const std::string& directory, const std::string& filename) {
        std::string path = (std::filesystem::path{ directory } / filename).string();
        return read_file(path);
    }

    // InitCL method
    // ----------------------------------------------------------------------------
    void Kernel::init(bool profilling_enabled) {
        auto device = m_context.getInfo<CL_CONTEXT_DEVICES>().front();
        if (profilling_enabled) {
            m_queue = cl::CommandQueue(m_context, device, CL_QUEUE_PROFILING_ENABLE);
            m_queue2 = cl::CommandQueue(m_context, device, CL_QUEUE_PROFILING_ENABLE);
        }
        else {
            m_queue = cl::CommandQueue(m_context, device);
            m_queue2 = cl::CommandQueue(m_context, device);
        }
        m_cl_started = true;
    }

    // KillCL method
    // ----------------------------------------------------------------------------
    void Kernel::KillCL() {
        if (!m_cl_started) return;
        clReleaseCommandQueue(m_queue2());
        clReleaseCommandQueue(m_queue());
        clReleaseContext(m_context());
    }

    // check_cl_started method
    // ----------------------------------------------------------------------------
    void Kernel::check_cl_started() {
        if (!m_cl_started) FatalError("Call InitCL() before using OpenCL functionality.");
    }

    void Kernel::set_arg(int idx, const cl_mem* buffer) { check_cl_started(); CHECKCL(clSetKernelArg(m_kernel(), idx, sizeof(cl_mem), buffer)); }
    void Kernel::set_arg(int idx, const cl::Buffer* buffer) { check_cl_started(); CHECKCL(m_kernel.setArg(idx, *buffer)); }
    void Kernel::set_arg(int idx, const cl::Buffer& buffer) { check_cl_started(); CHECKCL(m_kernel.setArg(idx, buffer)); }
    void Kernel::set_arg(int idx, const Buffer* buffer) { check_cl_started(); set_arg(idx, buffer->get_device_buffer()); }
    void Kernel::set_arg(int idx, const Buffer& buffer) { set_arg(idx, &buffer); }
    void Kernel::set_arg(int idx, const cl::LocalSpaceArg& buffer) { check_cl_started(); CHECKCL(m_kernel.setArg(idx, buffer)); }
    void Kernel::set_arg(int idx, const int value) { check_cl_started(); CHECKCL(clSetKernelArg(m_kernel(), idx, sizeof(int), &value)); }
    void Kernel::set_arg(int idx, const float value) { check_cl_started(); CHECKCL(clSetKernelArg(m_kernel(), idx, sizeof(float), &value)); }
    void Kernel::set_arg(int idx, const glm::vec2& value) { check_cl_started(); CHECKCL(clSetKernelArg(m_kernel(), idx, sizeof(glm::vec2), &value)); }
    void Kernel::set_arg(int idx, const glm::vec3& value) { check_cl_started(); glm::vec4 tmp(value, 0); CHECKCL(clSetKernelArg(m_kernel(), idx, sizeof(glm::vec4), &tmp)); }
    void Kernel::set_arg(int idx, const glm::vec4& value) { check_cl_started(); CHECKCL(clSetKernelArg(m_kernel(), idx, sizeof(glm::vec4), &value)); }

}
