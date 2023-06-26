set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(GCC_GTEST_LINK_FLAGS "-lgtest -lgtest_main")

# add_executable(app.exe app.cpp)
# set_target_properties(app.exe 
#     PROPERTIES
#     # PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
#     ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#     LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#     RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
#     )
# target_link_libraries(app.exe PRIVATE ${ALL_LIBS})


# add_executable(cube.exe cube.cpp)
# set_target_properties(cube.exe 
#     PROPERTIES
#     # PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
#     ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#     LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#     RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
#     )
# target_link_libraries(cube.exe PRIVATE ${ALL_LIBS})

add_executable(main.exe main.cpp)
set_target_properties(main.exe 
    PROPERTIES
    # PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
target_link_libraries(main.exe PRIVATE ${ALL_LIBS})