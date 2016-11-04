#######################################################################################
## ascii_game CMake Project File
##

# Version requirement and project info
cmake_minimum_required(VERSION 3.3.0)
project(ascii_game)

set(ASCII_MAKE_PLUGINS "ALL" CACHE STRING "Renderer plugins to build")
set(ASCII_LOADED_PLUGINS "")

# Include submodule libraries
add_subdirectory(cl)
add_subdirectory(log)
add_subdirectory(ut)
add_subdirectory(btt)
add_subdirectory(sol2)
add_subdirectory(asio)

# Source files
file(GLOB SOURCE_FILES src/*.cxx)

# Define target as executable
add_executable(ascii_game ${SOURCE_FILES})

# Include own include files
target_include_directories(ascii_game PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)

# Link to libraries
target_link_libraries(ascii_game PUBLIC ${LIBSOL2_LIBRARIES} ${LIBBTT_LIBRARIES} ${LIBUT_LIBRARIES} ${LIBCL_LIBRARIES} ${LIBLOG_LIBRARIES} ${ASIO_LIBRARIES})

# Enumerate all plugins and link to them
add_subdirectory(plugins)

foreach(PLUGIN ${ASCII_LOADED_PLUGINS})
	message(STATUS "Registered plugin \"${PLUGIN}\"!")
	target_link_libraries(ascii_game PUBLIC ${PLUGIN})
endforeach()
###

# Generate renderer factory header file
find_package(PythonInterp REQUIRED)

string(REPLACE ";" " " CMD_ARGS "${ASCII_LOADED_PLUGINS}")

add_custom_target(
	generate_factory_header
	COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/cmake/generate.py ${PROJECT_SOURCE_DIR}/cmake/templates/renderer_factory.hxx.template ${CMD_ARGS} > ${PROJECT_BINARY_DIR}/renderer_factory.hxx
)

set_source_files_properties(${PROJECT_BINARY_DIR}/renderer_factory.hxx PROPERTIES GENERATED 1)

add_dependencies(ascii_game generate_factory_header)
target_include_directories(ascii_game PUBLIC ${PROJECT_BINARY_DIR})
###

# Require support for at least C++14.
set_property(TARGET ascii_game PROPERTY CXX_STANDARD 14)
set_property(TARGET ascii_game PROPERTY CXX_STANDARD_REQUIRED ON)




