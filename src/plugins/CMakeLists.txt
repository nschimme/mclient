add_definitions(-DQT_PLUGIN)

# mClient Plugins
set(PLUGINS_DIR "plugins")


# Core Plugins
add_subdirectory(socketmanagerio)
add_subdirectory(telnet)
add_subdirectory(mumexml)


# Feature Plugins
add_subdirectory(mmapperplugin)
add_subdirectory(proxymanagerio)
add_subdirectory(remoteedit)
add_subdirectory(qtscriptplugin)

# Display-Input Plugins
add_subdirectory(multilineinput)
#add_subdirectory(simplelineinput)


# Display-Output Plugins
#add_subdirectory(webkitdisplay)
add_subdirectory(simpledisplay)

# Obsolete Plugins
#add_subdirectory(aliasplugin)
#add_subdirectory(actionplugin)
