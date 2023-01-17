#[=======================================================================[.rst:

GenPkgConfig
------------

This is the library helping you to generate and install pkg-config files.

Unlicense
^^^^^^^^^

This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this software, either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this software dedicate any and all copyright interest in the software to the public domain. We make this dedication for the benefit of the public at large and to the detriment of our heirs and successors. We intend this dedication to be an overt act of relinquishment in perpetuity of all present and future rights to this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
For more information, please refer to <https://unlicense.org/>

Warning
^^^^^^^

CMake is currently merging a built-in impl of pkg-config file generator! https://gitlab.kitware.com/cmake/cmake/-/merge_requests/6363

Functions
^^^^^^^^^
.. command:: configure_pkg_config_file

  .. versionadded:: 3.22

  Generates a pkg-config file for 

  ::

    configure_pkg_config_file(<targetName>
        NAME <name of the package>
        VERSION <version to be written into the package>
        DESCRIPTION <description to be written into the package>
        URL <homepage URL to be written into the package>
        COMPONENT <install as the component>
        INSTALL_LIB_DIR <path to something like CMAKE_INSTALL_LIBDIR>
        INSTALL_INCLUDE_DIR <path to something like CMAKE_INSTALL_INCLUDEDIR>
        REQUIRES ... <list of pkg-config packages this one depends on> ...
        REQUIRES ... <list of pkg-config packages this one conflicts with> ...
    )

    The arguments are optional and usually are not needed to be set if global (not component-specific) CPACK vars have been set before.

    Generation is done in build time using packaging expressions.

#]=======================================================================]

set("GEN_PKG_CONFIG_WORKAROUNDS_BUILD_TIME_SCRIPTS" "${CMAKE_CURRENT_LIST_DIR}/buildTimeScripts")

cmake_policy(SET CMP0070 NEW)

function(configure_pkg_config_file TARGET)
	cmake_parse_arguments(""
		"" # options
		"NAME;VERSION;DESCRIPTION;URL;COMPONENT;INSTALL_LIB_DIR;INSTALL_INCLUDE_DIR" # one_value_keywords
		"REQUIRES;CONFLICTS" # multi_value_keywords
		${ARGN}
	)

	configure_pkg_config_file_vars("${TARGET}" "${_NAME}" "${_INSTALL_LIB_DIR}" "${_INSTALL_INCLUDE_DIR}" "${_COMPONENT}" "${_DESCRIPTION}" "${_URL}" "${_VERSION}" "${_REQUIRES}" "${_CONFLICTS}")
endfunction()

function(ge_expr_basename inputExpr outVar)
	set("${outVar}" "$<TARGET_PROPERTY:${inputExpr}>" PARENT_SCOPE)
endfunction()

