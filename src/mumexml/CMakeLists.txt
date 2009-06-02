add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)


# mumexml
set(mumexml_SRCS
    MumeXML.cpp)

set(mumexml_MOC_HDRS
    MumeXML.h)

QT4_WRAP_CPP(mumexml_MOC_SRCS ${mumexml_MOC_HDRS})
add_library(mumexml SHARED ${mumexml_SRCS}
    ${mumexml_MOC_SRCS})
target_link_libraries(mumexml mclientplugin ${QT_LIBRARIES})

INSTALL(TARGETS mumexml
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT plugins
)
