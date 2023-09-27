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
if (EXISTS ${CMAKE_SOURCE_DIR}/cl_source)
    add_custom_command(TARGET main.exe POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/cl_source
        ${CMAKE_BINARY_DIR}/bin/cl_source
        )
endif()

# OpenCl examples
if (EXISTS ${CMAKE_SOURCE_DIR}/apps/cl_examples)
    add_executable(example_cl_1.exe ${CMAKE_CURRENT_SOURCE_DIR}/cl_examples/example_cl_1.cpp)
    set_target_properties(example_cl_1.exe 
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
    target_link_libraries(example_cl_1.exe PRIVATE ${ALL_LIBS})

    add_executable(add_two_values.exe ${CMAKE_CURRENT_SOURCE_DIR}/cl_examples/add_two_values.cpp)
    set_target_properties(add_two_values.exe 
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
    target_link_libraries(add_two_values.exe PRIVATE ${ALL_LIBS})

    add_executable(mat_mul.exe ${CMAKE_CURRENT_SOURCE_DIR}/cl_examples/mat_mul.cpp)
    set_target_properties(mat_mul.exe 
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
    target_link_libraries(mat_mul.exe PRIVATE ${ALL_LIBS})
endif()