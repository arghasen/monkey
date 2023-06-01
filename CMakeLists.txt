cmake_minimum_required(VERSION 3.0)

project(MonkeyInterpreter)

set(CMAKE_CXX_STANDARD 20)

# Find Boost libraries
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

# Add the Monkey Interpreter library
add_library(MonkeyInterpreter STATIC
    lexer/lexer.cpp
    lexer/token.cpp
)

add_executable(MonkeyRepl main.cpp)
target_link_libraries(MonkeyRepl MonkeyInterpreter)

enable_testing()

set(TESTS_SRC tests/lexer_test.cpp)
# Add the Monkey Interpreter test executable
add_executable(MonkeyInterpreterTest ${TESTS_SRC})

# Link the Monkey Interpreter test executable with Boost libraries
target_link_libraries(MonkeyInterpreterTest MonkeyInterpreter ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

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