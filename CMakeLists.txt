cmake_minimum_required(VERSION 3.21)
project(UniversalConverter VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(UniversalConverter 
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

target_link_libraries(UniversalConverter PRIVATE Qt6::Core Qt6::Widgets)

if(WIN32)
    # set_target_properties(UniversalConverter PROPERTIES WIN32_EXECUTABLE ON)
    # add_custom_command(TARGET UniversalConverter POST_BUILD
    #     COMMAND ${Qt6_DIR}/../../../bin/windeployqt.exe
    #     "$<TARGET_FILE:UniversalConverter>"
    # )
endif()