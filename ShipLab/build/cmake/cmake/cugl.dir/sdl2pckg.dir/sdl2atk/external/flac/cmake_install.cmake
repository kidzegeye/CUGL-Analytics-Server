# Install script for directory: /Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/sdl2atk/external/flac

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
    set(CMAKE_INSTALL_CONFIG_NAME "")
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

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/FLAC/targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/FLAC/targets.cmake"
         "/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/ShipLab/build/cmake/cmake/cugl.dir/sdl2pckg.dir/sdl2atk/external/flac/CMakeFiles/Export/baf7b64a9c1b56d368d1c4c52c93ff8a/targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/FLAC/targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/FLAC/targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/FLAC" TYPE FILE FILES "/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/ShipLab/build/cmake/cmake/cugl.dir/sdl2pckg.dir/sdl2atk/external/flac/CMakeFiles/Export/baf7b64a9c1b56d368d1c4c52c93ff8a/targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/FLAC" TYPE FILE FILES "/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/ShipLab/build/cmake/cmake/cugl.dir/sdl2pckg.dir/sdl2atk/external/flac/CMakeFiles/Export/baf7b64a9c1b56d368d1c4c52c93ff8a/targets-noconfig.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/FLAC" TYPE FILE FILES
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/ShipLab/build/cmake/cmake/cugl.dir/sdl2pckg.dir/sdl2atk/external/flac/flac-config.cmake"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/ShipLab/build/cmake/cmake/cugl.dir/sdl2pckg.dir/sdl2atk/external/flac/flac-config-version.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/FLAC" TYPE FILE FILES
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/ShipLab/build/cmake/cmake/cugl.dir/sdl2pckg.dir/sdl2atk/external/flac/flac-config.cmake"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/ShipLab/build/cmake/cmake/cugl.dir/sdl2pckg.dir/sdl2atk/external/flac/flac-config-version.cmake"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/ShipLab/build/cmake/cmake/cugl.dir/sdl2pckg.dir/sdl2atk/external/flac/src/cmake_install.cmake")

endif()
