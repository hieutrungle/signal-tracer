# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# set(GCC_GTEST_LINK_FLAGS "-lgtest -lgtest_main -lOpenCL")

add_executable(app.exe app.cpp)
set_target_properties(app.exe 
    PROPERTIES
    # PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
target_link_libraries(app.exe PRIVATE ${ALL_LIBS})

add_custom_command(TARGET app.exe POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shader
        ${CMAKE_BINARY_DIR}/bin/shader
        )

add_custom_command(TARGET app.exe POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets/demo_layouts
    ${CMAKE_BINARY_DIR}/bin/assets/demo_layouts
    )

# OpenCl main
add_executable(main.exe main.cpp)
set_target_properties(main.exe 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
target_link_libraries(main.exe PRIVATE ${ALL_LIBS})


# OpenCl examples
add_executable(example_cl_1.exe example_cl_1.cpp)
set_target_properties(example_cl_1.exe 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
target_link_libraries(example_cl_1.exe PRIVATE ${ALL_LIBS})

add_custom_command(TARGET example_cl_1.exe POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/cl_source
    ${CMAKE_BINARY_DIR}/bin/cl_source
    )