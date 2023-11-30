cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(my_Bartender)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

find_package(ROOT REQUIRED)

include(${ROOT_USE_FILE})

include_directories(${ROOT_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)

link_directories(${ROOT_LIBRARY_DIR})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

file(GLOB MACRO_FILES
    "*.mac"
)

file(COPY ${MACRO_FILES} DESTINATION ${PROJECT_BINARY_DIR})

add_executable(bartender bartender.cc ${sources} ${headers})
target_link_libraries(bartender ${ROOT_LIBRARIES})

add_custom_target(my_Bartender DEPENDS bartender)