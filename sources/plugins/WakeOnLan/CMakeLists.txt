# include common plugin definitions
include("../YadomsPluginsCommon.cmake")

#########################################
## Place here all the plugin source files
#########################################
set(WAKEONLAN_SOURCES
  stdafx.h
  stdafx.cpp
  WakeOnLan.h
  WakeOnLan.cpp
)
#ADD_SUBDIRECTORY(awake)  

#########################################
## Place here all additional include directories
## (by default yadoms-shared and boost are included)
#########################################
set(WAKEONLAN_ADDITIONAL_INC_DIR
       ${AWAKE_INCLUDE_DIRECTORY})
set(WAKEONLAN_INCLDIR
	${CMAKE_CURRENT_SOURCE_DIR})

#########################################
## Place here all additional libraries to link with
## (by default yadoms-shared and boost are included)
#########################################

#########################################
## Place here all additional compiler flags
## i.e. : for raspberry to link with wiringPi : use add_definition("-lwiringPi")
#########################################

#########################################
## Always leave these lines to configure plugin correctly
## even if FREEMOBILESMS_INCLDIR or FREEMOBILESMS_LINK are empty
#########################################
PLUGIN_SOURCES(WakeOnLan ${WAKEONLAN_SOURCES})
PLUGIN_INCLDIR(WakeOnLan ${WAKEONLAN_INCLDIR} ${WAKEONLAN_ADDITIONAL_INC_DIR})
PLUGIN_LINK(WakeOnLan ${WAKEONLAN_LINK})

#########################################
## Post-build commands
## Copy files to build directory
## "package.json" and "icon.png" are needed, but you can add extra-files
#########################################
PLUGIN_POST_BUILD_COPY_FILE(WakeOnLan package.json)
PLUGIN_POST_BUILD_COPY_FILE(WakeOnLan icon.png)
PLUGIN_POST_BUILD_COPY_DIRECTORY(WakeOnLan locales)
