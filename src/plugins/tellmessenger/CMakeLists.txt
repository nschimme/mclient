# tellmessenger

# Source Files
file(GLOB tellmessenger_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

# Header Files
file(GLOB tellmessenger_MOC_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")

# Resource Files
file(GLOB tellmessenger_RCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.qrc")

# UI Files
file(GLOB tellmessenger_UIS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
  
QT4_WRAP_CPP(tellmessenger_MOC_SRCS ${tellmessenger_MOC_HDRS})
QT4_WRAP_UI(tellmessenger_UIS_H ${tellmessenger_UIS})
QT4_ADD_RESOURCES(tellmessenger_RCC_SRCS ${tellmessenger_RCS})

add_library(tellmessenger SHARED
    ${tellmessenger_SRCS}
    ${tellmessenger_MOC_SRCS}
    ${tellmessenger_RCC_SRCS}
    ${tellmessenger_UIS_H}
)

target_link_libraries(
    tellmessenger
    mclientplugin
    mainwindow
    ${QT_LIBRARIES}
)

INSTALL(TARGETS tellmessenger
  RUNTIME DESTINATION "${PLUGINS_DIR}"
  LIBRARY DESTINATION "${PLUGINS_DIR}"
  COMPONENT plugins
)
