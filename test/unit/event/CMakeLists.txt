file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(testEvent ${sources} ${headers})
target_link_libraries(testEvent cppevent gtest)
