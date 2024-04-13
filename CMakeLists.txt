cmake_minimum_required(VERSION 2.8)

# include the project setting file
include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

# specify project name
set(PROJECT_NAME VisualizerNode)

# set SOURCE_FILES
set(SOURCE_FILES
   VisualizerNode.h VisualizerNode.cpp
   VisualizerNodeDrawOverride.h VisualizerNodeDrawOverride.cpp
   VisualizerDrawDataNode.h VisualizerDrawDataNode.cpp
   VisualizeCmd.h VisualizeCmd.cpp
   pluginEntry.cpp
)

# set linking libraries
set(LIBRARIES
     OpenMaya
     OpenMayaRender
     OpenMayaUI
     Foundation
)

find_opengl()

# Build plugin
build_plugin()

