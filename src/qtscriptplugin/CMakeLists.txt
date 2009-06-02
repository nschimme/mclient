add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

# qtscriptplugin
set(qtscriptplugin_SRCS
    QtScriptPlugin.cpp
    ScriptEngine.cpp)

set(qtscriptplugin_MOC_HDRS
    QtScriptPlugin.h
    ScriptEngine.h)

QT4_WRAP_CPP(qtscriptplugin_MOC_SRCS ${qtscriptplugin_MOC_HDRS})

add_library(qtscriptplugin SHARED
    ${qtscriptplugin_SRCS}
    ${qtscriptplugin_MOC_SRCS})
target_link_libraries(qtscriptplugin mclientplugin ${QT_LIBRARIES})

install(TARGETS qtscriptplugin
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT plugins
)
