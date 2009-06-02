add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)


# webkitdisplay
set(webkitdisplay_SRCS
    WebKitDisplay.cpp
    DisplayWidget.cpp)

set(webkitdisplay_MOC_HDRS
    WebKitDisplay.h
    DisplayWidget.h)

set(webkitdisplay_RESOURCE
    WebKitDisplay.qrc)

QT4_ADD_RESOURCES(webkitdisplay_RCC_SRCS ${webkitdisplay_RESOURCE})
QT4_WRAP_CPP(webkitdisplay_MOC_SRCS ${webkitdisplay_MOC_HDRS})
add_library(webkitdisplay SHARED ${webkitdisplay_SRCS}
    ${webkitdisplay_MOC_SRCS} ${webkitdisplay_RCC_SRCS})
target_link_libraries(webkitdisplay mclientplugin ${QT_LIBRARIES})

INSTALL(TARGETS webkitdisplay
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT plugins
)
