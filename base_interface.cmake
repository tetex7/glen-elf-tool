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
include_guard(GLOBAL)

add_library(base_interface INTERFACE)
target_compile_features(base_interface INTERFACE
        c_std_99
        cxx_std_23
)

target_compile_options(base_interface INTERFACE -O0 -g -Wall -Werror -fno-strict-aliasing -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function)
target_compile_options(base_interface INTERFACE $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>)

if (CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(STATUS "Disabling clang specific warnings")
else ()
    target_compile_options(base_interface INTERFACE $<$<COMPILE_LANGUAGE:C>:-Wno-multistatement-macros>)
endif ()