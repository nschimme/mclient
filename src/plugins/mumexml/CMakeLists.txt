# mumexml

# Source Files
file(GLOB mumexml_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

# Header Files
file(GLOB mumexml_MOC_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")

# Resource Files
file(GLOB mumexml_RCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.qrc")

# UI Files
file(GLOB mumexml_UIS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
  
QT4_WRAP_CPP(mumexml_MOC_SRCS ${mumexml_MOC_HDRS})
QT4_WRAP_UI(mumexml_UIS_H ${mumexml_UIS})
QT4_ADD_RESOURCES(mumexml_RCC_SRCS ${mumexml_RCS})

add_library(mumexml SHARED
    ${mumexml_SRCS}
    ${mumexml_MOC_SRCS}
    ${mumexml_RCC_SRCS}
    ${mumexml_UIS_H}
)

target_link_libraries(
    mumexml
    mclientplugin
    ${QT_LIBRARIES}
)

INSTALL(TARGETS mumexml
  RUNTIME DESTINATION "${PLUGINS_DIR}"
  LIBRARY DESTINATION "${PLUGINS_DIR}"
  COMPONENT plugins
)
