#pragma once

#ifndef OPENCL_UTILS_HPP
#define OPENCL_UTILS_HPP

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define CL_HPP_TARGET_OPENCL_VERSION 200
#include "CL/opencl.hpp"
#include "glad/gl.h"
#include "glm/glm.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <stdio.h>      /* printf */
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */


namespace CLUtils {

    cl::Device get_default_device(const bool& verbose = false);

#ifndef CHECKING_CL_ERRORS
#define FATALERROR( fmt, ... ) FatalError( "Error on line %d of %s: " fmt "\n", __LINE__, __FILE__, ##__VA_ARGS__ )

    inline void FatalError(const char* fmt, ...) {
        char t[16384];
        va_list args;
        va_start(args, fmt);
        vsnprintf(t, sizeof(t), fmt, args);
        va_end(args);
#ifdef _MSC_VER
        MessageBox(NULL, t, "Fatal error", MB_OK);
#else
        fprintf(stderr, "%s", t);
#endif
        while (1) exit(0);
    }

#define CHECKCL(r) CheckCL( r, __FILE__, __LINE__ )

#define CHECKCL_ERROR(func) \
    try {func;}  \
    catch (const cl::Error& errr) { \
        CHECKCL(errr.err()); \
    };

#endif

    // CHECKCL method
    // OpenCL error handling.
    // ----------------------------------------------------------------------------
    bool CheckCL(cl_int result, const char* file, int line);

    std::string read_file(const std::string& path);
    std::string read_file(const std::string& directory, const std::string& filename);
    std::string read_file(const std::filesystem::path& path);

    class Buffer;

    class Kernel {
        friend class Buffer;
    public:
        Kernel() = default;
        Kernel(const std::string& source, const std::string& kernel_function, const cl::Context& context) {
            m_idx = 100;
            m_context = context;
            cl::Program::Sources sources;
            sources.emplace_back(source.c_str(), source.length());
            m_program = cl::Program(m_context, sources);
            CHECKCL_ERROR(m_program.build());

            CHECKCL_ERROR(m_kernel = cl::Kernel(m_program, kernel_function.c_str(), &m_err));
            CHECKCL(m_err);
            init();
        }

        Kernel(const std::string& source, const std::string& kernel_function, const cl::Device& device)
            : Kernel{ source, kernel_function, cl::Context(device) } {}

        Kernel(const std::string& directory, const std::string& filename, const std::string& kernel_function, const cl::Context& context)
            : Kernel{ read_file(directory, filename), kernel_function, context } {}

        Kernel(const std::string& directory, const std::string& filename, const std::string& kernel_function, bool verbose = false)
            : Kernel{ read_file(directory, filename), kernel_function, cl::Context(get_default_device(verbose)) } {}

        Kernel(const std::filesystem::path& directory, const std::string& filename, const std::string& kernel_function, bool verbose = false)
            : Kernel{ read_file(directory, filename), kernel_function, cl::Context(get_default_device(verbose)) } {}

        Kernel(const std::filesystem::path& path, const std::string& kernel_function, const cl::Context& context)
            : Kernel{ read_file(path), kernel_function, context } {}

        Kernel(const std::filesystem::path& path, const std::string& kernel_function, bool verbose = false)
            : Kernel{ read_file(path), kernel_function, cl::Context(get_default_device(verbose)) } {}

        cl::Device get_context_device() { return m_context.getInfo<CL_CONTEXT_DEVICES>().front(); }
        cl::Context get_context() { return m_context; }
        cl::Program get_program() { return m_program; }
        cl::Kernel get_kernel() { return m_kernel; }
        cl::CommandQueue get_queue() { return m_queue; }

        void run(const cl::NDRange& global, const cl::NDRange& local = cl::NullRange, cl::Event* event = nullptr) {
            check_cl_started();
            CHECKCL_ERROR(m_queue.enqueueNDRangeKernel(m_kernel, cl::NullRange, global, local, nullptr, event));
        }

