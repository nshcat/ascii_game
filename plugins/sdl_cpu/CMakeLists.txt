###################################################################
# ascii_game plugin project file
#
# This template automatically registers a renderer plugin
###################################################################

# Set minimum required CMAKE version
cmake_minimum_required(VERSION 3.1.3)
project(sdl_cpu)

include("${CMAKE_CURRENT_LIST_DIR}/../template.cmake")

# Use crude hack to get project to compile with MSYS.
if(MINGW OR MSYS)
	# Manually set paths
	set_target_properties(${PLUGIN_NAME} PROPERTIES COMPILE_FLAGS "-I/mingw64/include/SDL2")
		
	# Link to support libraries
	target_link_libraries(${PLUGIN_NAME} PUBLIC mingw32 sdl2main sdl2 sdl2_image)
	
# Use find_package to retrieve SDL2 paths
else()
	set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake)
	find_package(SDL2 REQUIRED)
	find_package(SDL2_image REQUIRED)
	
	# Include SDL2 include directories
	target_include_directories(${PLUGIN_NAME} PUBLIC ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR})

	# Link to libraries
	target_link_libraries(${PLUGIN_NAME} PUBLIC ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY})
endif()