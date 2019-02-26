cmake_minimum_required(VERSION 2.8)

## This includes the Vita toolchain, must go before project definition
# It is a convenience so you do not have to type 
# -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake for cmake. It is 
# highly recommended that you include this block for all projects.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VITASDK})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
    else()
        message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
    endif()
endif()

project(easyplugin)

include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_APP_NAME "Easy Plugin")
set(VITA_TITLEID  "ESPL00009")
set(VITA_VERSION  "01.00")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=7")


find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

link_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

file(GLOB vhbb
    "src/utils/vhbb/*.cpp"
    "src/utils/vhbb/*.h"
)

file(GLOB minizip
    "src/utils/vhbb/minizip/*.c"
    "src/utils/vhbb/minizip/*.h"
)

file(GLOB screens
    "src/screens/*.cpp"
    "src/screens/*.hpp"
)

add_executable(easyplugin
    ${minizip}
    ${vhbb}
    ${screens}
    src/main.cpp
    src/net/download.cpp
    src/utils/filesystem.cpp
    src/utils/search.cpp
    src/utils/format.cpp
    src/graphics/2dfont.cpp
)

target_link_libraries(easyplugin
    vita2d
    sqlite
    ${CURL_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    freetype
    png
    z
    c
    SceGxm_stub
    SceSqlite_stub 
    SceCtrl_stub
    SceNet_stub
    SceNetCtl_stub
    SceAppUtil_stub
    SceAppMgr_stub
    ScePromoterUtil_stub 
    SceSsl_stub
    SceIofilemgr_stub
    SceHttp_stub
    ScePower_stub
    SceIme_stub
    SceSysmodule_stub
    SceShellSvc_stub 
    SceDisplay_stub
    SceCommonDialog_stub
    SceLibKernel_stub
)


vita_create_self(easyplugin.self easyplugin UNSAFE)

vita_create_vpk(easyplugin.vpk ${VITA_TITLEID} easyplugin.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE sce_sys/icon0.png sce_sys/icon0.png
  FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
  FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
  FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
  FILE src/graphics/bg.png resources/bg.png
  FILE src/graphics/desc.png resources/desc.png
  FILE src/graphics/desc1.png resources/desc1.png
  FILE src/graphics/desc2.png resources/desc2.png
)