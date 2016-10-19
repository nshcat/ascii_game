# Set minimum required CMAKE version
cmake_minimum_required(VERSION 3.1.3)

# Deduce renderer plugin identifier from directory name
get_filename_component(PLUGIN_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

project(${PLUGIN_NAME})

# Include libut
#add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../ut ${CMAKE_CURRENT_BINARY_DIR}/ut)
#add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../log ${CMAKE_CURRENT_BINARY_DIR}/log)

# Source files
file(GLOB SOURCE_FILES src/*.cxx)

# Define target as static library
add_library(${PLUGIN_NAME} ${SOURCE_FILES})

# Add plugin include directory as public interface
target_include_directories(${PLUGIN_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/${PLUGIN_NAME}/include)

# Add parent include directories (manly for renderer_base and common) as private
target_include_directories(${PLUGIN_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include)

# Link to libut
target_link_libraries(${PLUGIN_NAME} PUBLIC ${LIBUT_LIBRARIES} ${LIBLOG_LIBRARIES})

# Set language standard
set_property(TARGET ${PLUGIN_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PLUGIN_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)