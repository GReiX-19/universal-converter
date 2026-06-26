cmake_minimum_required(VERSION 3.21)
project(UniversalConverter VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

set(GUI_TYPE "")

if (WIN32)
    set(GUI_TYPE "WIN32")
endif()

qt_add_executable(${PROJECT_NAME} ${GUI_TYPE}
    src/main.cpp
    src/MainWindow.cpp
    src/SourcePanel.cpp
    src/FormatPanel.cpp
    src/OutputPanel.cpp
    src/Converter.cpp
    src/ConversionRules.cpp
    src/DependencyChecker.cpp
    src/Downloader.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets)

if(WIN32)
    get_target_property(_qt_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qt_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE} 
            --dir $<TARGET_FILE_DIR:${PROJECT_NAME}> 
            $<TARGET_FILE:${PROJECT_NAME}>
            COMMENT "Running windeployqt..."
        )
    endif()
endif()
