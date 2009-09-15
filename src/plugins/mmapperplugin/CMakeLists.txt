# mmapperplugin

if (NOT MINGW)
    find_package(ZLIB REQUIRED)
else (NOT MINGW)
    message(STATUS "Warning: MingW requires zlib to be installed manually")
endif (NOT MINGW)

# Source Files
file(GLOB_RECURSE mmapperplugin_SRCS
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     "*.cpp"
)

# Header Files
file(GLOB_RECURSE mmapperplugin_MOC_HDRS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "*.h"
)

# Resource Files
file(GLOB_RECURSE mmapperplugin_RCS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "*.qrc"
)

# UI Files
file(GLOB_RECURSE mmapperplugin_UIS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "*.ui"
)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/configuration
    ${CMAKE_CURRENT_SOURCE_DIR}/display
    ${CMAKE_CURRENT_SOURCE_DIR}/expandoracommon
    ${CMAKE_CURRENT_SOURCE_DIR}/global
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow
    ${CMAKE_CURRENT_SOURCE_DIR}/mapdata
    ${CMAKE_CURRENT_SOURCE_DIR}/mapfrontend
    ${CMAKE_CURRENT_SOURCE_DIR}/mapstorage
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty
    ${CMAKE_CURRENT_SOURCE_DIR}/pandoragroup
    ${CMAKE_CURRENT_SOURCE_DIR}/parser
    ${CMAKE_CURRENT_SOURCE_DIR}/pathmachine
    ${CMAKE_CURRENT_SOURCE_DIR}/preferences
    ${ZLIB_INCLUDE_DIR}
)

QT4_WRAP_CPP(mmapperplugin_MOC_SRCS ${mmapperplugin_MOC_HDRS})
QT4_WRAP_UI(mmapperplugin_UIS_H ${mmapperplugin_UIS})
QT4_ADD_RESOURCES(mmapperplugin_RCC_SRCS ${mmapperplugin_RCS})

add_library(mmapperplugin SHARED ${mmapperplugin_SRCS}
    ${mmapperplugin_MOC_SRCS} ${mmapperplugin_RCC_SRCS}
    ${mmapperplugin_UIS_H})
target_link_libraries(mmapperplugin
     mclientplugin
     mainwindow
     ${QT_LIBRARIES}
)

INSTALL(TARGETS mmapperplugin
  RUNTIME DESTINATION "${PLUGINS_DIR}"
  LIBRARY DESTINATION "${PLUGINS_DIR}"
  COMPONENT plugins
)
