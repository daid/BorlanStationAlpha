cmake_minimum_required(VERSION 3.10)

project(BorlanStationAlpha CXX)

find_package(SDL2 REQUIRED)
if(NOT DEFINED SDL2_LIBRARIES)
    set(SDL2_LIBRARIES SDL2::SDL2)
endif()
if(WIN32)
    set(SDL2_LIBRARIES -Wl,-Bstatic ${SDL2_LIBRARIES} -lstdc++ -lpthread -Wl,-Bdynamic -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -lsetupapi)
endif()

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_executable(${PROJECT_NAME} ${SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wno-unused-parameter)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DSDL_FRONTEND)
target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL2_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
