file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(testConnect ${sources} ${headers})
target_link_libraries(testConnect cppevent gtest)
