file(GLOB sources "*.cpp")

add_executable(testDelegate ${sources})
target_link_libraries(testDelegate cppevent gtest)
