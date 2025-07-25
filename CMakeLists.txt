cmake_minimum_required(VERSION 3.15)

project(TaCAD VERSION 0.1 LANGUAGES CXX)

# Qt设置
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加自定义模块路径
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# 依赖库路径配置
set(QT6_DIR "" CACHE PATH "Qt6 installation directory")
set(OpenCASCADE_DIR "" CACHE PATH "OpenCASCADE installation directory")
set(BOOST_ROOT "" CACHE PATH "Boost root directory")

# 将用户指定的路径添加到搜索路径
if(QT6_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${QT6_DIR})
endif()

if(OpenCASCADE_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${OpenCASCADE_DIR})
endif()

if(BOOST_ROOT)
    list(APPEND CMAKE_PREFIX_PATH ${BOOST_ROOT})
endif()

# 查找Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets LinguistTools Test OpenGLWidgets)

message(STATUS "Found Qt6 at ${Qt6_DIR}")

# Qt6标准项目设置
qt_standard_project_setup()

# 包含配置文件
include(SetupOpenCASCADE)
include(SetupBoost)
include(SetupOpenGL)

# 添加子目录
add_subdirectory(cmake)
add_subdirectory(ext)
add_subdirectory(src)
