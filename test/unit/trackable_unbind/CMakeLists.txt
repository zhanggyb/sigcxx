file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(test_trackable_unbind ${sources} ${headers})
target_link_libraries(test_trackable_unbind sigcxx gtest common)
