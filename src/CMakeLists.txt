# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

file(GLOB_RECURSE COMM_HEADER_LIST CONFIGURE_DEPENDS "${CommRay_SOURCE_DIR}/include/*.hpp" "${CommRay_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE COMM_SOURCE_LIST CONFIGURE_DEPENDS "${CommRay_SOURCE_DIR}/src/*.cpp" "${CommRay_SOURCE_DIR}/src/*.c")
# set(COMM_HEADER_LIST "${ray_tracing_SOURCE_DIR}/include/modern/lib.hpp")

# Make an automatic library - will be static or dynamic based on user setting
list(APPEND LIBRARY_SOURCES ${COMM_SOURCE_LIST} ${COMM_HEADER_LIST})
# message(STATUS "LIBRARY_SOURCES: ${LIBRARY_SOURCES}")
add_library(commRay ${LIBRARY_SOURCES})

# We need this directory, and users of our library will need it too
file(GLOB HEADER_FOLDERS LIST_DIRECTORIES true "${CommRay_SOURCE_DIR}/include/comm_ray/*")
# message(STATUS "HEADER_FOLDERS: ${HEADER_FOLDERS}")
target_include_directories(commRay PUBLIC 
${HEADER_FOLDERS}
)

# This depends on (header only) boost
# target_link_libraries(commRay PRIVATE Boost::boost compiler_flags)
# target_link_libraries(commRay PRIVATE ${OPENGL_LIBRARY} compiler_flags fmt::fmt glfw glm assimp glad)

target_link_libraries(commRay PRIVATE ${OPENGL_LIBRARY} ${EXTERNAL_LIBS})


# # All users of this library will need at least C++11
# target_compile_features(commRay PUBLIC cxx_std_11)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${COMM_HEADER_LIST} )