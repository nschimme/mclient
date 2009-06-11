# telnet

# Source Files
file(GLOB telnet_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

# Header Files
file(GLOB telnet_MOC_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")

# Resource Files
file(GLOB telnet_RCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.qrc")

# UI Files
file(GLOB telnet_UIS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
  
QT4_WRAP_CPP(telnet_MOC_SRCS ${telnet_MOC_HDRS})
QT4_WRAP_UI(telnet_UIS_H ${telnet_UIS})
QT4_ADD_RESOURCES(telnet_RCC_SRCS ${telnet_RCS})

add_library(telnet SHARED
    ${telnet_SRCS}
    ${telnet_MOC_SRCS}
    ${telnet_RCC_SRCS}
    ${telnet_UIS_H}
)

target_link_libraries(
    telnet
    mclientplugin
    ${QT_LIBRARIES}
)

INSTALL(TARGETS telnet
  RUNTIME DESTINATION "${PLUGINS_DIR}"
  LIBRARY DESTINATION "${PLUGINS_DIR}"
  COMPONENT plugins
)
