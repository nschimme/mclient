include_directories(${mclient_plugin_SOURCE_DIR}/src)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

# actionplugin
set(actionplugin_SRCS
    ActionPlugin.cpp)

set(actionplugin_MOC_HDRS
    ActionPlugin.h)

QT4_WRAP_CPP(actionplugin_MOC_SRCS ${actionplugin_MOC_HDRS})

add_library(actionplugin SHARED
    ${actionplugin_SRCS}
    ${actionplugin_MOC_SRCS})
target_link_libraries(actionplugin mclientplugin ${QT_LIBRARIES})

install(TARGETS actionplugin
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT plugins
)
