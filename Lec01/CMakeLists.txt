cmake_minimum_required(VERSION 2.8)

project(OpenGLLec01) 

set(PROJECT_DIR ${CMAKE_SOURCE_DIR})

set(PROJECT_INCLUDE_DIR ${PROJECT_DIR}/include)

set(PROJECT_SOURCE_DIR ${PROJECT_DIR}/src)

set(PROJECT_SOURCES
	${PROJECT_SOURCE_DIR}/kutuphane.cpp
	${PROJECT_SOURCE_DIR}/main.cpp
	${PROJECT_SOURCE_DIR}/glad.c
	)


set(PROJECT_HEADERS
	${PROJECT_SOURCE_DIR}/kutuphane.hpp
	)

find_package(glfw3 3.3 REQUIRED)

find_package(glm REQUIRED)

include_directories(${PROJECT_INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} )

target_link_libraries(${PROJECT_NAME} glfw)



