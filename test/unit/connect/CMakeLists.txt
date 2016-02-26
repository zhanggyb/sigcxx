file(GLOB sources "*.cpp")

add_executable(testConnect ${sources})
target_link_libraries(testConnect cppevent gtest)
