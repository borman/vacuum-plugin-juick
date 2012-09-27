if (NOT VACUUM_SDK_PATH)
	if (APPLE)
		set(VACUUM_SDK_PATH "${CMAKE_INSTALL_PREFIX}/vacuum.app/Contents/Resources/include")
	elseif (HAIKU)
		set(VACUUM_SDK_PATH "${CMAKE_INSTALL_PREFIX}/Vacuum/sdk")
	elseif (UNIX)
		set(VACUUM_SDK_PATH "${CMAKE_INSTALL_PREFIX}/include/vacuum")
	elseif (WIN32)
		set(VACUUM_SDK_PATH "${CMAKE_INSTALL_PREFIX}/sdk")
	endif (APPLE)
endif (NOT VACUUM_SDK_PATH)

if (NOT VACUUM_LIB_PATH)
	if (APPLE)
		set(VACUUM_LIB_PATH "${CMAKE_INSTALL_PREFIX}/vacuum.app/Contents/Frameworks")
	elseif (HAIKU)
		set(VACUUM_LIB_PATH "${CMAKE_INSTALL_PREFIX}/Vacuum")
	elseif (UNIX)
		set(VACUUM_LIB_PATH "${CMAKE_INSTALL_PREFIX}/lib")
	elseif (WIN32)
		set(VACUUM_LIB_PATH "${CMAKE_INSTALL_PREFIX}")
	endif (APPLE)
endif (NOT VACUUM_LIB_PATH) 

include("${VACUUM_SDK_PATH}/make/config.cmake")