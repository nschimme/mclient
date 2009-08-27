# proxymanagerio

# Source Files
file(GLOB proxymanagerio_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

# Header Files
file(GLOB proxymanagerio_MOC_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")

# Resource Files
file(GLOB proxymanagerio_RCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.qrc")

# UI Files
file(GLOB proxymanagerio_UIS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
  
QT4_WRAP_CPP(proxymanagerio_MOC_SRCS ${proxymanagerio_MOC_HDRS})
QT4_WRAP_UI(proxymanagerio_UIS_H ${proxymanagerio_UIS})
QT4_ADD_RESOURCES(proxymanagerio_RCC_SRCS ${proxymanagerio_RCS})

add_library(proxymanagerio SHARED
    ${proxymanagerio_SRCS}
    ${proxymanagerio_MOC_SRCS}
    ${proxymanagerio_RCC_SRCS}
    ${proxymanagerio_UIS_H}
)

target_link_libraries(
    proxymanagerio
    mclientplugin
    ${QT_LIBRARIES}
)

INSTALL(TARGETS proxymanagerio
  RUNTIME DESTINATION "${PLUGINS_DIR}"
  LIBRARY DESTINATION "${PLUGINS_DIR}"
  COMPONENT plugins
)
