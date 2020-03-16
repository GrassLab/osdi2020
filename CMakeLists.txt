cmake_minimum_required(VERSION 3.16)
PROJECT (OSDI2020)

#set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(ARMGNU "aarch64-linux-gnu")
set(CMAKE_C_COMPILER "${ARMGNU}-gcc")
SET(CMAKE_C_LINK_EXECUTABLE "${ARMGNU}-ld")
#set(COPS "-Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only")

#set(ASMOPS -Iinclude)
set(CMAKE_C_FLAGS "-I${INC_DIR} -Wall -nostdlib -nostartfiles -ffreestanding -mgeneral-regs-only")

add_subdirectory(src)
include_directories()
