find_package(GTest REQUIRED)

# TODO: Add source files needed for tests
add_executable(TextTest main.cpp tests.cpp)

target_link_libraries(TextTest GTest::gtest GTest::gtest_main pthread)

target_include_directories(TextTest PRIVATE ${CMAKE_SOURCE_DIR}/include)

gtest_discover_tests(TextTest PROPERTIES ENVIRONMENT "GTEST_COLOR=1")
