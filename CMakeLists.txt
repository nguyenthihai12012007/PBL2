cmake_minimum_required(VERSION 3.16)
project(JobMatch VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Fallback path for Qt 6.5.3 on macOS
if(EXISTS "/Users/nhuhoa/Qt/6.5.3/macos")
    list(PREPEND CMAKE_PREFIX_PATH "/Users/nhuhoa/Qt/6.5.3/macos")
endif()

# Find Qt 6 (Widgets)
find_package(Qt6 6.5 COMPONENTS Widgets REQUIRED)

# ==============================================================================
# TARGET: JobSystemQt (Desktop GUI)
# ==============================================================================
set(QT_SOURCES
    main_qt.cpp
    StyleHelper.h
    LoginWindow.h
    LoginWindow.cpp
    MainWindow.h
    MainWindow.cpp
    CandidatePage.h
    CandidatePage.cpp
    CompanyPage.h
    CompanyPage.cpp
    JobPage.h
    JobPage.cpp
    CustomMessageBox.h
    CustomMessageBox.cpp
    JobSearch.h
    JobSearch.cpp
    # Business logic files (Read/Write Data and Entities)
    class.h
    User.cpp
    Data.cpp
    Candidate.cpp
    Company.cpp
    Job.cpp
    Matching.h
    Matching.cpp
)

add_executable(JobSystemQt ${QT_SOURCES})

target_link_libraries(JobSystemQt PRIVATE
    Qt6::Widgets
)

target_include_directories(JobSystemQt PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
