file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(testPerformance ${sources} ${headers})
target_link_libraries(testPerformance cppevent gtest)
