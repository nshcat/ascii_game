
# Set minimum required CMAKE version
cmake_minimum_required(VERSION 3.3.0)

# subdirlist - Retrieve a list of all subdirectories
macro(subdirlist result curdir)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
        list(APPEND dirlist ${child})
    endif()
  endforeach()
  set(${result} ${dirlist})
endmacro()
# ---

# Fetch list of subdirectories
subdirlist(DIRS ${CMAKE_CURRENT_LIST_DIR})

# Register plugins if requested
foreach(SUBDIR ${DIRS})
	message(STATUS "Found renderer plugin \"${SUBDIR}\"!")
	#if(${SUBDIR} IN_LIST ASCII_MAKE_PLUGINS)
	add_subdirectory(${SUBDIR})
	
	if("${ASCII_LOADED_PLUGINS}" STREQUAL "")
		set(ASCII_LOADED_PLUGINS "${SUBDIR}" PARENT_SCOPE)
	else()
		set(ASCII_LOADED_PLUGINS "${ASCII_LOADED_PLUGINS};${SUBDIR}" PARENT_SCOPE)
	endif()
		
	#endif()
endforeach()
