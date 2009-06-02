add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)


# telnet
set(telnet_SRCS
    Telnet.cpp)

set(telnet_MOC_HDRS
    Telnet.h)

QT4_WRAP_CPP(telnet_MOC_SRCS ${telnet_MOC_HDRS})
add_library(telnet SHARED ${telnet_SRCS}
    ${telnet_MOC_SRCS})
target_link_libraries(telnet mclientplugin ${QT_LIBRARIES})

INSTALL(TARGETS telnet
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT plugins
)
