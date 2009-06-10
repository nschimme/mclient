# commandmanager

# Source Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
set(commandmanager_SRCS ${files})

# Header Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")
set(commandmanager_MOC_HDRS ${files})

# UI Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")
set(commandmanager_UIS ${files})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
  
QT4_WRAP_CPP(commandmanager_MOC_SRCS ${commandmanager_MOC_HDRS})
QT4_WRAP_UI(commandmanager_UIS_H ${commandmanager_UIS})

add_library(commandmanager STATIC
    ${commandmanager_SRCS}
    ${commandmanager_MOC_SRCS}
    ${commandmanager_RCC_SRCS}
    ${commandmanager_UIS_H}
)

target_link_libraries(
    commandmanager
    mclientplugin
    mainwindow
    ${QT_LIBRARIES}
)
