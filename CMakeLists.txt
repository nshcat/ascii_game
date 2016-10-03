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

# Source files
file(GLOB SOURCE_FILES src/*.cxx)

# Define target as executable
add_executable(ascii_game ${SOURCE_FILES})

# Link to libraries
target_link_libraries(ascii_game ${LIBUT_LIBRARIES} ${LIBCL_LIBRARIES} ${LIBLOG_LIBRARIES})

# Add top include directory as dependency.
target_include_directories(ascii_game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
	
# Require support for at least C++14.
set_property(TARGET ascii_game PROPERTY CXX_STANDARD 14)
set_property(TARGET ascii_game PROPERTY CXX_STANDARD_REQUIRED ON)
