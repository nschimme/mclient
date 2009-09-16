# simpledisplay

# Source Files
file(GLOB simpledisplay_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

# Header Files
file(GLOB simpledisplay_MOC_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")

# Resource Files
file(GLOB simpledisplay_RCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.qrc")

# UI Files
file(GLOB simpledisplay_UIS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
  
QT4_WRAP_CPP(simpledisplay_MOC_SRCS ${simpledisplay_MOC_HDRS})
QT4_WRAP_UI(simpledisplay_UIS_H ${simpledisplay_UIS})
QT4_ADD_RESOURCES(simpledisplay_RCC_SRCS ${simpledisplay_RCS})

add_library(simpledisplay SHARED
    ${simpledisplay_SRCS}
    ${simpledisplay_MOC_SRCS}
    ${simpledisplay_RCC_SRCS}
    ${simpledisplay_UIS_H}
)

target_link_libraries(
    simpledisplay
    mclientplugin
    mainwindow
    ${QT_LIBRARIES}
)

INSTALL(TARGETS simpledisplay
  RUNTIME DESTINATION "${PLUGINS_DIR}"
  LIBRARY DESTINATION "${PLUGINS_DIR}"
  COMPONENT plugins
)
