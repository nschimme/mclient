# commandprocessor

# Source Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
set(commandprocessor_SRCS ${files})

# Header Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")
set(commandprocessor_MOC_HDRS ${files})

# UI Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")
set(commandprocessor_UIS ${files})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
  
QT4_WRAP_CPP(commandprocessor_MOC_SRCS ${commandprocessor_MOC_HDRS})
QT4_WRAP_UI(commandprocessor_UIS_H ${commandprocessor_UIS})

add_library(commandprocessor STATIC
    ${commandprocessor_SRCS}
    ${commandprocessor_MOC_SRCS}
    ${commandprocessor_RCC_SRCS}
    ${commandprocessor_UIS_H}
)

target_link_libraries(
    commandprocessor
    mclientplugin
    ${QT_LIBRARIES}
)
