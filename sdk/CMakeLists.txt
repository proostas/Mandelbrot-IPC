cmake_minimum_required(VERSION 3.9)

find_package(Qt5 COMPONENTS Core Network REQUIRED)

add_library(SDK
        message.hpp
        jobrequest.hpp
        jobresult.hpp
        messageutils.hpp
)
target_link_libraries(SDK PUBLIC Qt5::Core Qt5::Network)
target_include_directories(SDK PUBLIC ${CMAKE_CURRENT_LIST_DIR})
add_library(Mandelbrot::SDK ALIAS SDK)
