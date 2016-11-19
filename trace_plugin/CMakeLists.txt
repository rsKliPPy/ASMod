###################################################
#                                                 #
#                                                 #
#   Metamod-P Trace Plugin CMake build file       #
#   See http://metamod.org/trace.html             #
#                                                 #
#                                                 #
###################################################

#Change this to match your plugin's directory structure.
set( PLUGIN_LIB_DIR ${META_BASE_DIRECTORY}/trace_plugin/dlls )

set( PLUGIN_NAME trace_plugin )

#Set plugin specific linker flags here.
set( PLUGIN_LINK_FLAGS )

set( HLSDK_3_2_OLD_EIFACE "0" CACHE BOOL "Whether to use the old SDK interface for the Trace plugin" )

if( HLSDK_3_2_OLD_EIFACE )
	set( HLSDK_3_2_OLD_EIFACE "HLSDK_3_2_OLD_EIFACE" )
else()
	set( HLSDK_3_2_OLD_EIFACE )
endif()

#Add plugin sources here, or using add_subdirectory.
add_sources(
	api_info.cpp
	dllapi.cpp
	dllapi_post.h
	dllapi_post.cpp
	engine_api.cpp
	engine_api_post.h
	engine_api_post.cpp
	h_export.cpp
	info_name.h
	log_plugin.h
	log_plugin.cpp
	meta_api.cpp
	plugin.h
	plugin.cpp
	res_meta.rc
	sdk_util.cpp
	trace_api.h
	trace_api.cpp
	vdate.cpp
)

#Add public plugin headers here, if any.
#add_includes(
#
#)

#Process source files for inclusion.
preprocess_sources()
	
#Metamod-P's Makefile forces meta_api to recompile if info_name has changed.
set_property(SOURCE ${CURRENT_CMAKE_SOURCE_DIR}/meta_api.cpp APPEND PROPERTY OBJECT_DEPENDS ${CURRENT_CMAKE_SOURCE_DIR}/info_name.h )

add_library( ${PLUGIN_NAME} SHARED ${PREP_SRCS} )

#Add include paths here.
target_include_directories( ${PLUGIN_NAME} PRIVATE
	.
	${SHARED_INCLUDE_PATHS}
)

#Define preprocessor symbols here.
target_compile_definitions( ${PLUGIN_NAME} PRIVATE
	${SHARED_DEFINITIONS}
	${HLSDK_3_2_OLD_EIFACE}
)

#Add library dependencies here.
target_link_libraries( ${PLUGIN_NAME}
	${SHARED_LIBRARY_DEPS}
)

#If the user wants automatic deployment to a game directory, set the output directory paths.
if( DEPLOY_TO_GAME )
	#CMake places libraries in /Debug or /Release on Windows, so explicitly set the paths for both.
	#On Linux, it uses LIBRARY_OUTPUT_DIRECTORY
	set_target_properties( ${PLUGIN_NAME} PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_LIB_DIR}
		RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PLUGIN_LIB_DIR}
		RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_LIB_DIR}
	)
endif()

#Set 32 bit flag, any plugin specific flags.
set_target_properties( ${PLUGIN_NAME} 
	PROPERTIES COMPILE_FLAGS "${LINUX_32BIT_FLAG}" 
	LINK_FLAGS "${SHARED_LINKER_FLAGS} ${PLUGIN_LINK_FLAGS} ${LINUX_32BIT_FLAG}"
)

#No lib prefix.
SET_TARGET_PROPERTIES( ${PLUGIN_NAME} PROPERTIES PREFIX "" )

#Create filters.
create_source_groups( "${CMAKE_SOURCE_DIR}" )

#Clear sources list for next target.
clear_sources()
