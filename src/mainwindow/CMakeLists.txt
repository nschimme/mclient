add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)
add_definitions(-fPIC)

# mainwindow
set(mainwindow_SRCS
    MainWindow.cpp
    ActionManager.cpp)

set(mainwindow_MOC_HDRS
    MainWindow.h
    ActionManager.h)

QT4_WRAP_CPP(mainwindow_MOC_SRCS ${mainwindow_MOC_HDRS})
add_library(mainwindow ${mainwindow_SRCS}
    ${mainwindow_MOC_SRCS})
target_link_libraries(mainwindow ${QT_LIBRARIES})
