# Author: Gertjan Debusschere
# License: GPL-3.0-or-later
cmake_minimum_required(VERSION 3.10)
project(MarkdownCsvEditor)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.1/mingw_64/lib/cmake")
find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport)
# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

# Source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MarkdownEditorWidget.cpp
    src/CsvEditorWidget.cpp
    src/FileHandler.cpp
    src/MarkdownPreviewWidget.cpp
    src/MarkdownHighlighter.cpp
)

# Add executable target
add_executable(MarkdownCsvEditor
    src/main.cpp
    src/MainWindow.cpp
    src/MarkdownEditorWidget.cpp
    src/CsvEditorWidget.cpp
    src/FileHandler.cpp
    src/MarkdownPreviewWidget.cpp
    src/MarkdownHighlighter.cpp
)
target_link_libraries(MarkdownCsvEditor PRIVATE Qt6::Widgets Qt6::PrintSupport)
