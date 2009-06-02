include_directories(${mclient_plugin_SOURCE_DIR}/src)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

# aliasplugin
set(aliasplugin_SRCS
    AliasPlugin.cpp)

set(aliasplugin_MOC_HDRS
    AliasPlugin.h)

QT4_WRAP_CPP(aliasplugin_MOC_SRCS ${aliasplugin_MOC_HDRS})

add_library(aliasplugin SHARED
    ${aliasplugin_SRCS}
    ${aliasplugin_MOC_SRCS})
target_link_libraries(aliasplugin mclientplugin ${QT_LIBRARIES})

install(TARGETS aliasplugin
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT plugins
)
