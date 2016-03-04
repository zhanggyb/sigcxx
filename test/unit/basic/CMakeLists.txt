file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(testBasic ${sources} ${headers})
target_link_libraries(testBasic cppevent gtest)
