project(gmp)

cmake_minimum_required(VERSION 3.5)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

if(CORE_SYSTEM_NAME MATCHES "linux")
  if (CPU MATCHES "i.86")
    list(APPEND gmp_conf ABI=32)
  endif()
elseif(CORE_SYSTEM_NAME MATCHES "darwin_embedded")
  list(APPEND gmp_conf --disable-assembly)
elseif(CORE_SYSTEM_NAME MATCHES "osx")
  list(APPEND gmp_conf --with-pic)
elseif(CORE_SYSTEM_NAME MATCHES "android")
  list(APPEND gmp_conf --with-pic)
endif()


if(CORE_SYSTEM_NAME STREQUAL ios OR CORE_SYSTEM_NAME STREQUAL darwin_embedded)
set (CMAKE_IOS_DEVELOPER_ROOT "${CMAKE_XCODE_ROOT}/Platforms/iPhone${DEVICE_OR_SIMULATOR}.platform/Developer")
# Find and use the most recent iOS sdk unless specified manually with CMAKE_IOS_SDK_ROOT
if (NOT DEFINED CMAKE_IOS_SDK_ROOT)
    file (GLOB _CMAKE_IOS_SDKS "${CMAKE_IOS_DEVELOPER_ROOT}/SDKs/*")
    if (_CMAKE_IOS_SDKS)
        list (SORT _CMAKE_IOS_SDKS)
        list (REVERSE _CMAKE_IOS_SDKS)
        list (GET _CMAKE_IOS_SDKS 0 CMAKE_IOS_SDK_ROOT)
    else ()
        message (STATUS "XXXXXXNo iOS SDK's found in default search path ${CMAKE_IOS_DEVELOPER_ROOT}. Manually set CMAKE_IOS_SDK_ROOT or install the iOS SDK.")
    endif ()
    message (STATUS "XXXXXXXToolchain using default iOS SDK: ${CMAKE_IOS_SDK_ROOT}")
endif ()
#set (CMAKE_IOS_SDK_ROOT ${CMAKE_IOS_SDK_ROOT} CACHE PATH "Location of the selected iOS SDK")
message (STATUS "XXXXXXXToolch Test")
# set (CMAKE_OSX_SYSROOT ${CMAKE_IOS_SDK_ROOT} CACHE PATH "Sysroot used for iOS support")
endif()

#
# Darwin based OSes will use -isysroot, however libtool will expect --sysroot
# So we hack around this otherwise libttol won't be able to find the standard C headers
#
if(CORE_SYSTEM_NAME STREQUAL osx)
  set (COMPILER_WITH_LIBTOOL_SYSROOT_APPLE
  "CC_FOR_BUILD=${CMAKE_C_COMPILER} --sysroot ${CMAKE_OSX_SYSROOT}"
  "CPP_FOR_BUILD=${CMAKE_C_COMPILER} -E --sysroot ${CMAKE_OSX_SYSROOT}"
  )
elseif(CORE_SYSTEM_NAME STREQUAL ios OR CORE_SYSTEM_NAME STREQUAL darwin_embedded)
  set (COMPILER_WITH_LIBTOOL_SYSROOT_APPLE
  "CC_FOR_BUILD=${CMAKE_C_COMPILER} --sysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk"
  "CPP_FOR_BUILD=${CMAKE_C_COMPILER} -E --sysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk"
  )
else()
  set (COMPILER_WITH_LIBTOOL_SYSROOT_APPLE "")
endif()

include(ExternalProject)
externalproject_add(gmp
                    SOURCE_DIR ${CMAKE_SOURCE_DIR}
                    CONFIGURE_COMMAND <SOURCE_DIR>/configure
                      ${COMPILER_WITH_LIBTOOL_SYSROOT_APPLE}
                      --prefix=${CMAKE_INSTALL_PREFIX}
                      --disable-shared
                      --with-pic
                      ${gmp_conf})

install(CODE "Message(Done)")

