# Install script for directory: /Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/sdl2atk/external/ogg

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

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/shiplab/build/cmake/build-debug/cugl.dir/sdl2pckg.dir/sdl2atk/external/ogg/libogg.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libogg.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libogg.a")
    execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libogg.a")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ogg" TYPE FILE FILES
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/shiplab/build/cmake/build-debug/cugl.dir/sdl2pckg.dir/sdl2atk/external/ogg/include/ogg/config_types.h"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/../../external/ogg/include/ogg/ogg.h"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/../../external/ogg/include/ogg/os_types.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg/OggTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg/OggTargets.cmake"
         "/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/shiplab/build/cmake/build-debug/cugl.dir/sdl2pckg.dir/sdl2atk/external/ogg/CMakeFiles/Export/dee6fd410a50d06b294b496f57355584/OggTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg/OggTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg/OggTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg" TYPE FILE FILES "/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/shiplab/build/cmake/build-debug/cugl.dir/sdl2pckg.dir/sdl2atk/external/ogg/CMakeFiles/Export/dee6fd410a50d06b294b496f57355584/OggTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg" TYPE FILE FILES "/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/shiplab/build/cmake/build-debug/cugl.dir/sdl2pckg.dir/sdl2atk/external/ogg/CMakeFiles/Export/dee6fd410a50d06b294b496f57355584/OggTargets-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg" TYPE FILE FILES
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/shiplab/build/cmake/build-debug/cugl.dir/sdl2pckg.dir/sdl2atk/external/ogg/OggConfig.cmake"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/shiplab/build/cmake/build-debug/cugl.dir/sdl2pckg.dir/sdl2atk/external/ogg/OggConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/Users/kiduszegeye/Desktop/Cornell/CS 5999/lab1/shiplab/build/cmake/build-debug/cugl.dir/sdl2pckg.dir/sdl2atk/external/ogg/ogg.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/SDL2_atk/html" TYPE FILE FILES
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/framing.html"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/index.html"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/oggstream.html"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/ogg-multiplex.html"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/fish_xiph_org.png"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/multiplex1.png"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/packets.png"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/pages.png"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/stream.png"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/vorbisword2.png"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/white-ogg.png"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/white-xifish.png"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/rfc3533.txt"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/rfc5334.txt"
    "/Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/skeleton.html"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/SDL2_atk/html" TYPE DIRECTORY FILES "/Users/kiduszegeye/Desktop/Cornell/CS 5999/CUGL-Analytics-Server/cugl/sdlapp/buildfiles/cmake/../../external/ogg/doc/libogg")
endif()

