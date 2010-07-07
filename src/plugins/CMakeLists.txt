add_definitions(-DQT_PLUGIN)

# mClient Plugins
set(PLUGINS_DIR "${MCLIENT_PLUGINS_DIR}")


# Core Plugins
add_subdirectory(socketmanagerio)
add_subdirectory(telnet)
add_subdirectory(mumexml)


# Feature Plugins
add_subdirectory(mmapperplugin)
add_subdirectory(proxymanagerio)
add_subdirectory(remoteedit)
add_subdirectory(qtscriptplugin)
add_subdirectory(tellmessenger)

# Display-Input Plugins
add_subdirectory(multilineinput)
#add_subdirectory(simplelineinput)


# Display-Output Plugins
add_subdirectory(webkitdisplay)
#add_subdirectory(simpledisplay)
