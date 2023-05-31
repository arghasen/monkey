cmake_minimum_required(VERSION 3.0)

project(MonkeyInterpreter)

set(CMAKE_CXX_STANDARD 20)

# Find Boost libraries
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(MonkeyRepl main.cpp)

# Add the Monkey Interpreter test executable
add_executable(MonkeyInterpreterTest tests/lexer_test.cpp)

# Link the Monkey Interpreter test executable with Boost libraries
target_link_libraries(MonkeyInterpreterTest ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

# Add the test to CTest
add_test(NAME MonkeyInterpreterTest COMMAND MonkeyInterpreterTest)