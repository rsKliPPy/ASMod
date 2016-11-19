###################################################
#                                                 #
#                                                 #
#   Metamod-P CMake build file                    #
#   Based on the Metamod-P Makefile               #
#   Written by Sam "Solokiller" Vanheer           #
#                                                 #
#                                                 #
###################################################
cmake_minimum_required( VERSION 3.6 )

# function to collect all the sources from sub-directories
# into a single list
function(add_sources)
  get_property(is_defined GLOBAL PROPERTY SRCS_LIST DEFINED)
  if(NOT is_defined)
    define_property(GLOBAL PROPERTY SRCS_LIST
      BRIEF_DOCS "List of source files"
      FULL_DOCS "List of source files to be compiled in one library")
  endif()
  # make absolute paths
  set(SRCS)
  foreach(s IN LISTS ARGN)
    if(NOT IS_ABSOLUTE "${s}")
      get_filename_component(s "${s}" ABSOLUTE)
    endif()
    list(APPEND SRCS "${s}")
  endforeach()
  # append to global list
  set_property(GLOBAL APPEND PROPERTY SRCS_LIST "${SRCS}")
endfunction(add_sources)

# preprocess sources
function( preprocess_sources )
	set(PREP_SRCS PARENT_SCOPE)
	get_property(SRCS GLOBAL PROPERTY SRCS_LIST)
	foreach(s IN LISTS SRCS)
	  file(RELATIVE_PATH rs "${CMAKE_CURRENT_SOURCE_DIR}" "${s}")
	  string(REGEX REPLACE "r$" "" o "${CMAKE_CURRENT_BINARY_DIR}/${rs}")
	  add_custom_command(
		OUTPUT "${o}"
		COMMAND ${CMAKE_COMMAND} -E copy "${s}" "${o}"
		DEPENDS "${s}"
		COMMENT "Creating ${o}"
		VERBATIM
		)
	  list(APPEND PREP_SRCS "${s}")
	endforeach()
	set(PREP_SRCS ${PREP_SRCS} PARENT_SCOPE)
endfunction( preprocess_sources )

function( create_source_groups _src_root_path )
	get_property(SRCS GLOBAL PROPERTY SRCS_LIST)
	foreach(_source IN ITEMS ${SRCS})
		get_filename_component(_source_path "${_source}" PATH)
		file(RELATIVE_PATH _source_path_rel "${_src_root_path}" "${_source_path}")
		string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
		source_group("${_group_path}" FILES "${_source}")
	endforeach()
endfunction( create_source_groups )

#Function to clear the sources list.
function( clear_sources )
	set_property( GLOBAL PROPERTY SRCS_LIST "" )
	set( PREP_SRCS PARENT_SCOPE )
endfunction( clear_sources )

# Function to add include files.
function( add_includes )
  get_property(is_defined GLOBAL PROPERTY INCLUDES_LIST DEFINED)
  if(NOT is_defined)
    define_property(GLOBAL PROPERTY INCLUDES_LIST
      BRIEF_DOCS "List of include files"
      FULL_DOCS "List of include files to be compiled in one library")
  endif()
  # make absolute paths
  set(INCLUDES)
  foreach(s IN LISTS ARGN)
    if(NOT IS_ABSOLUTE "${s}")
      get_filename_component(s "${s}" ABSOLUTE)
    endif()
    list(APPEND INCLUDES "${s}")
  endforeach()
  # append to global list
  set_property(GLOBAL APPEND PROPERTY INCLUDES_LIST "${INCLUDES}")
endfunction( add_includes )

# Function to install includes.
function( install_includes _include_root_path )
	get_property(INCLUDES GLOBAL PROPERTY INCLUDES_LIST)
	
	foreach(_include IN ITEMS ${INCLUDES})
		get_filename_component(_include_path "${_include}" PATH)
		file(RELATIVE_PATH _include_path_rel "${_include_root_path}" "${_include_path}")
		string(REPLACE "/" "\\" _group_path "${_include_path_rel}")
		install( FILES "${_include}" DESTINATION "include/${_group_path}" )
	endforeach()
	
	set_property( GLOBAL PROPERTY INCLUDES_LIST "" )
endfunction( install_includes )

macro(configure_msvc_runtime)
  if(MSVC)
    # Default to statically-linked runtime.
    if("${MSVC_RUNTIME}" STREQUAL "")
      set(MSVC_RUNTIME "static")
    endif()
    # Set compiler options.
    set(variables
      CMAKE_C_FLAGS_DEBUG
      CMAKE_C_FLAGS_MINSIZEREL
      CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_RELWITHDEBINFO
      CMAKE_CXX_FLAGS_DEBUG
      CMAKE_CXX_FLAGS_MINSIZEREL
      CMAKE_CXX_FLAGS_RELEASE
      CMAKE_CXX_FLAGS_RELWITHDEBINFO
    )
    if(${MSVC_RUNTIME} STREQUAL "static")
      message(STATUS
        "MSVC -> forcing use of statically-linked runtime."
      )
      foreach(variable ${variables})
        if(${variable} MATCHES "/MD")
          string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
        endif()
      endforeach()
    else()
      message(STATUS
        "MSVC -> forcing use of dynamically-linked runtime."
      )
      foreach(variable ${variables})
        if(${variable} MATCHES "/MT")
          string(REGEX REPLACE "/MT" "/MD" ${variable} "${${variable}}")
        endif()
      endforeach()
    endif()
  endif()
