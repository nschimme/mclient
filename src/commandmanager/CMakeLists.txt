add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)
add_definitions(-fPIC)

# commandmanager
set(commandmanager_SRCS
    CommandManager.cpp)

set(commandmanager_MOC_HDRS
    CommandManager.h)

QT4_WRAP_CPP(commandmanager_MOC_SRCS ${commandmanager_MOC_HDRS})
add_library(commandmanager ${commandmanager_SRCS}
     ${commandmanager_MOC_SRCS})
target_link_libraries(commandmanager ${QT_LIBRARIES})
