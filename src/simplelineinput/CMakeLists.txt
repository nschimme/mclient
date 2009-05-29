include_directories(${mclient_plugin_SOURCE_DIR}/src)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

# simplelineinput
set(simplelineinput_SRCS
    SimpleLineInput.cpp
    InputWidget.cpp)

set(simplelineinput_MOC_HDRS
    SimpleLineInput.h
    InputWidget.h)

QT4_WRAP_CPP(simplelineinput_MOC_SRCS ${simplelineinput_MOC_HDRS})

add_library(simplelineinput SHARED
    ${simplelineinput_SRCS}
    ${simplelineinput_MOC_SRCS})
target_link_libraries(simplelineinput mclientplugin ${QT_LIBRARIES})

install(TARGETS simplelineinput
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT plugins
)
