cmake_minimum_required(VERSION 3.22)

foreach(required_variable
  objectsFile
  pkgConfigFileUnfinished
  pkgConfigFileFinal
  objectsPlaceholder
)
  if(NOT DEFINED "${required_variable}")
    message(FATAL_ERROR "${required_variable} is required")
  endif()
endforeach()

file(READ "${objectsFile}" TARGET_OBJECTS)

set(PROPERLY_JOINED_TARGET_OBJECTS)
set(OBJECT_NAME_KEYS)
foreach(object_path IN LISTS TARGET_OBJECTS)
  get_filename_component(object_name "${object_path}" NAME)
  set(object_name_key "${object_name}")
  if(WIN32)
    string(TOLOWER "${object_name_key}" object_name_key)
  endif()
  if(object_name_key IN_LIST OBJECT_NAME_KEYS)
    message(FATAL_ERROR
      "Object files for pkg-config must have unique basenames: ${object_name}"
    )
  endif()
  list(APPEND OBJECT_NAME_KEYS "${object_name_key}")
  string(REPLACE "\\" "\\\\" object_name "${object_name}")
  string(REPLACE " " "\\ " object_name "${object_name}")
  list(APPEND PROPERLY_JOINED_TARGET_OBJECTS "\${libdir}/${object_name}")
endforeach()
list(JOIN PROPERLY_JOINED_TARGET_OBJECTS " " PROPERLY_JOINED_TARGET_OBJECTS)

file(READ "${pkgConfigFileUnfinished}" PKG_CONFIG_CONTENT)
string(FIND "${PKG_CONFIG_CONTENT}" "${objectsPlaceholder}" OBJECTS_PLACEHOLDER_INDEX)
if(OBJECTS_PLACEHOLDER_INDEX EQUAL -1)
  message(FATAL_ERROR
    "Object placeholder is missing from the unfinished pkg-config file"
  )
endif()
string(REPLACE
  "${objectsPlaceholder}"
  "${PROPERLY_JOINED_TARGET_OBJECTS}"
  PKG_CONFIG_CONTENT
  "${PKG_CONFIG_CONTENT}"
)
file(WRITE "${pkgConfigFileFinal}" "${PKG_CONFIG_CONTENT}")
