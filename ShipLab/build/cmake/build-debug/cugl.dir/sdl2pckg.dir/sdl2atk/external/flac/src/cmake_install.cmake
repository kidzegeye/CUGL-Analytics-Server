# Install script for directory: /Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/sdl2atk/external/flac/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/shiplab/build/cmake/build-debug/cugl.dir/sdl2pckg.dir/sdl2atk/external/flac/src/libFLAC/cmake_install.cmake")
  include("/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/shiplab/build/cmake/build-debug/cugl.dir/sdl2pckg.dir/sdl2atk/external/flac/src/share/replaygain_analysis/cmake_install.cmake")
  include("/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/shiplab/build/cmake/build-debug/cugl.dir/sdl2pckg.dir/sdl2atk/external/flac/src/share/replaygain_synthesis/cmake_install.cmake")
  include("/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/shiplab/build/cmake/build-debug/cugl.dir/sdl2pckg.dir/sdl2atk/external/flac/src/share/getopt/cmake_install.cmake")
  include("/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/shiplab/build/cmake/build-debug/cugl.dir/sdl2pckg.dir/sdl2atk/external/flac/src/share/utf8/cmake_install.cmake")
  include("/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/shiplab/build/cmake/build-debug/cugl.dir/sdl2pckg.dir/sdl2atk/external/flac/src/share/grabbag/cmake_install.cmake")

endif()

