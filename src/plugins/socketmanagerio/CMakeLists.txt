# socketmanagerio

# Source Files
file(GLOB socketmanagerio_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

# Header Files
file(GLOB socketmanagerio_MOC_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")

# Resource Files
file(GLOB socketmanagerio_RCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.qrc")

# UI Files
file(GLOB socketmanagerio_UIS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
  
QT4_WRAP_CPP(socketmanagerio_MOC_SRCS ${socketmanagerio_MOC_HDRS})
QT4_WRAP_UI(socketmanagerio_UIS_H ${socketmanagerio_UIS})
QT4_ADD_RESOURCES(socketmanagerio_RCC_SRCS ${socketmanagerio_RCS})

add_library(socketmanagerio SHARED
    ${socketmanagerio_SRCS}
    ${socketmanagerio_MOC_SRCS}
    ${socketmanagerio_RCC_SRCS}
    ${socketmanagerio_UIS_H}
)

target_link_libraries(
    socketmanagerio
    mclientplugin
    mainwindow
    ${QT_LIBRARIES}
)

INSTALL(TARGETS socketmanagerio
  RUNTIME DESTINATION "${PLUGINS_DIR}"
  LIBRARY DESTINATION "${PLUGINS_DIR}"
  COMPONENT plugins
)
