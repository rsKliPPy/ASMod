###################################################
#                                                 #
#                                                 #
#   Metamod-P CMake build file                    #
#   See http://metamod.org/about.html             #
#                                                 #
#                                                 #
###################################################

set( METAMOD_LIB_DIR ${META_BASE_DIRECTORY}/metamod/dlls )

set( METAMOD_NAME metamod )

#Set Metamod specific linker flags here.
set( METAMOD_LINK_FLAGS )

if( WIN32 AND NOT MSVC )
	#Cygwin & MinGW require this. See the file itself for the reason.
	set( METAMOD_LINK_FLAGS
		${METAMOD_LINK_FLAGS}
		-Xlinker --script -Xlinker ${CMAKE_CURRENT_SOURCE_DIR}/i386pe.merge
	)
endif()

if( OPT STREQUAL "opt" OR OPT STREQUAL "opt-fast" )
	set( __INTERNALS_USE_REGPARAMS__ __INTERNALS_USE_REGPARAMS__ )
else()
	set( __INTERNALS_USE_REGPARAMS__ )
endif()

set( METAMOD_P_PERFORMANCE_MONITORING "0" CACHE BOOL "Whether to enable performance monitoring code" )

if( METAMOD_P_PERFORMANCE_MONITORING )
	set( META_PERFMON META_PERFMON )
else()
	set( META_PERFMON )
endif()

#Add Metamod sources here, or using add_subdirectory.
add_sources(
	${SHARED_SOURCES}
	api_hook.cpp
	api_hook.h
	api_info.cpp
	api_info.h
	commands_meta.cpp
	commands_meta.h
	comp_dep.h
	conf_meta.cpp
	conf_meta.h
	dllapi.cpp
	dllapi.h
	enginecallbacks.h
	engineinfo.cpp
	engineinfo.h
	engine_api.cpp
	engine_api.h
	engine_t.h
	games.h
	GiveFnptrsToDllExport.h
	game_autodetect.cpp
	game_autodetect.h
	game_support.cpp
	game_support.h
	h_export.cpp
	h_export.h
	info_name.h
	linkent.h
	linkgame.cpp
	linkplug.cpp
	log_meta.cpp
	log_meta.h
	metamod.cpp
	metamod.h
	meta_api.h
	meta_eiface.cpp
	meta_eiface.h
	mhook.cpp
	mhook.h
	mlist.cpp
	mlist.h
	mm_pextensions.h
	mplayer.cpp
	mplayer.h
	mplugin.cpp
	mplugin.h
	mqueue.cpp
	mqueue.h
	mreg.cpp
	mreg.h
	mutil.cpp
	mutil.h
	new_baseclass.h
	osdep.cpp
	osdep.h
	osdep_p.cpp
	osdep_p.h
	plinfo.h
	reg_support.cpp
	reg_support.h
	res_meta.rc
	ret_type.h
	sdk_util.cpp
	sdk_util.h
	studioapi.cpp
	studioapi.h
	support_meta.cpp
	support_meta.h
	thread_logparse.cpp
	thread_logparse.h
	tqueue.h
	types_meta.h
	vdate.cpp
	vdate.h
	vers_meta.h
)

if( WIN32 )
	add_sources(
		osdep_detect_gamedll_win32.cpp
		osdep_linkent_win32.cpp
	)
else()
	add_sources(
		osdep_detect_gamedll_linux.cpp
		osdep_linkent_linux.cpp
	)
endif()

#Add public Metamod headers here, if any.
#add_includes(
#
#)

#Process source files for inclusion.
preprocess_sources()
	
#Metamod-P's Makefile forces meta_api to recompile if info_name has changed.
set_property(SOURCE ${CURRENT_CMAKE_SOURCE_DIR}/meta_api.cpp APPEND PROPERTY OBJECT_DEPENDS ${CURRENT_CMAKE_SOURCE_DIR}/info_name.h )

add_library( ${METAMOD_NAME} SHARED ${PREP_SRCS} )

#Add include paths here.
target_include_directories( ${METAMOD_NAME} PRIVATE
	.
	${SHARED_INCLUDE_PATHS}
)

#Define preprocessor symbols here.
#__METAMOD_BUILD__ for our special eiface.h
target_compile_definitions( ${METAMOD_NAME} PRIVATE
	${SHARED_DEFINITIONS}
	METAMOD_EXPORTS
	__METAMOD_BUILD__
	${__INTERNALS_USE_REGPARAMS__}
	${META_PERFMON}
)

#Add library dependencies here.
target_link_libraries( ${METAMOD_NAME}
	${SHARED_LIBRARY_DEPS}
)

#If the user wants automatic deployment to a game directory, set the output directory paths.
if( DEPLOY_TO_GAME )
	#CMake places libraries in /Debug or /Release on Windows, so explicitly set the paths for both.
	#On Linux, it uses LIBRARY_OUTPUT_DIRECTORY
	set_target_properties( ${METAMOD_NAME} PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${METAMOD_LIB_DIR}
		RUNTIME_OUTPUT_DIRECTORY_DEBUG ${METAMOD_LIB_DIR}
		RUNTIME_OUTPUT_DIRECTORY_RELEASE ${METAMOD_LIB_DIR}
	)
endif()

#Set 32 bit flag, any metamod specific flags.
set_target_properties( ${METAMOD_NAME} 
	PROPERTIES COMPILE_FLAGS "${LINUX_32BIT_FLAG}" 
	LINK_FLAGS "${SHARED_LINKER_FLAGS} ${METAMOD_LINK_FLAGS} ${LINUX_32BIT_FLAG}"
)

#No lib prefix.
SET_TARGET_PROPERTIES( ${METAMOD_NAME} PROPERTIES PREFIX "" )

#Create filters.
create_source_groups( "${CMAKE_SOURCE_DIR}" )

#Clear sources list for next target.
clear_sources()
