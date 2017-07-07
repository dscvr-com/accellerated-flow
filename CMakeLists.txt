# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (Tutorials)

find_package(OpenGL REQUIRED)


if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)

include_directories(
	external/AntTweakBar-1.16/include/
	external/glfw-3.1.2/include/GLFW/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	external/assimp-3.0.1270/include/
	external/bullet-2.81-rev2613/src/
	external/bitmap
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
	BITMAP_LIBRARY
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

# Tutorial 14
add_executable(OpticalFlow_OpenGL
	OpticalFlow_OpenGL/tutorial14.cpp
	OpticalFlow_OpenGL/OpenGL.cpp
	OpticalFlow_OpenGL/ShaderManager.cpp
	OpticalFlow_OpenGL/ConvenienceHelper.cpp
	
	OpticalFlow_OpenGL/OpenGL.h
	OpticalFlow_OpenGL/ShaderManager.h
	OpticalFlow_OpenGL/Constants.h
	OpticalFlow_OpenGL/ConvenienceHelper.h
	
	common/shader.cpp
	common/shader.hpp
	common/controls.cpp
	common/controls.hpp
	common/texture.cpp
	common/texture.hpp
	common/objloader.cpp
	common/objloader.hpp
	common/vboindexer.cpp
	common/vboindexer.hpp
	common/text2D.hpp
	common/text2D.cpp
	
	OpticalFlow_OpenGL/StandardShadingRTT.vertexshader
	OpticalFlow_OpenGL/VerticalConvolution.fragmentshader
	OpticalFlow_OpenGL/Passthrough.vertexshader
	OpticalFlow_OpenGL/WobblyTexture.fragmentshader
	OpticalFlow_OpenGL/HorizontalConvolution.fragmentshader
	OpticalFlow_OpenGL/MatrixUpdate.fragmentshader
	OpticalFlow_OpenGL/HorizontalGaussBlur.fragmentshader
	OpticalFlow_OpenGL/VerticalGaussBlur.fragmentshader
	OpticalFlow_OpenGL/FlowUpdateShader.fragmentshader
)
target_link_libraries(OpticalFlow_OpenGL
	${ALL_LIBS}
)


# Xcode and Visual working directories
set_target_properties(OpticalFlow_OpenGL PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tutorial14_render_to_texture/")
create_target_launcher(OpticalFlow_OpenGL WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tutorial14_render_to_texture/")

SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )


if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
   TARGET OpticalFlow_OpenGL POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/OpticalFlow_OpenGL${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/OpticalFlow_OpenGL/"
)

elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

