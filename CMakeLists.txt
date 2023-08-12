cmake_minimum_required(VERSION 3.14...3.26)

if(${CMAKE_VERSION} VERSION_LESS 3.26)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.26)
endif()

project(CommRay VERSION 1.0
                  DESCRIPTION "Signal Tracing"
                  LANGUAGES CXX)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()


# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # Optionally set things like CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE here
    # specify the C++ standard
    set(CMAKE_DEBUG_POSTFIX d)
    add_library(compiler_flags INTERFACE)
    target_compile_features(compiler_flags INTERFACE cxx_std_20)

    # add compiler warning flags just when building this project via
    # the BUILD_INTERFACE genex
    set(GCC_LIKE_CXX "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
    set(MSVC_CXX "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

    target_compile_options(compiler_flags INTERFACE
    "$<${GCC_LIKE_CXX}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused;-Werror;-lglfw3;-lGL;-lX11;-fopenmp;-lpthread;-lXrandr;-lXi;-ldl;-lOpenCL>>"
    "$<$<CONFIG:DEBUG>:-g3>"
    "$<$<CONFIG:DEBUG>:-Og>"
    "$<$<CONFIG:RELEASE>:-O3>"
    "$<${MSVC_CXX}:$<BUILD_INTERFACE:-W3>>")


    # Let's ensure -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Testing only available if this is the main app
    # Note this needs to be done in the main CMakeLists
    # since it calls enable_testing, which must be in the
    # main CMakeLists.
    include(CTest)

    # Docs only available if this is the main app
    find_package(Doxygen)
    if(Doxygen_FOUND)
        add_subdirectory(docs)
    else()
        message(STATUS "Doxygen not found, not building docs")
    endif()
endif()



# FetchContent added in CMake 3.11, downloads during the configure step
# FetchContent_MakeAvailable was added in CMake 3.14; simpler usage
include(FetchContent)

# Accumulator library
# This is header only, so could be replaced with git submodules or FetchContent
find_package(Boost REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenCL REQUIRED)
find_package(OpenMP REQUIRED)
# Adds Boost::boost

# Gtest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Formatting library
FetchContent_Declare(
  fmtlib
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git)
FetchContent_MakeAvailable(fmtlib)
# Adds fmt::fmt

# OpenMP
if(NOT TARGET OpenMP::OpenMP_CXX)
    find_package(Threads REQUIRED)
    add_library(OpenMP::OpenMP_CXX IMPORTED INTERFACE)
    set_property(TARGET OpenMP::OpenMP_CXX
                 PROPERTY INTERFACE_COMPILE_OPTIONS ${OpenMP_CXX_FLAGS})
    # Only works if the same flag is passed to the linker; use CMake 3.9+ otherwise (Intel, AppleClang)
    set_property(TARGET OpenMP::OpenMP_CXX
                 PROPERTY INTERFACE_LINK_LIBRARIES ${OpenMP_CXX_FLAGS} Threads::Threads)
endif()

# Compile external dependencies 
add_subdirectory(external)

set(EXTERNAL_LIBS
    ${OPENGL_LIBRARY}
    ${OpenCL_LIBRARIES}
    # OpenCL::OpenCL
    OpenMP::OpenMP_CXX
    compiler_flags
    fmt::fmt
    glfw
    glm
    assimp
    glad
)

# The compiled library code is here
add_subdirectory(src)

set(ALL_LIBS
    commRay
	${EXTERNAL_LIBS}
)

# The executable code is here
add_subdirectory(apps)

# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
# if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING)
#    AND BUILD_TESTING)
#   add_subdirectory(tests)
# endif()