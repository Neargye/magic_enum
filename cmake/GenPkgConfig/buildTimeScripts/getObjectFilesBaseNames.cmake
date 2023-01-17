 
message(STATUS "objectsFile  ${objectsFile}")
message(STATUS "pkgConfigFileFinal  ${pkgConfigFileFinal}")
message(STATUS "pkgConfigFileUnlinished  ${pkgConfigFileUnlinished}")

file(READ "${objectsFile}" TARGET_OBJECTS)

set(PROPERLY_JOINED_TARGET_OBJECTS "")

foreach(objFullPath ${TARGET_OBJECTS})
	get_filename_component(objFullPath "${objFullPath}" NAME)
	list(APPEND PROPERLY_JOINED_TARGET_OBJECTS "${objFullPath}")
endforeach()
list(JOIN PROPERLY_JOINED_TARGET_OBJECTS " " PROPERLY_JOINED_TARGET_OBJECTS)

message(STATUS "PROPERLY_JOINED_TARGET_OBJECTS AFTER ${PROPERLY_JOINED_TARGET_OBJECTS}")

configure_file("${pkgConfigFileUnlinished}" "${pkgConfigFileFinal}" @ONLY)