function(configure_pkg_config_file_vars TARGET _NAME _INSTALL_LIB_DIR _INSTALL_INCLUDE_DIR _COMPONENT _DESCRIPTION _URL _VERSION _REQUIRES _CONFLICTS)
	#$<TARGET_PROPERTY:${TARGET},NAME>
	#INTERFACE_LINK_DIRECTORIES
	#INTERFACE_LINK_LIBRARIES
	#INTERFACE_LINK_OPTIONS

	if(_NAME)
	else()
		set(_NAME "$<TARGET_PROPERTY:${TARGET},NAME>")
	endif()

	if(_DESCRIPTION)
	else()
		set(_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")
	endif()

	if(_VERSION)
	else()
		set(_VERSION "${CPACK_PACKAGE_VERSION}")
	endif()

	if(_URL)
	else()
		set(_URL "${CPACK_PACKAGE_HOMEPAGE_URL}")
	endif()

	if(INSTALL_INCLUDE_DIR)
	else()
		set(INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
	endif()

	if(INSTALL_LIB_DIR)
	else()
		set(INSTALL_LIB_DIR "${CMAKE_INSTALL_LIBDIR}")
	endif()

	set(PKG_CONFIG_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${_NAME}.pc")

	set(PUBLIC_INCLUDES "$<TARGET_PROPERTY:${TARGET},INTERFACE_INCLUDE_DIRECTORIES>")
	set(PUBLIC_LIBS "$<TARGET_PROPERTY:${TARGET},INTERFACE_LINK_LIBRARIES>")
	set(PUBLIC_COMPILE_FLAGS "$<TARGET_PROPERTY:${TARGET},INTERFACE_COMPILE_DEFINITIONS>")

	set("IS_INTERFACE" "$<STREQUAL:$<TARGET_PROPERTY:${TARGET},TYPE>,INTERFACE_LIBRARY>")
	set("IS_OBJECT" "$<STREQUAL:$<TARGET_PROPERTY:${TARGET},TYPE>,OBJECT_LIBRARY>")
	get_target_property(CONFIGURE_TIME_TARGET_TYPE "${TARGET}" TYPE)
	if(CONFIGURE_TIME_TARGET_TYPE STREQUAL OBJECT_LIBRARY)
		set(CONFIGURE_TIME_IS_OBJECT ON)  # Special measures have to be taken!!!
	endif()

	set("NEEDS_LIBS" "$<AND:$<NOT:${IS_INTERFACE}>,$<NOT:${IS_OBJECT}>>")
	set("NEEDS_LIB_DIR" "$<NOT:${IS_INTERFACE}>")
	string(REPLACE "," "$<COMMA>" NEEDS_LIBS_ESCAPED "${NEEDS_LIBS}")
	string(REPLACE ">" "$<ANGLE-R>" NEEDS_LIBS_ESCAPED "${NEEDS_LIBS_ESCAPED}")

	list(APPEND header "prefix=${CMAKE_INSTALL_PREFIX}")
	list(APPEND header "$<IF:$<OR:$<BOOL:${PUBLIC_LIBS}>,${NEEDS_LIB_DIR}>,libdir=\${prefix}/${INSTALL_LIB_DIR},>")
	list(APPEND header "$<IF:$<BOOL:${PUBLIC_INCLUDES}>,includedir=\${prefix}/${INSTALL_INCLUDE_DIR},>")


	list(APPEND libSpecific "Name: ${_NAME}")
	if(_DESCRIPTION)
		list(APPEND libSpecific "Description: ${_DESCRIPTION}")
	endif()
	if(_URL)
		list(APPEND libSpecific "URL: ${_URL}")
	endif()
	if(_VERSION)
		list(APPEND libSpecific "Version: ${_VERSION}")
	endif()
	if(_REQUIRES)
		list(APPEND libSpecific "Requires: ${_REQUIRES}")
	endif()
	if(_CONFLICTS)
		list(APPEND libSpecific "Conflicts: ${_CONFLICTS}")
	endif()

	set(OTHER_INCLUDE_FLAGS "-I$<JOIN:$<REMOVE_DUPLICATES:${PUBLIC_INCLUDES}>, -I>")  # Not needed, we can only get build interface flags here. Insert them after -I\${includedir} if you find a way to fix/workaround it

	# Here is a workaround to inability to use TARGET_LINKER_FILE_NAME for targets not involving library generation.
	# Strangely $<IF evaluates both branches, not only the one taken, which causes an error
	# We workaround it by generating the subexpression source using $<IF and then evaluating it using $<GENEX_EVAL
	# Of course we could have used conditional expressions on CMake script part, but I have decided to implement it in generator expressions part, so hypthetically all the expressions can be merged into a single file and this function can be made simple

	set(ESCAPED_GENEXPR_BEGINNING "$<1:$><")  # A hack because there is no escape for `$` or `<` or `$<`.  So we just disrupt $< into pieces
	set(CURRENT_LIB_ESCAPED_BINARY_NAME "${ESCAPED_GENEXPR_BEGINNING}TARGET_LINKER_FILE_NAME:${TARGET}$<ANGLE-R>")
	set(LINK_CURRENT_LIB_FLAG "$<GENEX_EVAL:$<IF:${NEEDS_LIBS},-l:${CURRENT_LIB_ESCAPED_BINARY_NAME},>>")

	if(CONFIGURE_TIME_IS_OBJECT)
		set(IS_TARGET_OBJECTS_CONFIGURE_TIME_UNAVAILABLE ON)
		if(IS_TARGET_OBJECTS_CONFIGURE_TIME_UNAVAILABLE)
			message(WARNING "CMake is shit: There is (at least was at the time of writing of this code) no generator expression to get only basenames of object files. They are also unavailable at configure stage. And there were no CMake generator expressions for making replacements in strings. So we workaround this.")
			set(TARGET_OBJECTS_FILE "${TARGET}.obj_list")
			set(OBJECTS_FILE_RETRIEVAL_TARGET_NAME "${TARGET}_get_objects_list")

			set(PKGCONFIG_DUMMY_UNFINISHED_GEN_TARGET_NAME "${TARGET}_pkgconfig_unfinished")
			set(PKGCONFIG_PATCH_TARGET_NAME "${TARGET}_patch_pkgconfig")

			set(PKG_CONFIG_FILE_NAME_FINISHED "${PKG_CONFIG_FILE_NAME}")
			set(PKG_CONFIG_FILE_NAME_UNFINISHED "${PKG_CONFIG_FILE_NAME_FINISHED}.unfinished")

			file(GENERATE OUTPUT "${TARGET_OBJECTS_FILE}" CONTENT "$<TARGET_OBJECTS:${TARGET}>")

			add_custom_command(
				OUTPUT "${TARGET_OBJECTS_FILE}"
				COMMENT "A dummy command to workaround cmake limitations"
			)
			add_custom_target("${OBJECTS_FILE_RETRIEVAL_TARGET_NAME}"
				DEPENDS "${TARGET_OBJECTS_FILE}"
			)

			add_custom_command(
				OUTPUT "${PKG_CONFIG_FILE_NAME_FINISHED}"
				PRE_BUILD COMMAND ${CMAKE_COMMAND} "-DobjectsFile=\"${TARGET_OBJECTS_FILE}\"" "-DpkgConfigFileUnlinished=\"${PKG_CONFIG_FILE_NAME_UNFINISHED}\"" "-DpkgConfigFileFinal=\"${PKG_CONFIG_FILE_NAME_FINISHED}\"" "-P" "${GEN_PKG_CONFIG_WORKAROUNDS_BUILD_TIME_SCRIPTS}/getObjectFilesBaseNames.cmake"
				MAIN_DEPENDENCY "${TARGET_OBJECTS_FILE}"
				DEPENDS "${PKG_CONFIG_FILE_NAME_UNFINISHED}"
				COMMENT "Working around CMake limitations about getting list of basenames of object files and about lack of generator expressions to modify strings: ${PKG_CONFIG_FILE_NAME_UNFINISHED} + ${TARGET_OBJECTS_FILE} -> ${PKG_CONFIG_FILE_NAME_FINISHED}"
			)
			add_custom_target("${PKGCONFIG_PATCH_TARGET_NAME}" ALL
				DEPENDS "${PKG_CONFIG_FILE_NAME_FINISHED}"
			)
			add_dependencies("${PKGCONFIG_PATCH_TARGET_NAME}" "${OBJECTS_FILE_RETRIEVAL_TARGET_NAME}" "${PKGCONFIG_DUMMY_UNFINISHED_GEN_TARGET_NAME}")

			set(PROPERLY_JOINED_TARGET_OBJECTS "@PROPERLY_JOINED_TARGET_OBJECTS@")
		else()
			set("RAW_TARGET_OBJECTS" "$<TARGET_OBJECTS:${TARGET}>")
			message(FATAL_ERROR "This branch is unimplemented because CMake lacked the needed functionality at the time")
			set(PROPERLY_JOINED_TARGET_OBJECTS "${RAW_TARGET_OBJECTS}")
		endif()
	endif()

	set(LINK_CURRENT_OBJECT_FLAG "$<IF:${IS_OBJECT},${PROPERLY_JOINED_TARGET_OBJECTS},>")
	
	list(APPEND libSpecific "$<IF:$<OR:$<BOOL:${PUBLIC_LIBS}>,${NEEDS_LIBS},${IS_OBJECT}>,Libs: -L\${libdir} ${LINK_CURRENT_LIB_FLAG} ${LINK_CURRENT_OBJECT_FLAG} $<IF:$<BOOL:${PUBLIC_LIBS}>,-l$<JOIN:$<REMOVE_DUPLICATES:${PUBLIC_LIBS}>, -l>,>,>\n$<IF:$<OR:$<BOOL:${PUBLIC_INCLUDES}>,$<BOOL:${PUBLIC_COMPILE_FLAGS}>>,Cflags: -I\${includedir} $<JOIN:$<REMOVE_DUPLICATES:${PUBLIC_COMPILE_FLAGS}>,>,>")


	list(JOIN header "\n" header)
	list(JOIN libSpecific "\n" libSpecific)
	set(libSpecific "${header}\n\n${libSpecific}")

	if(CONFIGURE_TIME_IS_OBJECT)
		file(GENERATE OUTPUT "${PKG_CONFIG_FILE_NAME_UNFINISHED}"
			CONTENT "${libSpecific}"
		)

		# Dummy target for generated files
		add_custom_command(
			OUTPUT "${PKG_CONFIG_FILE_NAME_UNFINISHED}"
			COMMENT "A dummy command to workaround cmake limitations"
		)
		add_custom_target("${PKGCONFIG_DUMMY_UNFINISHED_GEN_TARGET_NAME}"
			DEPENDS "${PKG_CONFIG_FILE_NAME_UNFINISHED}"
		)


		install(FILES "${PKG_CONFIG_FILE_NAME_FINISHED}"
			DESTINATION "${_INSTALL_LIB_DIR}/pkgconfig"
			COMPONENT "${_COMPONENT}"
		)
	else()
		file(GENERATE OUTPUT "${PKG_CONFIG_FILE_NAME}"
			CONTENT "${libSpecific}"
		)


		install(FILES "${PKG_CONFIG_FILE_NAME}"
			DESTINATION "${_INSTALL_LIB_DIR}/pkgconfig"
			COMPONENT "${_COMPONENT}"
		)
	endif()
endfunction()

