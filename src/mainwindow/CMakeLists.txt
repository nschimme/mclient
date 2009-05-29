INCLUDE_DIRECTORIES(${mainwindow_SOURCE_DIR}/ui ${CMAKE_CURRENT_SRC_DIR})

ADD_DEFINITIONS(${QT_DEFINITIONS})
#ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
add_definitions(-fPIC)

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


#install(TARGETS 
#    mclient_plugin 
#    ARCHIVE DESTINATION lib)
