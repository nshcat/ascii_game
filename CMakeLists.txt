#######################################################################################
## ascii_game CMake Project File
##

# Version requirement and project info
cmake_minimum_required(VERSION 3.1.3)
project(ascii_game)

# Include submodule libraries
add_subdirectory(cl)
add_subdirectory(log)
add_subdirectory(ut)
add_subdirectory(btt)

# Source files
file(GLOB SOURCE_FILES src/*.cxx)

# Define target as executable
add_executable(ascii_game ${SOURCE_FILES})

# Include own include files
target_include_directories(ascii_game PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)

# Use crude hack to get project to compile with MSYS.
if(MINGW OR MSYS)
	# Manually set paths
	set_target_properties(ascii_game PROPERTIES COMPILE_FLAGS "-I/mingw64/include/SDL2")
		
	# Link to support libraries
	target_link_libraries(ascii_game PUBLIC mingw32 sdl2main sdl2 sdl2_image ${LIBBTT_LIBRARIES} ${LIBUT_LIBRARIES} ${LIBCL_LIBRARIES} ${LIBLOG_LIBRARIES})
	
# Use find_package to retrieve SDL2 paths
else()
	set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
	find_package(SDL2 REQUIRED)
	find_package(SDL2_image REQUIRED)
	
	# Include SDL2 include directories
	target_include_directories(ascii_game PUBLIC ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR})

	# Link to libraries
	target_link_libraries(ascii_game PUBLIC ${LIBBTT_LIBRARIES} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${LIBUT_LIBRARIES} ${LIBCL_LIBRARIES} ${LIBLOG_LIBRARIES})
endif()

# Require support for at least C++14.
set_property(TARGET ascii_game PROPERTY CXX_STANDARD 14)
set_property(TARGET ascii_game PROPERTY CXX_STANDARD_REQUIRED ON)




