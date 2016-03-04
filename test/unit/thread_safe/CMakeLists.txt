file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(testThreadSafe ${sources} ${headers})
target_link_libraries(testThreadSafe cppevent gtest)
