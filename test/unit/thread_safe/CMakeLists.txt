file(GLOB sources "*.cpp")

add_executable(testThreadSafe ${sources})
target_link_libraries(testThreadSafe cppevent gtest)
