# Author: Gertjan Debusschere
# License: GPL-3.0-or-later
cmake_minimum_required(VERSION 3.10)
project(MarkdownCsvEditor)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 COMPONENTS Widgets PrintSupport Core5Compat REQUIRED)
add_executable(MarkdownCsvEditor
    src/main.cpp
    src/MainWindow.cpp
    src/MarkdownEditorWidget.cpp
    src/CsvEditorWidget.cpp
    src/FileHandler.cpp
    src/MarkdownPreviewWidget.cpp
    src/MarkdownHighlighter.cpp
    src/WelcomePage.cpp
    src/WelcomePage.h
)
target_link_libraries(MarkdownCsvEditor PRIVATE Qt6::Widgets Qt6::PrintSupport Qt6::Core5Compat)
