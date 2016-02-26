file(GLOB sources "*.cpp")

add_executable(testPerformance ${sources})
target_link_libraries(testPerformance cppevent gtest)
