cmake_minimum_required(VERSION 3.26)
project(mqtt)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)
add_definitions(-DCMAKE_TOOLCHAIN_FILE=/root/vcpkg/scripts/buildsystems/vcpkg.cmake)

set(CMAKE_PREFIX_PATH "/usr/local/lib64/cmake/PahoMqttCpp" ${CMAKE_PREFIX_PATH})

find_package(PahoMqttCpp REQUIRED)
find_package(tinyxml2 REQUIRED)

add_executable(mqtt main.cpp CRC24Q.cpp)

target_link_libraries(mqtt
        tinyxml2::tinyxml2
        PahoMqttCpp::paho-mqtt3a PahoMqttCpp::paho-mqttpp3)

add_executable(test test.cpp)

add_executable(guide guide.cpp)
target_link_libraries(guide
        PahoMqttCpp::paho-mqtt3a PahoMqttCpp::paho-mqttpp3)
