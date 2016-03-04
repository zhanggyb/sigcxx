file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(testDerived ${sources} ${headers})
target_link_libraries(testDerived cppevent gtest)
