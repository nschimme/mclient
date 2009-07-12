# mainwindow

# Source Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
set(mainwindow_SRCS ${files})

# Header Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")
set(mainwindow_MOC_HDRS ${files})

# UI Files
file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui")
set(mainwindow_UIS ${files})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
  
QT4_WRAP_CPP(mainwindow_MOC_SRCS ${mainwindow_MOC_HDRS})
QT4_WRAP_UI(mainwindow_UIS_H ${mainwindow_UIS})

add_library(mainwindow STATIC
    ${mainwindow_SRCS}
    ${mainwindow_MOC_SRCS}
    ${mainwindow_RCC_SRCS}
    ${mainwindow_UIS_H}
)

target_link_libraries(
    mainwindow
    mclientplugin
    ${QT_LIBRARIES}
)
