# aliasplugin

# Source Files
file(GLOB aliasplugin_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

# Header Files
file(GLOB aliasplugin_MOC_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")

# Resource Files
file(GLOB aliasplugin_RCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.qrc")

# UI Files
file(GLOB aliasplugin_UIS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
  
QT4_WRAP_CPP(aliasplugin_MOC_SRCS ${aliasplugin_MOC_HDRS})
QT4_WRAP_UI(aliasplugin_UIS_H ${aliasplugin_UIS})
QT4_ADD_RESOURCES(aliasplugin_RCC_SRCS ${aliasplugin_RCS})

add_library(aliasplugin SHARED
    ${aliasplugin_SRCS}
    ${aliasplugin_MOC_SRCS}
    ${aliasplugin_RCC_SRCS}
    ${aliasplugin_UIS_H}
)

target_link_libraries(
    aliasplugin
    mclientplugin
    ${QT_LIBRARIES}
)

INSTALL(TARGETS aliasplugin
  RUNTIME DESTINATION "${PLUGINS_DIR}"
  LIBRARY DESTINATION "${PLUGINS_DIR}"
  COMPONENT plugins
)
