cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)

set(EXTRA_COMPONENT_DIRS main application)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(ESP32-test-app)