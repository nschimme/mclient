# remoteedit

# Source Files
file(GLOB remoteedit_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

# Header Files
file(GLOB remoteedit_MOC_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")

# Resource Files
file(GLOB remoteedit_RCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.qrc")

# UI Files
file(GLOB remoteedit_UIS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
  
QT4_WRAP_CPP(remoteedit_MOC_SRCS ${remoteedit_MOC_HDRS})
QT4_WRAP_UI(remoteedit_UIS_H ${remoteedit_UIS})
QT4_ADD_RESOURCES(remoteedit_RCC_SRCS ${remoteedit_RCS})

add_library(remoteedit SHARED
    ${remoteedit_SRCS}
    ${remoteedit_MOC_SRCS}
    ${remoteedit_RCC_SRCS}
    ${remoteedit_UIS_H}
)

target_link_libraries(
    remoteedit
    mclientplugin
    ${QT_LIBRARIES}
)

INSTALL(TARGETS remoteedit
  RUNTIME DESTINATION "${PLUGINS_DIR}"
  LIBRARY DESTINATION "${PLUGINS_DIR}"
  COMPONENT plugins
)
