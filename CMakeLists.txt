cmake_minimum_required(VERSION 3.16)

project(damm95
    VERSION 1.0.0
    DESCRIPTION "Base95 implementation of the Damm check digit algorithm"
    LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(MSVC)
    set(DAMM95_WARNINGS /W4)
else()
    set(DAMM95_WARNINGS -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

add_library(damm95lib src/damm95.c)
target_include_directories(damm95lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_compile_options(damm95lib PRIVATE ${DAMM95_WARNINGS})
set_target_properties(damm95lib PROPERTIES OUTPUT_NAME damm95)

add_executable(damm95 src/main.c)
target_link_libraries(damm95 PRIVATE damm95lib)
target_compile_options(damm95 PRIVATE ${DAMM95_WARNINGS})
target_compile_definitions(damm95 PRIVATE DAMM95_VERSION="${PROJECT_VERSION}")

include(GNUInstallDirs)
install(TARGETS damm95 damm95lib)
install(FILES include/damm95.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CTest)
if(BUILD_TESTING)
    add_executable(test_damm95 tests/test_damm95.c)
    target_link_libraries(test_damm95 PRIVATE damm95lib)
    target_compile_options(test_damm95 PRIVATE ${DAMM95_WARNINGS})
    add_test(NAME unit COMMAND test_damm95 ${CMAKE_CURRENT_SOURCE_DIR}/tests/vectors.txt)
    add_test(NAME cli
        COMMAND ${CMAKE_COMMAND} -DDAMM95=$<TARGET_FILE:damm95>
                -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_cli.cmake)
endif()
