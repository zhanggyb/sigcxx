file(GLOB sources "*.cpp")

add_executable(testDerived ${sources})
target_link_libraries(testDerived cppevent gtest)
