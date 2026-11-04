cmake_minimum_required(VERSION 3.16)

project(damm93
    VERSION 1.0.0
    DESCRIPTION "Base93 implementation of the Damm check digit algorithm"
    LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(MSVC)
    set(DAMM93_WARNINGS /W4)
else()
    set(DAMM93_WARNINGS -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

add_library(damm93lib src/damm93.c)
target_include_directories(damm93lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_compile_options(damm93lib PRIVATE ${DAMM93_WARNINGS})
set_target_properties(damm93lib PROPERTIES OUTPUT_NAME damm93)

add_executable(damm93 src/main.c)
target_link_libraries(damm93 PRIVATE damm93lib)
target_compile_options(damm93 PRIVATE ${DAMM93_WARNINGS})
target_compile_definitions(damm93 PRIVATE DAMM93_VERSION="${PROJECT_VERSION}")

include(GNUInstallDirs)
install(TARGETS damm93 damm93lib)
install(FILES include/damm93.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CTest)
if(BUILD_TESTING)
    add_executable(test_damm93 tests/test_damm93.c)
    target_link_libraries(test_damm93 PRIVATE damm93lib)
    target_compile_options(test_damm93 PRIVATE ${DAMM93_WARNINGS})
    add_test(NAME unit COMMAND test_damm93 ${CMAKE_CURRENT_SOURCE_DIR}/tests/vectors.txt)
    add_test(NAME cli
        COMMAND ${CMAKE_COMMAND} -DDAMM93=$<TARGET_FILE:damm93>
                -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_cli.cmake)
endif()
