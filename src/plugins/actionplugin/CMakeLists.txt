# actionplugin

# Source Files
file(GLOB actionplugin_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

# Header Files
file(GLOB actionplugin_MOC_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")

# Resource Files
file(GLOB actionplugin_RCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.qrc")

# UI Files
file(GLOB actionplugin_UIS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
  
QT4_WRAP_CPP(actionplugin_MOC_SRCS ${actionplugin_MOC_HDRS})
QT4_WRAP_UI(actionplugin_UIS_H ${actionplugin_UIS})
QT4_ADD_RESOURCES(actionplugin_RCC_SRCS ${actionplugin_RCS})

add_library(actionplugin SHARED
    ${actionplugin_SRCS}
    ${actionplugin_MOC_SRCS}
    ${actionplugin_RCC_SRCS}
    ${actionplugin_UIS_H}
)

target_link_libraries(
    actionplugin
    mclientplugin
    ${QT_LIBRARIES}
)

INSTALL(TARGETS actionplugin
  RUNTIME DESTINATION "${PLUGINS_DIR}"
  LIBRARY DESTINATION "${PLUGINS_DIR}"
  COMPONENT plugins
)