        // void run(const size_t count, cl::Event* event = nullptr) {
        //     check_cl_started();
        //     auto device = get_context_device();
        //     const std::size_t tmp_local{ device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() * 2 };
        //     const std::size_t tmp_global{ ((count + tmp_local - 1) / tmp_local) * tmp_local };
        //     cl::NDRange global{ cl::NDRange(tmp_global) };
        //     cl::NDRange local{ cl::NDRange(tmp_local) };
        //     run(global, local, event);
        // }
        void run(const size_t count1, const size_t count2 = 1, const size_t count3 = 1, cl::Event* event = nullptr) {
            check_cl_started();
            cl::NDRange global{ count1, count2, count3 };
            cl::NDRange local{ cl::NullRange };
            run(global, local, event);
        }

#define T_ typename
        template<T_ A> void set_args(A a) { init_args(); set_arg(0, a); }
        template<T_ A, T_ B> void set_args(A a, B b) { init_args(); S(0, a); S(1, b); }
        template<T_ A, T_ B, T_ C> void set_args(A a, B b, C c) { init_args(); S(0, a); S(1, b); S(2, c); }
        template<T_ A, T_ B, T_ C, T_ D> void set_args(A a, B b, C c, D d) { init_args(); S(0, a); S(1, b); S(2, c); S(3, d); }
        template<T_ A, T_ B, T_ C, T_ D, T_ E> void set_args(A a, B b, C c, D d, E e) { init_args(); S(0, a); S(1, b); S(2, c); S(3, d); S(4, e); }
        template<T_ A, T_ B, T_ C, T_ D, T_ E, T_ F> void set_args(A a, B b, C c, D d, E e, F f) {
            init_args(); S(0, a); S(1, b); S(2, c); S(3, d); S(4, e); S(5, f);
        }
        template<T_ A, T_ B, T_ C, T_ D, T_ E, T_ F, T_ G> void set_args(A a, B b, C c, D d, E e, F f, G g) {
            init_args(); S(0, a); S(1, b); S(2, c); S(3, d); S(4, e); S(5, f); S(6, g);
        }
        template<T_ A, T_ B, T_ C, T_ D, T_ E, T_ F, T_ G, T_ H> void set_args(A a, B b, C c, D d, E e, F f, G g, H h) {
            init_args(); S(0, a); S(1, b); S(2, c); S(3, d); S(4, e); S(5, f); S(6, g); S(7, h);
        }
        template<T_ A, T_ B, T_ C, T_ D, T_ E, T_ F, T_ G, T_ H, T_ I> void set_args(A a, B b, C c, D d, E e, F f, G g, H h, I i) {
            init_args(); S(0, a); S(1, b); S(2, c); S(3, d); S(4, e); S(5, f); S(6, g); S(7, h); S(8, i);
        }
        template<T_ A, T_ B, T_ C, T_ D, T_ E, T_ F, T_ G, T_ H, T_ I, T_ J>
        void set_args(A a, B b, C c, D d, E e, F f, G g, H h, I i, J j) {
            init_args();
            S(0, a); S(1, b); S(2, c); S(3, d); S(4, e);
            S(5, f); S(6, g); S(7, h); S(8, i); S(9, j);
        }
        template<T_ A, T_ B, T_ C, T_ D, T_ E, T_ F, T_ G, T_ H, T_ I, T_ J, T_ K>
        void set_args(A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k) {
            init_args();
            S(0, a); S(1, b); S(2, c); S(3, d); S(4, e); S(5, f);
            S(6, g); S(7, h); S(8, i); S(9, j); S(10, k);
        }
        template<T_ A, T_ B, T_ C, T_ D, T_ E, T_ F, T_ G, T_ H, T_ I, T_ J, T_ K, T_ L>
        void set_args(A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l) {
            init_args();
            S(0, a); S(1, b); S(2, c); S(3, d); S(4, e); S(5, f);
            S(6, g); S(7, h); S(8, i); S(9, j); S(10, k); S(11, l);
        }
        template<T_ A, T_ B, T_ C, T_ D, T_ E, T_ F, T_ G, T_ H, T_ I, T_ J, T_ K, T_ L, T_ M>
        void set_args(A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m) {
            init_args();
            S(0, a); S(1, b); S(2, c); S(3, d); S(4, e); S(5, f); S(6, g);
            S(7, h); S(8, i); S(9, j); S(10, k); S(11, l); S(12, m);
        }
        template<T_ A, T_ B, T_ C, T_ D, T_ E, T_ F, T_ G, T_ H, T_ I, T_ J, T_ K, T_ L, T_ M, T_ N>
        void set_args(A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m, N n) {
            init_args();
            S(0, a); S(1, b); S(2, c); S(3, d); S(4, e); S(5, f); S(6, g);
            S(7, h); S(8, i); S(9, j); S(10, k); S(11, l); S(12, m), S(13, n);
        }
        template<T_ A, T_ B, T_ C, T_ D, T_ E, T_ F, T_ G, T_ H, T_ I, T_ J, T_ K, T_ L, T_ M, T_ N, T_ O>
        void set_args(A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m, N n, O o) {
            init_args();
            S(0, a); S(1, b); S(2, c); S(3, d); S(4, e); S(5, f); S(6, g); S(7, h);
            S(8, i); S(9, j); S(10, k); S(11, l); S(12, m), S(13, n); S(14, o);
        }
        template<T_ A, T_ B, T_ C, T_ D, T_ E, T_ F, T_ G, T_ H, T_ I, T_ J, T_ K, T_ L, T_ M, T_ N, T_ O, T_ P>
        void set_args(A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m, N n, O o, P p) {
            init_args();
            S(0, a); S(1, b); S(2, c); S(3, d); S(4, e); S(5, f); S(6, g); S(7, h);
            S(8, i); S(9, j); S(10, k); S(11, l); S(12, m), S(13, n); S(14, o), S(15, p);
        }
        template<T_ A, T_ B, T_ C, T_ D, T_ E, T_ F, T_ G, T_ H, T_ I, T_ J, T_ K, T_ L, T_ M, T_ N, T_ O, T_ P, T_ Q>
        void set_args(A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m, N n, O o, P p, Q q) {
            init_args();
            S(0, a); S(1, b); S(2, c); S(3, d); S(4, e); S(5, f); S(6, g); S(7, h);
            S(8, i); S(9, j); S(10, k); S(11, l); S(12, m), S(13, n); S(14, o), S(15, p);
            S(16, q);
        }
        template<T_ A, T_ B, T_ C, T_ D, T_ E, T_ F, T_ G, T_ H, T_ I, T_ J, T_ K, T_ L, T_ M, T_ N, T_ O, T_ P, T_ Q, T_ R>
        void set_args(A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m, N n, O o, P p, Q q, R r) {
            init_args();
            S(0, a); S(1, b); S(2, c); S(3, d); S(4, e); S(5, f); S(6, g); S(7, h);
            S(8, i); S(9, j); S(10, k); S(11, l); S(12, m), S(13, n); S(14, o), S(15, p);
            S(16, q), S(17, r);
        }
        template<T_ A, T_ B, T_ C, T_ D, T_ E, T_ F, T_ G, T_ H, T_ I, T_ J, T_ K, T_ L, T_ M, T_ N, T_ O, T_ P, T_ Q, T_ R, T_ T>
        void set_args(A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m, N n, O o, P p, Q q, R r, T t) {
            init_args();
            S(0, a); S(1, b); S(2, c); S(3, d); S(4, e); S(5, f); S(6, g); S(7, h);
            S(8, i); S(9, j); S(10, k); S(11, l); S(12, m), S(13, n); S(14, o), S(15, p);
            S(16, q), S(17, r), S(18, t);
        }
        template<T_ A, T_ B, T_ C, T_ D, T_ E, T_ F, T_ G, T_ H, T_ I, T_ J, T_ K, T_ L, T_ M, T_ N, T_ O, T_ P, T_ Q, T_ R, T_ T, T_ U>
        void set_args(A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m, N n, O o, P p, Q q, R r, T t, U u) {
            init_args();
            S(0, a); S(1, b); S(2, c); S(3, d); S(4, e); S(5, f); S(6, g); S(7, h);
            S(8, i); S(9, j); S(10, k); S(11, l); S(12, m), S(13, n); S(14, o), S(15, p);
            S(16, q), S(17, r), S(18, t), S(19, u);
        }
        template<T_ T> void S(uint i, T t) { set_arg(i, t); }
        void init_args() {
            // acqBuffer = 0; /* nothing to acquire until told otherwise */
        }

