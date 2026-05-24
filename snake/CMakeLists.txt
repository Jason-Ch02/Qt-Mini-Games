cmake_minimum_required(VERSION 3.16)

project(snake--button VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia Network)
find_package(Qt6 REQUIRED COMPONENTS MultimediaWidgets)


set(PROJECT_SOURCES
    main.cpp
    widget.cpp
    index.cpp
    gameengine.cpp
    mediamanager.cpp
    index.h
    widget.h
    gameengine.h
    mediamanager.h
    index.ui
    widget.ui
    LeaderboardClient.h
    LeaderboardClient.cpp
    resources.rc
)

#为变量加上双引号，防止变量为空时 if 语句参数不足报错
if("${QT_VERSION_MAJOR}" STREQUAL "6")
    qt_add_executable(snake--button
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(snake--button SHARED ${PROJECT_SOURCES})
    else()
        add_executable(snake--button ${PROJECT_SOURCES})
    endif()
endif()

# 【核心改动 3】将 target_link_libraries 移到构建目标之后
# 只有先执行了上面的 qt_add_executable，这里才能给 snake--button 链接库
target_link_libraries(snake--button PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::Network
)
target_link_libraries(snake--button PRIVATE Qt6::MultimediaWidgets)

# 【核心改动 4】同样为变量加引号，解决 58 行左右的 Argument 报错
if("${QT_VERSION}" VERSION_LESS "6.1.0")
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.snake--button)
endif()

set_target_properties(snake--button PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS snake--button
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if("${QT_VERSION_MAJOR}" STREQUAL "6")
    qt_finalize_executable(snake--button)
endif()
