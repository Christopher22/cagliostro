cmake_minimum_required(VERSION 3.15)
project(cagliostro)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(cagliostro_components Widgets Multimedia MultimediaWidgets)
find_package(Qt5 COMPONENTS ${cagliostro_components} REQUIRED)
list(TRANSFORM cagliostro_components PREPEND "Qt5::")

add_executable(cagliostro cagliostro/main.cpp cagliostro/model/Question.cpp cagliostro/model/Question.h cagliostro/model/Page.cpp cagliostro/model/Page.h cagliostro/model/content/Content.h cagliostro/model/Entity.cpp cagliostro/model/Entity.h cagliostro/model/Wizard.cpp cagliostro/model/Wizard.h cagliostro/model/Selection.cpp cagliostro/model/Selection.h cagliostro/view/Wizard.cpp cagliostro/view/Wizard.h cagliostro/view/Page.cpp cagliostro/view/Page.h cagliostro/model/content/Video.cpp cagliostro/model/content/Video.h cagliostro/model/content/Content.cpp)
target_link_libraries(cagliostro ${cagliostro_components})

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