    public:
        static void init(bool profilling_enabled = false);
        static void KillCL();
        static void check_cl_started();

        // set_arg methods
        // ----------------------------------------------------------------------------
        void set_arg(int idx, const cl_mem* buffer);
        void set_arg(int idx, const cl::Buffer* buffer);
        void set_arg(int idx, const cl::Buffer& buffer);
        void set_arg(int idx, const Buffer* buffer);
        void set_arg(int idx, const Buffer& buffer);
        void set_arg(int idx, const cl::LocalSpaceArg& buffer);
        void set_arg(int idx, const int value);
        void set_arg(int idx, const float value);
        void set_arg(int idx, const glm::vec2& value);
        void set_arg(int idx, const glm::vec3& value);
        void set_arg(int idx, const glm::vec4& value);

    private:
        inline static uint m_idx{ 1 };
        inline static cl::Context m_context;
        cl::Program m_program;
        inline static cl::CommandQueue m_queue, m_queue2;
        cl::Kernel m_kernel;
        inline static cl_int m_err;
        inline static bool m_cl_started{ false };
    };


    class Buffer {
    public:
        enum { DEFAULT = 1, TEXTURE = 8, TARGET = 16, READONLY = 4, WRITEONLY = 2 };

        Buffer(std::size_t count, void* data_ptr, unsigned int type) {
            if (!Kernel::m_cl_started) { Kernel::init(); }
            m_type = type;
            m_own_data = false;
            int rw_flag = CL_MEM_READ_WRITE;
            if (type & READONLY) rw_flag = CL_MEM_READ_ONLY;
            if (type & WRITEONLY) rw_flag = CL_MEM_WRITE_ONLY;
            m_aligned = false;
            if ((type & (TEXTURE | TARGET)) == 0) {
                m_size = count;
                m_texture_id = 0; // not representing a texture
                m_device_buffer = cl::Buffer(Kernel::m_context, rw_flag, m_size, 0, 0);
                m_host_buffer_ptr = (uint*) data_ptr;
            }
            else {
                m_texture_id = count; // representing texture count
                int error = 0;
                if (type == TARGET) m_device_buffer = clCreateFromGLTexture(Kernel::m_context(), CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, count, &error);
                else m_device_buffer = clCreateFromGLTexture(Kernel::m_context(), CL_MEM_READ_ONLY, GL_TEXTURE_2D, 0, count, &error);
                CHECKCL(error);
                m_host_buffer_ptr = 0;
            }
        }

