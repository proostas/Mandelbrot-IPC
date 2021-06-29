cmake_minimum_required(VERSION 3.9)

find_package(Qt5 COMPONENTS Core Network REQUIRED)

add_executable(Worker
    main.cpp
    job.hpp job.cpp
    worker.hpp worker.cpp
    application.hpp application.cpp
)
target_link_libraries(Worker PRIVATE Qt5::Core Qt5::Network Mandelbrot::SDK)
