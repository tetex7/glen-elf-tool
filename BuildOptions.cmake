#
# Copyright (C) 2024  Tetex7
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

execute_process(
        COMMAND ${CMAKE_C_COMPILER} -dumpmachine
        OUTPUT_VARIABLE PLATFORM_TRIPLE
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(NOTICE "system triple-prefixed is ${PLATFORM_TRIPLE} on compiler ${CMAKE_C_COMPILER}")

# --- Automatically pick objcopy binary ---
# 1. Try triple-prefixed version (e.g. x86_64-linux-gnu-objcopy)
# 2. Fall back to plain objcopy if triple is empty or prefixed tool doesn't exist
if(PLATFORM_TRIPLE)
    find_program(PLAT_OBJCOPY
            NAMES
            ${PLATFORM_TRIPLE}-objcopy
            objcopy
            DOC "Platform-appropriate objcopy tool"
    )
    find_program(PLAT_NM
            NAMES
            ${PLATFORM_TRIPLE}-nm
            nm
            DOC "Platform-appropriate objcopy tool"
    )
else()
    find_program(PLAT_OBJCOPY
            NAMES
            objcopy
            DOC "Default objcopy tool"
    )
    find_program(PLAT_NM
            NAMES
            nm
            DOC "Default objcopy tool"
    )
endif()

if(NOT PLAT_OBJCOPY)
    message(FATAL_ERROR "Failed to locate objcopy for platform '${PLATFORM_TRIPLE}'. Please install binutils.")
else()
    message(STATUS "Using objcopy: ${PLAT_OBJCOPY}")
endif()

if(NOT PLAT_NM)
    message(FATAL_ERROR "Failed to locate nm for platform '${PLATFORM_TRIPLE}'. Please install binutils.")
else()
    message(STATUS "Using nm: ${PLAT_NM}")
endif()

# Debug flag control
option(GLEN_ELF_TOOL_CONFIG_DEBUG "Enable debug mode" ON)

string(TIMESTAMP GLEN_ELF_TOOL_CONFIG_GENERATION_DATE "%m-%d-%Y(%H:%M:%S)")
string(TIMESTAMP GLEN_ELF_TOOL_CONFIG_GENERATION_YEAR "%Y")


option(GLEN_LOADER_USE_CPACK_PACKAGING "If Cpack packaging should be enabled" ON)


function(set_target_output target_name out_path)
    set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${out_path}"
            LIBRARY_OUTPUT_DIRECTORY "${out_path}"
            ARCHIVE_OUTPUT_DIRECTORY "${out_path}"
    )
endfunction()

execute_process(COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE GIT_COMMIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git rev-parse --abbrev-ref HEAD OUTPUT_VARIABLE GIT_BRANCH_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)

# --- Create a custom "config_info" target ---
set(CONFIG_SUMMARY "${CMAKE_BINARY_DIR}/CMakeFiles/config_summary.txt")

file(WRITE ${CONFIG_SUMMARY} "========= libsafetynet Build Configuration =========\n")
file(APPEND ${CONFIG_SUMMARY} "Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}\n")
file(APPEND ${CONFIG_SUMMARY} "Platform: ${CMAKE_SYSTEM_NAME}\n")
file(APPEND ${CONFIG_SUMMARY} "Platform Triple: ${PLATFORM_TRIPLE}\n")
file(APPEND ${CONFIG_SUMMARY} "Platform C Compiler: ${CMAKE_C_COMPILER}\n")
file(APPEND ${CONFIG_SUMMARY} "Platform Objcopy Binary: ${PLAT_OBJCOPY}\n")
file(APPEND ${CONFIG_SUMMARY} "Platform NM Binary: ${PLAT_NM}\n")
file(APPEND ${CONFIG_SUMMARY} "Git: ${GIT_BRANCH_NAME} (${GIT_COMMIT_HASH})\n\n")
file(APPEND ${CONFIG_SUMMARY} "Feature Flags:\n")
foreach(flag
        GLEN_ELF_TOOL_CONFIG_DEBUG
        GLEN_LOADER_USE_CPACK_PACKAGING
)
    file(APPEND ${CONFIG_SUMMARY} "  ${flag} = ${${flag}}\n")
endforeach()
file(APPEND ${CONFIG_SUMMARY} "====================================================\n")

# target to print it using cmake -E cat (no shell parsing)
add_custom_target(config_info
        COMMAND ${CMAKE_COMMAND} -E cat ${CONFIG_SUMMARY}
        COMMENT "Display the current trt os boot loader configuration"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)