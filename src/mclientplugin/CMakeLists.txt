INCLUDE_DIRECTORIES(${mclient_plugin_SOURCE_DIR}/ui ${CMAKE_CURRENT_SRC_DIR})

ADD_DEFINITIONS(${QT_DEFINITIONS})
#ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
add_definitions(-fPIC)

set(mclientplugin_SRCS
    PluginEntry.cpp
    PluginManager.cpp
    MClientEvent.cpp
    MClientEventData.cpp
    MClientPlugin.cpp
    MClientFilterPlugin.cpp
    MClientIOPlugin.cpp
    MClientDisplayPlugin.cpp
    PluginConfigWidget.cpp
    ConfigManager.cpp)

SET(mclientplugin_MOC_HDRS
    PluginManager.h
    MClientPlugin.h
    MClientFilterPlugin.h
    MClientIOPlugin.h
    MClientDisplayPlugin.h
    PluginConfigWidget.h
    ConfigManager.h
    MClientDefinitions.h)

QT4_WRAP_CPP(mclientplugin_MOC_SRCS ${mclientplugin_MOC_HDRS})

add_library(mclientplugin
    ${mclientplugin_SRCS}
    ${mclientplugin_MOC_SRCS})
target_link_libraries(mclientplugin ${QT_LIBRARIES})
