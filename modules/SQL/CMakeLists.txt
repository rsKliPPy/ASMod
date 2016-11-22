###################################################
#                                                 #
#                                                 #
#   ASMod SQL module CMake build file             #
#                                                 #
#                                                 #
###################################################

#Change this to match your module's directory structure.
set( MODULE_LIB_DIR ${MODULE_BASE_DIRECTORY}/SQL/dlls )

set( MODULE_NAME SQL )

#Set module specific linker flags here.
#Delay load SQL libraries
set( MODULE_LINK_FLAGS "/DELAYLOAD:sqlite3.dll /DELAYLOAD:libmariadb.dll" )

#Add module sources here, or using add_subdirectory.
add_sources(
	${SHARED_SOURCES}
	ASHLSQL.h
	ASHLSQL.cpp
	CASSQLModule.h
	CASSQLModule.cpp
	Module.h
	Module.cpp
)

#TODO: shouldn't be copy pasting this into the repository - Solokiller
add_subdirectory( ${CMAKE_SOURCE_DIR}/common common )
add_subdirectory( ${CMAKE_SOURCE_DIR}/keyvalues keyvalues )
add_subdirectory( ${CMAKE_SOURCE_DIR}/utility utility )

#Add public module headers here, if any.
#add_includes(
#
#)

#Process source files for inclusion.
preprocess_sources()

add_library( ${MODULE_NAME} SHARED ${PREP_SRCS} )

#Add include paths here.
target_include_directories( ${MODULE_NAME} PRIVATE
	.
	${SHARED_INCLUDE_PATHS}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/common
	${CMAKE_SOURCE_DIR}/shared
	${CMAKE_SOURCE_DIR}/external/ANGELSCRIPT/include
	${CMAKE_SOURCE_DIR}/external/ASSQL/include
	${CMAKE_SOURCE_DIR}/external/CTPL/include
	${CMAKE_SOURCE_DIR}/external/SQLite/include
	${CMAKE_SOURCE_DIR}/external/MariaDB/include
)

#Define preprocessor symbols here.
target_compile_definitions( ${MODULE_NAME} PRIVATE
	${SHARED_DEFINITIONS}
)

#Add library dependencies here.
find_library( AngelscriptLib Angelscript PATHS ${CMAKE_SOURCE_DIR}/external/ANGELSCRIPT/lib )
find_library( AngelscriptUtilsLib AngelscriptUtils PATHS ${CMAKE_SOURCE_DIR}/external/ANGELSCRIPT/lib )
find_library( AngelscriptLibD Angelscriptd PATHS ${CMAKE_SOURCE_DIR}/external/ANGELSCRIPT/lib )
find_library( AngelscriptUtilsLibD AngelscriptUtilsd PATHS ${CMAKE_SOURCE_DIR}/external/ANGELSCRIPT/lib )

find_library( ASSQL AngelscriptSQL PATHS ${CMAKE_SOURCE_DIR}/external/ASSQL/lib/ )
find_library( ASSQLD AngelscriptSQLd PATHS ${CMAKE_SOURCE_DIR}/external/ASSQL/lib/ )

if( WIN32 )
	find_library( SQLITE3 "sqlite3${CMAKE_STATIC_LIBRARY_SUFFIX}" PATHS ${CMAKE_SOURCE_DIR}/external/SQLite/lib )
	find_library( MARIADB "mariadbclient${CMAKE_STATIC_LIBRARY_SUFFIX}" PATHS ${CMAKE_SOURCE_DIR}/external/MariaDB/lib )
	find_library( MARIADBD "mariadbclientd${CMAKE_STATIC_LIBRARY_SUFFIX}" PATHS ${CMAKE_SOURCE_DIR}/external/MariaDB/lib )
	set( PTHREAD "" )
else()
	find_library( SQLITE3 "${CMAKE_SHARED_LIBRARY_PREFIX}sqlite3${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS "${GAME_BASE_PATH}" )
	find_library( MARIADB "${CMAKE_SHARED_LIBRARY_PREFIX}mariadb.so.2" PATHS "${GAME_BASE_PATH}" )
	set( MARIADBD ${MARIADB} )
	set( PTHREAD "pthread" )
	
	#This mucking about is needed because CMake can't figure out how to handle libraries not named lib<name>.a
	set( ASSQL "-l:\"${ASSQL}\"" )
	set( ASSQLD "-l:\"${ASSQLD}\"" )
endif()

target_link_libraries( ${MODULE_NAME}
	${SHARED_LIBRARY_DEPS}
	optimized ${AngelscriptLib}
	optimized ${AngelscriptUtilsLib}
	debug ${AngelscriptLibD}
	debug ${AngelscriptUtilsLibD}
	"${SQLITE3}"
	optimized "${MARIADB}"
	debug "${MARIADBD}"
	optimized "${ASSQL}"
	debug "${ASSQLD}"
)

#If the user wants automatic deployment to a game directory, set the output directory paths.
if( DEPLOY_TO_GAME )
	#CMake places libraries in /Debug or /Release on Windows, so explicitly set the paths for both.
	#On Linux, it uses LIBRARY_OUTPUT_DIRECTORY
	set_target_properties( ${MODULE_NAME} PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${MODULE_LIB_DIR}
		RUNTIME_OUTPUT_DIRECTORY_DEBUG ${MODULE_LIB_DIR}
		RUNTIME_OUTPUT_DIRECTORY_RELEASE ${MODULE_LIB_DIR}
	)
endif()

#Set 32 bit flag, any module specific flags.
set_target_properties( ${MODULE_NAME} 
	PROPERTIES COMPILE_FLAGS "${LINUX_32BIT_FLAG}" 
	LINK_FLAGS "${SHARED_LINKER_FLAGS} ${MODULE_LINK_FLAGS} ${LINUX_32BIT_FLAG}"
)

#No lib prefix.
SET_TARGET_PROPERTIES( ${MODULE_NAME} PROPERTIES PREFIX "" )

#Create filters.
create_source_groups( "${CMAKE_SOURCE_DIR}" )

#Clear sources list for next target.
clear_sources()

if( WIN32 )
	#Copy the sqlite3 dll so the exe can get to it
	add_custom_command( TARGET ${MODULE_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/external/SQLite/lib/sqlite3.dll"
		"${GAME_DIRECTORY}"
	)

	#Copy the mariadb dll so the exe can get to it
	add_custom_command( TARGET ${MODULE_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/external/MariaDB/lib/libmariadb.dll"
		"${GAME_DIRECTORY}"
	)

	#Copy the mariadbd dll so the exe can get to it
	add_custom_command( TARGET ${MODULE_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/external/MariaDB/lib/libmariadbd.dll"
		"${GAME_DIRECTORY}"
	)
else()
	#Copy the sqlite3 library so the exe can get to it
	add_custom_command( TARGET ${MODULE_NAME} PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/external/SQLite/lib/libsqlite3.so"
		"${GAME_DIRECTORY}"
	)

	#Copy the mariadb library so the exe can get to it
	add_custom_command( TARGET ${MODULE_NAME} PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/external/MariaDB/lib/libmariadb.so.2"
		"${GAME_DIRECTORY}>"
	)
endif()