# Testing library
# FetchContent_Declare(
#   catch
#   GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#   GIT_TAG v2.13.6)
# FetchContent_MakeAvailable(catch)
# Adds Catch2::Catch2

# # Tests need to be added as executables first
# add_executable(testlib testlib.cpp)

# # I'm using C++17 in the test
# target_compile_features(testlib PRIVATE cxx_std_17)

# # Should be linked to the main library, as well as the Catch2 testing library
# target_link_libraries(testlib PRIVATE modern_library Catch2::Catch2)

# # If you register a test, then ctest and make test will run it.
# # You can also run examples and check the output, as well.
# add_test(NAME testlibtest COMMAND testlib) # Command can be a target

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


file(GLOB_RECURSE TEST_SOURCE_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/tests/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE TEST_HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/tests/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.c")

add_executable(gtest_main.exe gtest_main.cpp ${TEST_SOURCE_LIST} ${TEST_HEADER_LIST})
set_target_properties(gtest_main.exe 
    PROPERTIES
    # PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
target_link_libraries(gtest_main.exe PRIVATE ${ALL_LIBS} GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(gtest_main.exe)