# multilineinput

# Source Files
file(GLOB multilineinput_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

# Header Files
file(GLOB multilineinput_MOC_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")

# Resource Files
file(GLOB multilineinput_RCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.qrc")

# UI Files
file(GLOB multilineinput_UIS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
  
QT4_WRAP_CPP(multilineinput_MOC_SRCS ${multilineinput_MOC_HDRS})
QT4_WRAP_UI(multilineinput_UIS_H ${multilineinput_UIS})
QT4_ADD_RESOURCES(multilineinput_RCC_SRCS ${multilineinput_RCS})

add_library(multilineinput SHARED
    ${multilineinput_SRCS}
    ${multilineinput_MOC_SRCS}
    ${multilineinput_RCC_SRCS}
    ${multilineinput_UIS_H}
)

target_link_libraries(
    multilineinput
    mclientplugin
    mainwindow
    ${QT_LIBRARIES}
)

INSTALL(TARGETS multilineinput
  RUNTIME DESTINATION "${PLUGINS_DIR}"
  LIBRARY DESTINATION "${PLUGINS_DIR}"
  COMPONENT plugins
)
