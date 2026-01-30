cmake_minimum_required(VERSION 3.16)
project(test VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(CORE_SOURCES
    core/passenger.cpp
    core/discount.cpp
    core/tariff.cpp
    core/ticket.cpp
    core/station.cpp
)

set(CORE_HEADERS
    core/types.h
    core/passenger.h
    core/discount.h
    core/tariff.h
    core/ticket.h
    core/station.h
)

set(UI_SOURCES
    mainwindow.cpp
    addingdialog.cpp
    deldialog.cpp
    main.cpp
)

set(UI_HEADERS
    mainwindow.h
    addingdialog.h
    deldialog.h
)

set(FORMS
    mainwindow.ui
    addingdialog.ui
    deldialog.ui
)

add_executable(${PROJECT_NAME}
    ${CORE_SOURCES}
    ${CORE_HEADERS}
    ${UI_SOURCES}
    ${UI_HEADERS}
    ${FORMS}
)

target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/core
)
