#
# Copyright (C) 2025  Tetex7
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

cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SKIP_INSTALL_RULES ON)


if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

project(glen_elf_tool VERSION 1.0.0 LANGUAGES C CXX)
include(base_interface.cmake)
include(BuildOptions.cmake)

set(BIN_DIR ${CMAKE_BINARY_DIR}/build)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_SOURCE_DIR}/include)

find_package(Boost 1.65 REQUIRED COMPONENTS program_options)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/glen_elf_tool_config.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/include/glen_elf_tool_config.h
)

file(GLOB MAIN_TOOL_C_SOURCES "${SRC_DIR}/*.c" "${SRC_DIR}/**/*.c")
file(GLOB MAIN_TOOL_CXX_SOURCES "${SRC_DIR}/*.cpp" "${SRC_DIR}/**/*.cpp")

add_executable(glen_elf_tool ${MAIN_TOOL_C_SOURCES} ${MAIN_TOOL_CXX_SOURCES})
target_include_directories(glen_elf_tool PUBLIC ${INC_DIR})
target_link_libraries(glen_elf_tool PUBLIC base_interface)
#target_link_libraries(glen_elf_tool PUBLIC backtrace)
set_target_output(glen_elf_tool ${BIN_DIR})
target_link_libraries(glen_elf_tool PRIVATE Boost::program_options)

