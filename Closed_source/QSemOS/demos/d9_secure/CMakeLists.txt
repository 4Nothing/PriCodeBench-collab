cmake_minimum_required(VERSION 3.12)
project(${APP} LANGUAGES C ASM)

set(HOME_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../..)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_LINKER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-ld)

set(CMAKE_C_FLAGS "-g -O0 -mthumb -D__thumb__ -mcpu=cortex-r5 -std=gnu11 -fno-common -fomit-frame-pointer -mthumb -mfloat-abi=soft -mfpu=fpv4-sp-d16  -Wa,-mimplicit-it=thumb  -fstack-protector-strong  -funsigned-char -fdata-sections -ffunction-sections -fshort-enums")
set(CMAKE_CXX_FLAGS "-mthumb -D__thumb__ -mcpu=cortex-r5 -mfloat-abi=soft -mfpu=fpv4-sp-d16  -fno-threadsafe-statics -fno-builtin -DEIGEN_NO_IO=1")

set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_ASM_FLAGS ${CMAKE_C_FLAGS})
set(ldfile "${CMAKE_CURRENT_SOURCE_DIR}/build/d9_secure.ld")
set(CMAKE_EXE_LINKER_FLAGS "-nostdlib -Wl,-EL -Wl,-d -Wl,-no-enum-size-warning  -u _printf_float -nostartfiles -static -T ${ldfile}")

add_compile_options(
    -Wno-parentheses
)

include(${HOME_PATH}/cmake/functions/uniproton_functions.cmake)
import_kconfig(${HOME_PATH}/build/uniproton_config/config_cortex_r5_${CPU_TYPE}/defconfig)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/arch
    ${CMAKE_CURRENT_SOURCE_DIR}/include/net
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utility
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/d9_secure
    ${CMAKE_CURRENT_SOURCE_DIR}/apps
)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)
link_libraries(
    -Wl,--start-group
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/libCortexMXsec_c.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/libD9_SECURE.a"
    -Wl,--end-group
    -lgcc
)

add_subdirectory(config)
add_subdirectory(src)
add_subdirectory(apps)

list(APPEND OBJS $<TARGET_OBJECTS:config>)
list(APPEND OBJS $<TARGET_OBJECTS:src>)
list(APPEND OBJS $<TARGET_OBJECTS:apps>)

add_executable(${APP} ${OBJS})
