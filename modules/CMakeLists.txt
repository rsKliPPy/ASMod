#The name of the string object used in modules. For SC interop we need to use a different name so it doesn't interfere with the original string type.
set( AS_STRING_OBJNAME "string" CACHE STRING "The name of the object type used to represent strings" )

#Base directory for all modules.
set( MODULE_BASE_DIRECTORY ${META_BASE_DIRECTORY}/ASMod/modules )

#Shared source files.
add_subdirectory( ${CMAKE_SOURCE_DIR}/shared shared )

#Process source files for inclusion.
preprocess_sources()

set( SHARED_MODULE_SOURCES
	${PREP_SRCS}
)

#Clear the sources list.
clear_sources()

#Shared module include paths.
set( SHARED_MODULE_INCLUDES
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/common
	${CMAKE_SOURCE_DIR}/shared
	${CMAKE_SOURCE_DIR}/external/ANGELSCRIPT/include
)

#Shared module preprocessor definitions.
set( SHARED_MODULE_DEFS
	AS_STRING_OBJNAME="${AS_STRING_OBJNAME}"
)

add_subdirectory( stub_module )
add_subdirectory( SCInterop )
add_subdirectory( SQL )
