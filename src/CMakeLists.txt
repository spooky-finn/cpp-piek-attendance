cmake_minimum_required(VERSION 3.11-3.18)

project(cpp-piek-attendance)

set(SOURCES
    dataPreparation.cpp
    GQLTransmitter.cpp
    Employee.cpp
    DataLogger.cpp
    utils.cpp
    main.cpp
)

set(HEADERS
)

include_directories(
    ${CMAKE_SOURCE_DIR}/include/
)
find_package(CURL REQUIRED) 

add_executable(cpp-piek-attendance ${SOURCES} ${HEADERS})

# CURL
include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARIES})


set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
