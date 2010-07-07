# clientcore

# Source Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
set(clientcore_SRCS ${files})

# Header Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")
set(clientcore_MOC_HDRS ${files})

# UI Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")
set(clientcore_UIS ${files})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    command
    display
)

add_subdirectory(command)
add_subdirectory(display)
  
QT4_WRAP_CPP(clientcore_MOC_SRCS ${clientcore_MOC_HDRS})
QT4_WRAP_UI(clientcore_UIS_H ${clientcore_UIS})

add_library(clientcore STATIC
    ${clientcore_SRCS}
    ${clientcore_MOC_SRCS}
    ${clientcore_RCC_SRCS}
    ${clientcore_UIS_H}
)

target_link_libraries(
    clientcore
    commandprocessor
    mainwindow
    ${QT_LIBRARIES}
)

