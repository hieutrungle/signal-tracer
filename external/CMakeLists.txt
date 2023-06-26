if(MSVC AND NOT "${MSVC_VERSION}" LESS 1400)
	add_definitions( "/MP" )
endif()


add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

### GLFW ###
add_subdirectory (glfw)

# ### GLM ###
add_subdirectory(glm)

# ASSIMP
add_subdirectory(assimp)

# GLAD
add_subdirectory(glad)
