file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(testDelegate ${sources} ${headers})
target_link_libraries(testDelegate cppevent gtest)
