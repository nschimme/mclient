# mclientplugin

# Source Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
set(mclientplugin_SRCS ${files})

# Header Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")
set(mclientplugin_MOC_HDRS ${files})

# UI Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")
set(mclientplugin_UIS ${files})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
  
QT4_WRAP_CPP(mclientplugin_MOC_SRCS ${mclientplugin_MOC_HDRS})
QT4_WRAP_UI(mclientplugin_UIS_H ${mclientplugin_UIS})

add_library(mclientplugin STATIC
    ${mclientplugin_SRCS}
    ${mclientplugin_MOC_SRCS}
    ${mclientplugin_RCC_SRCS}
    ${mclientplugin_UIS_H}
)

target_link_libraries(
    mclientplugin
    commandprocessor
    ${QT_LIBRARIES}
)

