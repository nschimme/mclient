add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

# Source Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
set(mmapperplugin_SRCS ${mmapperplugin_SRCS} ${files})

# Header Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")
set(mmapperplugin_MOC_HDRS ${mmapperplugin_MOC_HDRS} ${files})

# Resource Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.qrc")
set(mmapperplugin_RCS ${mmapperplugin_RCS} ${files})

# UI Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")
set(mmapperplugin_UIS ${mmapperplugin_UIS} ${files})

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
    ${CMAKE_CURRENT_SOURCE_DIR}/pandoragroup
    ${CMAKE_CURRENT_SOURCE_DIR}/parser
    ${CMAKE_CURRENT_SOURCE_DIR}/pathmachine
    ${CMAKE_CURRENT_SOURCE_DIR}/preferences
#    ${CMAKE_CURRENT_SOURCE_DIR}/resources
#    ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons
#    ${CMAKE_CURRENT_SOURCE_DIR}/resources/pixmaps

)
  
QT4_WRAP_CPP(mmapperplugin_MOC_SRCS ${mmapperplugin_MOC_HDRS})
QT4_WRAP_UI(mmapperplugin_UIS_H ${mmapperplugin_UIS})
QT4_ADD_RESOURCES(mmapperplugin_RCC_SRCS ${mmapperplugin_RCS})

add_library(mmapperplugin SHARED ${mmapperplugin_SRCS}
    ${mmapperplugin_MOC_SRCS} ${mmapperplugin_RCC_SRCS}
    ${mmapperplugin_UIS_H})
target_link_libraries(mmapperplugin mclientplugin ${QT_LIBRARIES})

INSTALL(TARGETS mmapperplugin
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT plugins
)
