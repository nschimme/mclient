# webkitdisplay

# Source Files
file(GLOB webkitdisplay_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

# Header Files
file(GLOB webkitdisplay_MOC_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")

# Resource Files
file(GLOB webkitdisplay_RCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.qrc")

# UI Files
file(GLOB webkitdisplay_UIS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
  
QT5_WRAP_CPP(webkitdisplay_MOC_SRCS ${webkitdisplay_MOC_HDRS})
QT5_WRAP_UI(webkitdisplay_UIS_H ${webkitdisplay_UIS})
QT5_ADD_RESOURCES(webkitdisplay_RCC_SRCS ${webkitdisplay_RCS})

add_library(webkitdisplay SHARED
    ${webkitdisplay_SRCS}
    ${webkitdisplay_MOC_SRCS}
    ${webkitdisplay_RCC_SRCS}
    ${webkitdisplay_UIS_H}
)

target_link_libraries(
    webkitdisplay
    mclientplugin
    Qt5::WebKitWidgets
)

INSTALL(TARGETS webkitdisplay
  RUNTIME DESTINATION "${PLUGINS_DIR}"
  LIBRARY DESTINATION "${PLUGINS_DIR}"
  COMPONENT plugins
)
