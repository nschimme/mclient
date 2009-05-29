add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

# socketmanagerio
set(socketmanagerio_SRCS
    SocketManagerIO.cpp
    SocketReader.cpp
    SocketManagerIOConfig.cpp)

set(socketmanagerio_MOC_HDRS
    SocketManagerIO.h
    SocketReader.h
    SocketManagerIOConfig.h)

QT4_WRAP_CPP(socketmanagerio_MOC_SRCS ${socketmanagerio_MOC_HDRS})
add_library(socketmanagerio SHARED ${socketmanagerio_SRCS}
    ${socketmanagerio_MOC_SRCS})
target_link_libraries(socketmanagerio mclientplugin ${QT_LIBRARIES})

INSTALL(TARGETS socketmanagerio
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT plugins
)
