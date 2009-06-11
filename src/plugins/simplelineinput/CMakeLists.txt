# simplelineinput

# Source Files
file(GLOB simplelineinput_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

# Header Files
file(GLOB simplelineinput_MOC_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")

# Resource Files
file(GLOB simplelineinput_RCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.qrc")

# UI Files
file(GLOB simplelineinput_UIS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
  
QT4_WRAP_CPP(simplelineinput_MOC_SRCS ${simplelineinput_MOC_HDRS})
QT4_WRAP_UI(simplelineinput_UIS_H ${simplelineinput_UIS})
QT4_ADD_RESOURCES(simplelineinput_RCC_SRCS ${simplelineinput_RCS})

add_library(simplelineinput SHARED
    ${simplelineinput_SRCS}
    ${simplelineinput_MOC_SRCS}
    ${simplelineinput_RCC_SRCS}
    ${simplelineinput_UIS_H}
)

target_link_libraries(
    simplelineinput
    mclientplugin
    ${QT_LIBRARIES}
)

INSTALL(TARGETS simplelineinput
  RUNTIME DESTINATION "${PLUGINS_DIR}"
  LIBRARY DESTINATION "${PLUGINS_DIR}"
  COMPONENT plugins
)
