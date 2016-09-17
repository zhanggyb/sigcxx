file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(test_thread_safe ${sources} ${headers})
target_link_libraries(test_thread_safe sigcxx gtest)
