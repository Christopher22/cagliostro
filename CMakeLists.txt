#[[
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
]]

cmake_minimum_required(VERSION 3.15)
project(cagliostro)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Use conan with cmake
if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
endif ()
include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_check(REQUIRED)

# Specify the dependencies
set(cagliostro_dependencies opencv/4.1.1@conan/stable)
set(cagliostro_options opencv:ffmpeg=False opencv:protobuf=False opencv:freetype=False opencv:harfbuzz=False opencv:eigen=False opencv:glog=False opencv:gflags=False opencv:quirc=False)
conan_cmake_run(REQUIRES ${cagliostro_dependencies} OPTIONS ${cagliostro_options} BASIC_SETUP CMAKE_TARGETS BUILD missing)

set(cagliostro_components Widgets Multimedia MultimediaWidgets)
find_package(Qt5 COMPONENTS ${cagliostro_components} REQUIRED)
list(TRANSFORM cagliostro_components PREPEND "Qt5::")

add_executable(cagliostro cagliostro/main.cpp cagliostro/model/Question.cpp cagliostro/model/Question.h cagliostro/model/Page.cpp cagliostro/model/Page.h cagliostro/model/content/Content.h cagliostro/model/Entity.cpp cagliostro/model/Entity.h cagliostro/model/Wizard.cpp cagliostro/model/Wizard.h cagliostro/model/Selection.cpp cagliostro/model/Selection.h cagliostro/view/Wizard.cpp cagliostro/view/Wizard.h cagliostro/view/Page.cpp cagliostro/view/Page.h cagliostro/model/content/Video.cpp cagliostro/model/content/Video.h cagliostro/model/content/Content.cpp cagliostro/model/content/util/VideoDecoder.cpp cagliostro/model/content/util/VideoDecoder.h cagliostro/view/VideoViewer.cpp cagliostro/view/VideoViewer.h)
target_link_libraries(cagliostro ${cagliostro_components} CONAN_PKG::opencv)

IF (WIN32)
    # On Windows, deploy all required DLLs automatically
    get_target_property(qmake_executable Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(qt_bin_dir "${qmake_executable}" DIRECTORY)
    find_program(win_deploy_executable windeployqt HINTS "${qt_bin_dir}")

    add_custom_command(TARGET cagliostro POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E
            env PATH="${qt_bin_dir}" "${win_deploy_executable}"
            --verbose 0
            $<$<CONFIG:Debug>:--no-compiler-runtime>
            $<IF:$<CONFIG:Debug>,--debug,--release>
            \"$<TARGET_FILE:cagliostro>\"
            COMMENT "Deploying Qt..."
            )
endif ()