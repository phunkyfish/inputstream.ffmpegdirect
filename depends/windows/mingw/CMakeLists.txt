cmake_minimum_required(VERSION 3.5)
project(mingw)

foreach(repo msys mingw32 mingw64)
  if(${repo} STREQUAL msys)
    file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/etc/pacman.d/mirrorlist.${repo} "Server = http://mirrors.kodi.tv/build-deps/win32/msys2/repos/${repo}2/$arch\n")
  else()
    file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/etc/pacman.d/mirrorlist.${repo} "Server = http://mirrors.kodi.tv/build-deps/win32/msys2/repos/${repo}\n")
  endif()
endforeach()

include(CheckSymbolExists)
check_symbol_exists(_X86_ "Windows.h" _X86_)
check_symbol_exists(_AMD64_ "Windows.h" _AMD64_)

if(_X86_)
   set(HOST mingw-w64-i686)
   set(MINGW_PATH "mingw32")
elseif(_AMD64_)
   set(HOST mingw-w64-x86_64)
   set(MINGW_PATH "mingw64")
else()
   message(FATAL_ERROR "Unsupported architecture")
endif()

execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/usr/bin/bash.exe --login -c
  "pacman-key --init" ERROR_QUIET)
execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/usr/bin/bash.exe --login -c
  "pacman --noconfirm -S make ${HOST}-gcc autoconf automake libtool")

file(GLOB_RECURSE shared_libs ${CMAKE_CURRENT_SOURCE_DIR}/${MINGW_PATH}/*.dll.a)
if(shared_libs)
  file(REMOVE ${shared_libs})
endif()

configure_file(MinGWConfig.cmake.in MinGWConfig.cmake @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/MinGWConfig.cmake DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/caller.sh DESTINATION ${CMAKE_INSTALL_PREFIX})