        ~Buffer() {
            if (m_own_data) delete [] m_host_buffer_ptr;
            else { m_host_buffer_ptr = nullptr; }

            // if (m_texture_id) clReleaseMemObject(m_device_buffer());
            // else clReleaseMemObject(m_device_buffer());
        }

        cl::Buffer& get_device_buffer() { return m_device_buffer; }
        const cl::Buffer& get_device_buffer() const { return m_device_buffer; }
        uint* get_host_buffer() { return m_host_buffer_ptr; }

        void copy_to_device(bool blocking = true) {
            Kernel::check_cl_started();
            // if (m_type & (TEXTURE | TARGET)) return;
            if (m_host_buffer_ptr == 0) return;
            CHECKCL_ERROR(Kernel::m_queue.enqueueWriteBuffer(m_device_buffer, blocking ? CL_TRUE : CL_FALSE, 0, m_size, m_host_buffer_ptr));
        }

        void copy_to_device2(bool blocking, cl::Event* e = 0, const size_t s = 0) {
            Kernel::check_cl_started();
            // if (m_type & (TEXTURE | TARGET)) return;
            if (m_host_buffer_ptr == 0) return;
            CHECKCL_ERROR(Kernel::m_queue2.enqueueWriteBuffer(m_device_buffer, blocking ? CL_TRUE : CL_FALSE, 0, s ? s : m_size, m_host_buffer_ptr, 0, e));
        }

        void copy_from_device(bool blocking = true) {
            Kernel::check_cl_started();
            // if (m_type & (TEXTURE | TARGET)) return;
            if (m_host_buffer_ptr == 0) return;
            CHECKCL_ERROR(Kernel::m_queue.enqueueReadBuffer(m_device_buffer, blocking ? CL_TRUE : CL_FALSE, 0, m_size, m_host_buffer_ptr));
        }

        void copy_to(Buffer* buffer) {
            Kernel::check_cl_started();
            // if (m_type & (TEXTURE | TARGET)) return;
            if (m_host_buffer_ptr == 0) return;
            CHECKCL_ERROR(Kernel::m_queue.enqueueCopyBuffer(m_device_buffer, buffer->m_device_buffer, 0, 0, m_size));
        }

        void clear() {
            Kernel::check_cl_started();
            // if (m_type & (TEXTURE | TARGET)) return;
            if (m_host_buffer_ptr == 0) return;
            CHECKCL_ERROR(Kernel::m_queue.enqueueFillBuffer(m_device_buffer, 0, 0, m_size));
        }

    private:
        cl::Buffer m_device_buffer;
        uint* m_host_buffer_ptr;
        uint m_type, m_size /* in bytes */, m_texture_id;
        bool m_own_data, m_aligned;
        // cl_int m_err;
    };
}

#endif // OPENCL_UTILS_HPP