cmake_minimum_required(VERSION 3.19)

set(CMAKE_TOOLCHAIN_FILE "$ENV{SCE_PSP2_SDK_DIR}/host_tools/build/cmake/psp2-snc-toolchain.cmake")

project(VitaMediaPlayer)

include(VitaDevelopmentSuite)
set(CMAKE_C_STANDARD 99)

add_compile_options(
  -Xdiag=0 -Xquit=2
)

add_link_options(
  -s
)

FUNCTION(ADD_RESOURCES out_var)
  SET(result)
  FOREACH(in_f ${ARGN})
    SET(out_f "${CMAKE_CURRENT_BINARY_DIR}/${in_f}.o")
    GET_FILENAME_COMPONENT(out_dir ${out_f} DIRECTORY)
    ADD_CUSTOM_COMMAND(OUTPUT ${out_f}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${out_dir}
      COMMAND $ENV{SCE_PSP2_SDK_DIR}/host_tools/build/bin/psp2bin "${in_f}"
        -b2e -o "${out_f}"
      DEPENDS ${in_f}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMENT "Building resource ${out_f}"
      VERBATIM
      )
    LIST(APPEND result ${out_f})
  ENDFOREACH()
  SET(${out_var} "${result}" PARENT_SCOPE)
ENDFUNCTION()

file(GLOB res_files RELATIVE
  ${CMAKE_SOURCE_DIR}
  resources/*.png
)

add_resources(player_resources ${res_files})

include_directories(
  ${VDSUITE_USER_INCLUDE_DIRECTORIES}
  ${VDSUITE_STANDARD_INCLUDE_DIRECTORIES}
  src/include
)

link_directories(
  ${VDSUITE_LIBRARY_DIRECTORIES}
  ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(${PROJECT_NAME}
  ${player_resources}
  src/main.c
  src/utils.c
  src/dir.c
  src/fs.c
  src/texture.c
  src/commonValues.c
  src/menus/menuMain.c
  src/menus/menuInfo.c
  src/avplayer/avsubs.c
  src/avplayer/avplayer.c
  src/avplayer/avsound.c
  src/avplayer/avplayerUtils.c
  src/avplayer/overlay.c
)

target_link_libraries(${PROJECT_NAME}
  vita2d_sys_stub
  SceKernel_stub
  SceAvcodec_stub
  SceVideodec_stub
  SceAppUtil_stub
  SceAvPlayer_stub
  SceAudio_stub
  SceCtrl_stub
  SceDisplay_stub
  SceSysmodule_stub
  SceIofilemgr_stub
  SceGxm_stub
  SceThreadmgr_stub
  SceModulemgr_stub
)

VDSuiteCreateSfo(vmp-param.sfo param.yml)

VDSuitePackage(${PROJECT_NAME}.vpk
  FILES
    $<TARGET_FILE:${PROJECT_NAME}> eboot.bin
    vmp-param.sfo sce_sys/param.sfo
    resources/OpenSans-Bold.ttf OpenSans-Bold.ttf
    modules/reAvPlayer.suprx modules/reAvPlayer.suprx
    VMP_livearea sce_sys
)