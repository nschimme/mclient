add_definitions(-DQT_PLUGIN)

# mClient Plugins
set(PLUGINS_DIR "plugins")

add_subdirectory(socketmanagerio)
add_subdirectory(simplelineinput)
add_subdirectory(telnet)
add_subdirectory(mumexml)
add_subdirectory(webkitdisplay)
add_subdirectory(qtscriptplugin)
add_subdirectory(aliasplugin)
add_subdirectory(actionplugin)
add_subdirectory(mmapperplugin)