endmacro()

#
#Metamod-P project begin.
#

project( Metamod-P )

#Needs static CRT to avoid alloc issues.
set( MSVC_RUNTIME "static" )

configure_msvc_runtime()

set(variables
  CMAKE_C_FLAGS_DEBUG
  CMAKE_C_FLAGS_MINSIZEREL
  CMAKE_C_FLAGS_RELEASE
  CMAKE_C_FLAGS_RELWITHDEBINFO
  CMAKE_CXX_FLAGS_DEBUG
  CMAKE_CXX_FLAGS_MINSIZEREL
  CMAKE_CXX_FLAGS_RELEASE
  CMAKE_CXX_FLAGS_RELWITHDEBINFO
)
message(STATUS "Initial build flags:")
foreach(variable ${variables})
  message(STATUS "  '${variable}': ${${variable}}")
endforeach()
message(STATUS "")

#GoldSource is exclusively 32 bit so make sure it's always compatible.
if( UNIX )
	set( LINUX_32BIT_FLAG "-m32" )
else()
	set( LINUX_32BIT_FLAG "" )
endif()

set( DEPLOY_TO_GAME "0" CACHE BOOL "Whether to deploy libraries to the given game directory" )

set( GAME_DIRECTORY "" CACHE PATH "Path to the game directory to deploy libraries to if DEPLOY_TO_GAME is enabled" )

#Can't deploy without a game directory.
if( NOT GAME_DIRECTORY AND DEPLOY_TO_GAME )
	set( DEPLOY_TO_GAME "0" )
	MESSAGE( WARNING "Can't deploy without a game directory!" )
endif()

#The base directory for all Metamod data.
set( META_BASE_DIRECTORY ${GAME_DIRECTORY}/addons )

#Configurable by user.
set( SDK_DIRECTORY "${CMAKE_SOURCE_DIR}/hlsdk" CACHE PATH "Path to the Half-Life SDK directory" )

#Set global compiler settings.
if( WIN32 )
	#Warning level 4. Wall shows warnings in standard headers so we can't compile if that's enabled.
	#Treat warnings as errors.
	set( GLOBAL_COMPILER_FLAGS "/W4 /WX" )
	
	#Optimize for speed.
	#Omit frame pointers.
	#Precise floating point operations.
	set( GLOBAL_COMPILER_FLAGS_RELEASE "/O2 /Oy /fp:precise" )
else()
	#All warnings.
	#No unknown pragma directives.
	#No attributes.
	set( GLOBAL_COMPILER_FLAGS "-Wall -Wno-unknown-pragmas -Wno-attributes" )
	
	#Optimize for speed.
	#Omit frame pointers.
	#Precise floating point operations.
	set( GLOBAL_COMPILER_FLAGS_RELEASE "-O2 -fomit-frame-pointer -ffast-math" )
endif()

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GLOBAL_COMPILER_FLAGS}" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GLOBAL_COMPILER_FLAGS}" )
set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${GLOBAL_COMPILER_FLAGS_RELEASE}" )
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${GLOBAL_COMPILER_FLAGS_RELEASE}" )
set( CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${GLOBAL_COMPILER_FLAGS_RELEASE}" )
set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${GLOBAL_COMPILER_FLAGS_RELEASE}" )

#Shared include paths.
set( SHARED_INCLUDE_PATHS
	${CMAKE_SOURCE_DIR}/metamod
	${SDK_DIRECTORY}/engine
	${SDK_DIRECTORY}/common
	${SDK_DIRECTORY}/pm_shared
	${SDK_DIRECTORY}/dlls
	${SDK_DIRECTORY}
)

#Shared preprocessor definitions.
set( SHARED_DEFINITIONS
)

if( WIN32 )
	#Disable insecure CRT functions warning.
	set( SHARED_DEFINITIONS
		${SHARED_DEFINITIONS}
		_CRT_SECURE_NO_WARNINGS
	)
endif()

#Shared library dependencies.
set( SHARED_LIBRARY_DEPS
)

if( NOT WIN32 )
	set( SHARED_LIBRARY_DEPS
		${SHARED_LIBRARY_DEPS}
		ldl
		lm
		static-libgcc
	)	
endif()

#Shared linker flags.
set( SHARED_LINKER_FLAGS
)

if( WIN32 )
	#Alias stdcall for export.
	if( NOT MSVC )
		set( SHARED_LINKER_FLAGS
			${SHARED_LINKER_FLAGS}
			-Xlinker --add-stdcall-alias
		)
	endif()
else()
	set( SHARED_LINKER_FLAGS
		${SHARED_LINKER_FLAGS}
		-fPIC
	)
endif()

#Add subdirectories here.
add_subdirectory( metamod )
add_subdirectory( stub_plugin )
add_subdirectory( trace_plugin )
add_subdirectory( wdmisc_plugin )

#
#Metamod-P project end.
#
