# for lsp only, linker problem not solved yet

set(EXECUTABLE kernel8.elf)
set(LINKER_SCRIPT linker.ld)
file(GLOB C_FILES *.c)
file(GLOB ASM_FILES *.s)

#set_source_files_properties(${ASM_FILES} PROPERTIES COMPILE_FLAGS "")
add_executable(${EXECUTABLE} ${C_FILES} ${ASM_FILES} boot.S)
set_target_properties(${EXECUTABLE} PROPERTIES LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${LINKER_SCRIPT}")
#set_target_properties(${EXECUTABLE} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${LINKER_SCRIPT}")
    message( STATUS "linker script found" )
else()
    message( FATAL_ERROR "cannot find linker script ${CMAKE_CURRENT_SOURCE_DIR}/${LINKER_SCRIPT}" )
endif()
