cmake_minimum_required(VERSION 3.0)

project(MonkeyInterpreter)

set(CMAKE_CXX_STANDARD 20)

# Enable code coverage
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

# Find Boost libraries
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

# Add the Monkey Interpreter library
add_library(MonkeyInterpreter STATIC
    lexer/lexer.cpp
    lexer/token.cpp
    parser/ast.cpp
    parser/parser.cpp
    eval/object.cpp
    eval/evaluator.cpp
)

# Link the MonkeyInterpreter target with gcov
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(MonkeyInterpreter gcov)
endif()

# Find Git
find_package(Git)

# Get the version string using Git
execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags 
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Replace the VERSION macro in version.hpp.in with the actual version string
configure_file(${CMAKE_SOURCE_DIR}/version.hpp.in ${CMAKE_SOURCE_DIR}/version.hpp @ONLY)

add_executable(MonkeyRepl repl/repl.cpp)
# Add the version.hpp file to the MonkeyInterpreter library
target_include_directories(MonkeyRepl PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(MonkeyRepl MonkeyInterpreter)

enable_testing()

set(TESTS_SRC 
    tests/lexer_test.cpp 
    tests/parser_test.cpp
    tests/ast_test.cpp
    tests/evaluator_test.cpp
    )
# Add the Monkey Interpreter test executable
add_executable(MonkeyInterpreterTest ${TESTS_SRC})

# Link the Monkey Interpreter test executable with Boost libraries
target_link_libraries(MonkeyInterpreterTest MonkeyInterpreter ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

#add_test(NAME MonkeyInterpreterTest COMMAND MonkeyInterpreterTest)
# Add all the ADD_TEST for each test
foreach (TEST_SRC ${TESTS_SRC})
  get_filename_component (SRC_NAME ${TEST_SRC} NAME_WE)
  file(READ "${TEST_SRC}" SOURCE_FILE_CONTENTS)
    string(REGEX MATCHALL "BOOST_AUTO_TEST_CASE\\( *([A-Za-z_0-9]+) *\\)" 
           FOUND_TESTS ${SOURCE_FILE_CONTENTS})

    foreach(HIT ${FOUND_TESTS})
        string(REGEX REPLACE ".*\\( *([A-Za-z_0-9]+) *\\).*" "\\1" TEST_NAME ${HIT})

        add_test(NAME "${SRC_NAME}.${TEST_NAME}" 
                 COMMAND MonkeyInterpreterTest
                 --run_test=${TEST_NAME} --catch_system_error=yes)
    endforeach()
endforeach ()
