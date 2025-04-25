cmake_minimum_required(VERSION 3.15)  
project(little_reflection C CXX)  


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)  

set(CMAKE_CXX_STANDARD 20) 
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB ALL_FILES   
    ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp  
    ${CMAKE_CURRENT_SOURCE_DIR}/jsoncpp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/json/*.h
)  

add_executable(app ${ALL_FILES})  
