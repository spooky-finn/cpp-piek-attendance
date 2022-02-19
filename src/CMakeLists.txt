cmake_minimum_required(VERSION 3.11-3.18)

project(cpp-piek-attendance)

# set(CURL_LIBRARY "-lcurl") 
# find_package(CURL REQUIRED) 

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
    ${CURL_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/json/single_include/nlohmann/
    ${CMAKE_SOURCE_DIR}/include/
)

add_executable(cpp-piek-attendance ${SOURCES} ${HEADERS})

# -- curl 
set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED) 
target_link_libraries(${PROJECT_NAME} PRIVATE ${CURL_LIBRARIES})


set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
