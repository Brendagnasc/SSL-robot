cmake_minimum_required(VERSION 3.16)
project(ssl_robot VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Protobuf REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(PkgConfig REQUIRED)
pkg_check_modules(MEMCACHED REQUIRED libmemcached)

file(GLOB PROTO_FILES "${CMAKE_SOURCE_DIR}/proto/*.proto")
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Protobuf_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${MEMCACHED_INCLUDE_DIRS}
)

add_executable(ssl_robot
    src/main.cpp
    src/vision/vision_client.cpp
    src/world/world_model.cpp
    src/strategy/strategy_engine.cpp
    src/skills/move_to_position.cpp
    ${PROTO_SRCS}
)

target_link_libraries(ssl_robot PRIVATE
    ${Protobuf_LIBRARIES}
    ${Boost_LIBRARIES}
    ${MEMCACHED_LIBRARIES}
    pthread
)
