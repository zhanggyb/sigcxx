file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(testTrackable ${sources} ${headers})
target_link_libraries(testTrackable cppevent gtest)
