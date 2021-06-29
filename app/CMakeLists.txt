cmake_minimum_required(VERSION 3.9)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

add_executable(App
    main.cpp
    workerclient.hpp workerclient.cpp
    mandelbrotcalculator.hpp mandelbrotcalculator.cpp
    dialog.hpp dialog.cpp
    mandelbrotwidget.hpp mandelbrotwidget.cpp
)
target_link_libraries(App PRIVATE Qt5::Widgets Qt5::Network Mandelbrot::SDK)
