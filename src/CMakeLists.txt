#add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

# mClient

# Source files
set(mclient_SRCS main.cpp)
# foreach(dir commandmanager mainwindow)
#   file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${dir}/*.cpp")
#   set(mclient_SRCS ${mclient_SRCS} ${files})
# endforeach()

# # Header Files
# foreach(dir commandmanager mainwindow)
#   file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${dir}/*.h")
#   set(mclient_MOC_HDRS ${files})
# endforeach()

# # UI Files
# foreach(dir commandmanager mainwindow)
#   file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${dir}/*.ui")
#   set(mclient_UIS ${files})
# endforeach()

# Library Resource Files
file(GLOB_RECURSE mclient_RCS
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     "resources/*.qrc"
)

# Add Windows Resource File
if (MINGW)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/mclient_rc.o
        COMMAND windres.exe -I ${CMAKE_CURRENT_SOURCE_DIR}
        -i ${CMAKE_CURRENT_SOURCE_DIR}/resources/win32/mclient.rc
        -o ${CMAKE_CURRENT_BINARY_DIR}/mclient_rc.o
    )
    set(mclient_WINRC ${CMAKE_CURRENT_BINARY_DIR}/mclient_rc.o)
else (MINGW)
    set(mclient_WINRC resources/win32/mclient.rc)
endif (MINGW)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    mclientplugin
    mainwindow
    commandmanager
)
  
QT4_WRAP_UI(mclient_UIS_H ${mclient_UIS})
QT4_WRAP_CPP(mclient_MOC_SRCS ${mclient_MOC_HDRS})
QT4_ADD_RESOURCES(mclient_RCC_SRCS ${mclient_RCS})

add_executable(mclient #WIN32
    ${mclient_SRCS}
    ${mclient_MOC_SRCS}
    ${mclient_UIS_H}
    ${mclient_RCC_SRCS}
    ${mclient_WINRC}
)

target_link_libraries(mclient
    ${QT_QTMAIN_LIBRARY}
    ${QT_LIBRARIES}
    mclientplugin
    mainwindow
    commandmanager
)

# mClient's Core Engine
add_subdirectory(mclientplugin)
add_subdirectory(mainwindow)
add_subdirectory(commandmanager)

# Various Plugins
add_subdirectory(plugins)

# Installation Location
install(TARGETS mclient
        RUNTIME DESTINATION .
        COMPONENT applications)

# Windows Install Settings
if (WIN32)
    # Install Library Files
    # Note: this is not necessary if Qt is compiled statically
    message(STATUS "Qt libraries are at ${QT_LIBRARY_DIR}")
    install(FILES
        "${QT_LIBRARY_DIR}/QtCore${QT_VERSION_MAJOR}.dll"
        "${QT_LIBRARY_DIR}/QtGui${QT_VERSION_MAJOR}.dll"
        "${QT_LIBRARY_DIR}/QtNetwork${QT_VERSION_MAJOR}.dll"
        "${QT_LIBRARY_DIR}/QtXml${QT_VERSION_MAJOR}.dll"
        "${QT_LIBRARY_DIR}/QtOpenGL${QT_VERSION_MAJOR}.dll"
        "${QT_LIBRARY_DIR}/QtWebKit${QT_VERSION_MAJOR}.dll"
        DESTINATION .
	COMPONENT libraries
    )
    # Debug Libraries
    install(FILES
        "${QT_LIBRARY_DIR}/QtCored${QT_VERSION_MAJOR}.dll"
        "${QT_LIBRARY_DIR}/QtGuid${QT_VERSION_MAJOR}.dll"
        "${QT_LIBRARY_DIR}/QtNetworkd${QT_VERSION_MAJOR}.dll"
        "${QT_LIBRARY_DIR}/QtXmld${QT_VERSION_MAJOR}.dll"
        "${QT_LIBRARY_DIR}/QtOpenGLd${QT_VERSION_MAJOR}.dll"
        "${QT_LIBRARY_DIR}/QtWebKitd${QT_VERSION_MAJOR}.dll"
        DESTINATION .
	COMPONENT libraries
    )
    # MingW Library
    if (MINGW)
	string(REGEX REPLACE "[\\|/][^\\/]+exe$" ""
	       MINGW_LIBRARY_DIR
	       ${CMAKE_CXX_COMPILER}
        )
        message(STATUS "MingW library is at ${MINGW_LIBRARY_DIR}")

        install(FILES 
            "${MINGW_LIBRARY_DIR}/mingwm10.dll"
	    DESTINATION .
	    COMPONENT libraries
	)
    endif (MINGW)
endif (WIN32)

# Installer
set(CPACK_RESOURCE_FILE_LICENSE )
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Nils Schimmelmann")
set(CPACK_PACKAGE_CONTACT "nschimme@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A mud client especially written for the mud MUME")
if (SVN_BUILD)
    set(CPACK_PACKAGE_VERSION "svn-build-${SVN_BUILD}")
else (SVN_BUILD)
    set(CPACK_PACKAGE_VERSION "${MCLIENT_VERSION}")
endif (SVN_BUILD)
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "mClient")

# Components:
set(CPACK_STRIP_FILES TRUE)
# Applications Component
set(CPACK_COMPONENTS_ALL applications plugins libraries)
set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "mClient")
set(CPACK_COMPONENT_APPLICATIONS_DESCRIPTION "A mud client especially written for the mud MUME")
set(CPACK_COMPONENT_APPLICATIONS_REQUIRED TRUE)

# Plugins Component
set(CPACK_COMPONENT_PLUGINS_DISPLAY_NAME "Plugins")
set(CPACK_COMPONENT_PLUGINS_DESCRIPTION "Plugins for mClient to function")
set(CPACK_COMPONENT_PLUGINS_REQUIRED TRUE)

# Libraries Component
set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Runtime Libraries")
set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION "Qt/MingW libraries for running mClient")


# Source Package
set(CPACK_SOURCE_IGNORE_FILES
    "~$"
    "/\\\\.svn/"
    "/build/"
    "/winbuild/"
)
set(CPACK_SOURCE_PACKAGE_FILE_NAME
    "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-source"
)
set(CPACK_PACKAGE_FILE_NAME
    "${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}"
)
include(CPack)
