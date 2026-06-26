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

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)

set(TOOLS_CACHE_DIR "${CMAKE_BINARY_DIR}/external-tools")
file(MAKE_DIRECTORY "${TOOLS_CACHE_DIR}")

if(WIN32)
    set(YTDLP_URL "https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe")
    set(YTDLP_DEST "${TOOLS_CACHE_DIR}/yt-dlp.exe")
    set(FFMPEG_ARCHIVE_URL "https://github.com/yt-dlp/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-win64-gpl.zip")
    set(FFMPEG_ARCHIVE "${TOOLS_CACHE_DIR}/ffmpeg.zip")
    set(FFMPEG_BIN_SUBDIR "ffmpeg-master-latest-win64-gpl/bin")
    set(FFMPEG_EXE_NAME "ffmpeg.exe")
    set(FFPROBE_EXE_NAME "ffprobe.exe")
else()
    set(YTDLP_URL "https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp_linux")
    set(YTDLP_DEST "${TOOLS_CACHE_DIR}/yt-dlp")
    set(FFMPEG_ARCHIVE_URL "https://github.com/yt-dlp/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-linux64-gpl.tar.xz")
    set(FFMPEG_ARCHIVE "${TOOLS_CACHE_DIR}/ffmpeg.tar.xz")
    set(FFMPEG_BIN_SUBDIR "ffmpeg-master-latest-linux64-gpl/bin")
    set(FFMPEG_EXE_NAME "ffmpeg")
    set(FFPROBE_EXE_NAME "ffprobe")
endif()

set(EXEC_PERMS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

if(NOT EXISTS "${YTDLP_DEST}")
    message(STATUS "Downloading yt-dlp...")
    file(DOWNLOAD "${YTDLP_URL}" "${YTDLP_DEST}" SHOW_PROGRESS STATUS YTDLP_STATUS)
    list(GET YTDLP_STATUS 0 YTDLP_CODE)
    if(NOT YTDLP_CODE EQUAL 0)
        message(WARNING "Failed to download yt-dlp - check your internet connection")
    elseif(NOT WIN32)
        file(CHMOD "${YTDLP_DEST}" PERMISSIONS ${EXEC_PERMS})
    endif()
endif()

if(NOT EXISTS "${TOOLS_CACHE_DIR}/${FFMPEG_EXE_NAME}")
    message(STATUS "Downloading ffmpeg...")
    file(DOWNLOAD "${FFMPEG_ARCHIVE_URL}" "${FFMPEG_ARCHIVE}" SHOW_PROGRESS STATUS FFMPEG_STATUS)
    list(GET FFMPEG_STATUS 0 FFMPEG_CODE)
    if(NOT FFMPEG_CODE EQUAL 0)
        message(WARNING "Failed to download ffmpeg — check your internet connection")
    else()
        file(ARCHIVE_EXTRACT INPUT "${FFMPEG_ARCHIVE}" DESTINATION "${TOOLS_CACHE_DIR}/_extracted")
        file(COPY
            "${TOOLS_CACHE_DIR}/_extracted/${FFMPEG_BIN_SUBDIR}/${FFMPEG_EXE_NAME}"
            "${TOOLS_CACHE_DIR}/_extracted/${FFMPEG_BIN_SUBDIR}/${FFPROBE_EXE_NAME}"
            DESTINATION "${TOOLS_CACHE_DIR}"
        )
        file(REMOVE_RECURSE "${TOOLS_CACHE_DIR}/_extracted")
        file(REMOVE "${FFMPEG_ARCHIVE}")
        if(NOT WIN32)
            file(CHMOD "${TOOLS_CACHE_DIR}/${FFMPEG_EXE_NAME}" PERMISSIONS ${EXEC_PERMS})
            file(CHMOD "${TOOLS_CACHE_DIR}/${FFPROBE_EXE_NAME}" PERMISSIONS ${EXEC_PERMS})
        endif()
    endif()
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${YTDLP_DEST}"
        "${TOOLS_CACHE_DIR}/${FFMPEG_EXE_NAME}"
        "${TOOLS_CACHE_DIR}/${FFPROBE_EXE_NAME}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMENT "Copying yt-dlp/ffmpeg next to the executable..."
)

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
