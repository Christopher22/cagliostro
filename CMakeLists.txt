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

option(BUILD_CRYPTO "Build cagliostro with support for encrypted config files" ON)

# Get the path to current CMake
get_filename_component(cagliostro_cmake ${CMAKE_COMMAND} DIRECTORY)
MESSAGE(STATUS "Building with CMAKE in ${cagliostro_cmake}")

# Use conan with cmake
if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
endif ()
include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_check(REQUIRED)

# Specify the dependencies and require cryptopp only if it is needed
set(cagliostro_dependencies opencv/4.1.1@conan/stable)
if (BUILD_CRYPTO)
    list(APPEND cagliostro_dependencies "cryptopp/8.2.0@bincrafters/stable")
endif ()

# Specify the translations
set(cagliostro_translations "translations/cagliostro_de.ts")

# Specify conan options
set(cagliostro_options opencv:ffmpeg=False opencv:protobuf=False opencv:freetype=False opencv:harfbuzz=False opencv:eigen=False opencv:glog=False opencv:gflags=False opencv:quirc=False)

# Specify the required components
set(cagliostro_components Widgets Multimedia MultimediaWidgets)
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    LIST(APPEND cagliostro_components Test)
endif ()
find_package(Qt5 5.15 COMPONENTS ${cagliostro_components} LinguistTools REQUIRED)
list(TRANSFORM cagliostro_components PREPEND "Qt5::")

# Code files
set(src_model cagliostro/model/Question.cpp cagliostro/model/Question.h cagliostro/model/Page.cpp cagliostro/model/Page.h cagliostro/model/content/Content.h cagliostro/model/Entity.cpp cagliostro/model/Entity.h cagliostro/model/Wizard.cpp cagliostro/model/Wizard.h cagliostro/model/Selection.cpp cagliostro/model/Selection.h cagliostro/model/content/Video.cpp cagliostro/model/content/Video.h cagliostro/model/content/Content.cpp cagliostro/model/content/util/VideoDecoder.cpp cagliostro/model/content/util/VideoDecoder.h cagliostro/model/Config.cpp cagliostro/model/Config.h cagliostro/model/content/Resource.cpp cagliostro/model/content/Resource.h cagliostro/model/content/preprocessors/Header.cpp cagliostro/model/content/preprocessors/Header.h cagliostro/model/Responses.cpp cagliostro/model/Responses.h cagliostro/model/content/postprocessors/Delete.cpp cagliostro/model/content/postprocessors/Delete.h cagliostro/model/Source.cpp cagliostro/model/Source.h)
set(src_view cagliostro/view/Wizard.cpp cagliostro/view/Wizard.h cagliostro/view/Page.cpp cagliostro/view/Page.h cagliostro/view/VideoViewer.cpp cagliostro/view/VideoViewer.h cagliostro/view/Scale.cpp cagliostro/view/Scale.h cagliostro/view/ConfigPage.cpp cagliostro/view/ConfigPage.h cagliostro/view/util/FileSelector.cpp cagliostro/view/util/FileSelector.h cagliostro/view/util/Dialog.cpp cagliostro/view/util/Dialog.h cagliostro/view/util/DialogPage.cpp cagliostro/view/util/DialogPage.h cagliostro/view/util/ButtonGroup.cpp cagliostro/view/util/ButtonGroup.h)

# Prepare translations
set_source_files_properties(${cagliostro_translations} PROPERTIES OUTPUT_LOCATION "${CMAKE_BINARY_DIR}/translations")
qt5_add_translation(cagliostro_compiled_translations ${cagliostro_translations})
MESSAGE(STATUS "Using these translations: ${cagliostro_translations} --> ${cagliostro_compiled_translations}")

conan_cmake_run(REQUIRES ${cagliostro_dependencies} OPTIONS ${cagliostro_options} BASIC_SETUP CMAKE_TARGETS NO_OUTPUT_DIRS BUILD missing ENV PATH=${cagliostro_cmake} CONAN_CMAKE_PROGRAM=${CMAKE_COMMAND})
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    MESSAGE(STATUS "Build tests")
    enable_testing()
    add_executable(cagliostro ${src_model} cagliostro/Tests.cpp cagliostro/Tests.h)
    add_definitions(-DBUILD_TESTING)
    add_test(NAME CagliostroTest COMMAND cagliostro)
else ()
    if (WIN32)
        set(custom_flag WIN32)
    elseif (APPLE)
        set(custom_flag MACOSX_BUNDLE)
    endif ()
    add_executable(cagliostro ${custom_flag} ${cagliostro_compiled_translations} cagliostro/main.cpp ${src_model} ${src_view})
endif ()

# Build the crypto helper optional
if (BUILD_CRYPTO)
    MESSAGE(STATUS "Building cagliostro WITH support for encrypted files")
    add_executable(cagliostro_crypto cagliostro/main_crypto.cpp ${src_model})
    target_link_libraries(cagliostro_crypto ${cagliostro_components} CONAN_PKG::opencv CONAN_PKG::cryptopp)
    target_link_libraries(cagliostro ${cagliostro_components} CONAN_PKG::opencv CONAN_PKG::cryptopp)
    # We can rely on the deployment of the main executeable
else ()
    MESSAGE(STATUS "Building cagliostro WITHOUT support for encrypted files")
    target_link_libraries(cagliostro ${cagliostro_components} CONAN_PKG::opencv)
    target_compile_definitions(cagliostro PUBLIC NO_CRYPTO)
endif ()

get_target_property(qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(qt_bin_dir "${qmake_executable}" DIRECTORY)
if (WIN32)
    find_program(win_deploy_executable windeployqt HINTS "${qt_bin_dir}")
    add_custom_command(TARGET cagliostro POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E
            env PATH="${qt_bin_dir}" "${win_deploy_executable}"
            --verbose 0
            $<$<CONFIG:Debug>:--no-compiler-runtime>
            $<IF:$<CONFIG:Debug>,--debug,--release>
            \"$<TARGET_FILE:cagliostro>\"
            COMMENT "Deploying Qt for Windows..."
            )
elseif (APPLE)
    find_program(mac_deploy_executable macdeployqt HINTS "${qt_bin_dir}")
    add_custom_command(TARGET cagliostro POST_BUILD
            COMMAND "${mac_deploy_executable}" "$<TARGET_FILE_DIR:cagliostro>/../.." -always-overwrite
            COMMENT "Deploying Qt for macOS..."
            )
endif ()