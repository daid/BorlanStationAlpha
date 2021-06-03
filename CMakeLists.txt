cmake_minimum_required(VERSION 3.10)

project(BorlanStationAlpha CXX)

option(CONSOLE "Enable console frontend instead of SDL frontend" OFF)

if (NOT ${CONSOLE})
    find_package(SDL2 REQUIRED)
    if(NOT DEFINED SDL2_LIBRARIES)
        set(SDL2_LIBRARIES SDL2::SDL2)
    endif()
    if(WIN32)
        set(SDL2_LIBRARIES -Wl,-Bstatic ${SDL2_LIBRARIES} -lstdc++ -lpthread -Wl,-Bdynamic -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -lsetupapi)
    endif()
endif()

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_executable(${PROJECT_NAME} ${SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wshadow -Wno-unused-parameter -Wno-narrowing)

if (${CONSOLE})
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DCONSOLE_FRONTEND=1)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DSDL_FRONTEND=1)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL2_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
endif()
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(${PROJECT_NAME} PUBLIC "-mconsole")
endif()
