cmake_minimum_required(VERSION 3.15)
project(${PROJECT_NAME} CXX)

find_package(cpptango CONFIG REQUIRED)
if (NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_package(crashreporting2 CONFIG)
endif()

add_compile_definitions(PROJECT_NAME=${PROJECT_NAME})
add_compile_definitions(PROJECT_VERSION=${PROJECT_VERSION})

file(GLOB sources
    src/*.cpp
)

set(includedirs 
    src
)

add_executable(${EXECUTABLE_NAME} ${sources})
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${includedirs})
target_link_libraries(${EXECUTABLE_NAME} PRIVATE cpptango::cpptango)
if (NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE crashreporting2::crashreporting2)
endif()

install(TARGETS ${EXECUTABLE_NAME} DESTINATION "." RUNTIME DESTINATION bin)
