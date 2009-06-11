# qtscriptplugin

# Source Files
file(GLOB qtscriptplugin_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

# Header Files
file(GLOB qtscriptplugin_MOC_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")

# Resource Files
file(GLOB qtscriptplugin_RCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.qrc")

# UI Files
file(GLOB qtscriptplugin_UIS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
  
QT4_WRAP_CPP(qtscriptplugin_MOC_SRCS ${qtscriptplugin_MOC_HDRS})
QT4_WRAP_UI(qtscriptplugin_UIS_H ${qtscriptplugin_UIS})
QT4_ADD_RESOURCES(qtscriptplugin_RCC_SRCS ${qtscriptplugin_RCS})

add_library(qtscriptplugin SHARED
    ${qtscriptplugin_SRCS}
    ${qtscriptplugin_MOC_SRCS}
    ${qtscriptplugin_RCC_SRCS}
    ${qtscriptplugin_UIS_H}
)

target_link_libraries(
    qtscriptplugin
    mclientplugin
    ${QT_LIBRARIES}
)

INSTALL(TARGETS qtscriptplugin
  RUNTIME DESTINATION "${PLUGINS_DIR}"
  LIBRARY DESTINATION "${PLUGINS_DIR}"
  COMPONENT plugins
)
