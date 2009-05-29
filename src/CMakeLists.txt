add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

# Core Source Files
foreach(dir ${CMAKE_CURRENT_SOURCE_DIR})
    file(GLOB files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${dir}/*.cpp")
    set(mclient_SRCS ${mclient_SRCS} ${files})
endforeach()

# Core Header Files
foreach(dir ${CMAKE_CURRENT_SOURCE_DIR})
    file(GLOB files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${dir}/*.h")
    set(mclient_MOC_HDRS ${mclient_MOC_HDRS} ${files})
endforeach()

# Resource Files
foreach(dir mainwindow mclientplugin)
    file(GLOB files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${dir}/*.qrc")
    set(mclient_RCS ${mclient_RCS} ${files})
endforeach()

# UI Files
foreach(dir mainwindow mclientplugin)
    file(GLOB files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${dir}/*.ui")
    set(mclient_UIS ${mclient_UIS} ${files})
endforeach()

# Add Windows Resource File
if (MINGW)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/mclient_rc.o
                       COMMAND windres.exe -I ${CMAKE_CURRENT_SOURCE_DIR}
                                           -i ${CMAKE_CURRENT_SOURCE_DIR}/client.rc
                                           -o ${CMAKE_CURRENT_BINARY_DIR}/mclient_rc.o)
    set(mmapper_WINRC ${CMAKE_CURRENT_BINARY_DIR}/mclient_rc.o)
else (MINGW)
    set(mmapper_WINRC mclient.rc)
endif (MINGW)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    mainwindow
    mclientplugin
    commandmanager
)
  
QT4_WRAP_UI(mclient_UIS_H ${mclient_UIS})
QT4_WRAP_CPP(mclient_MOC_SRCS ${mclient_MOC_HDRS})
QT4_ADD_RESOURCES(mclient_RCC_SRCS ${mclient_RCS})

add_executable(mclient
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

# Installation Location
install(TARGETS mclient
        RUNTIME DESTINATION bin
        COMPONENT applications)

# mClient's Core Engine
add_subdirectory(mclientplugin)
add_subdirectory(mainwindow)
add_subdirectory(commandmanager)

# Various Dynamic Plugins
add_subdirectory(socketmanagerio)
add_subdirectory(simplelineinput)
add_subdirectory(telnet)
add_subdirectory(mumexml)
add_subdirectory(webkitdisplay)
#add_subdirectory(simpletestdisplay)

# Installer
set(CPACK_RESOURCE_FILE_LICENSE )
set(CPACK_PACKAGE_ICON "mainwindow/resources/m.png")
set(CPACK_PACKAGE_NAME "mClient")
set(CPACK_PACKAGE_VENDOR "Nils Schimmelmann")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "mClient Installer")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "mClient Installer")
include(CPack)
cpack_add_component(applications
  DISPLAY_NAME "mClient"
  DESCRIPTION "A mud client especially written for the mud MUME"
  REQUIRED
)
cpack_add_component(plugins
  DISPLAY_NAME "Plugins"
  DESCRIPTION "Plugins for mClient to function"
)
