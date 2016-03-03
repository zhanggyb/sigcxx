file(GLOB sources "*.cpp")

add_executable(testDisconnectOnFire ${sources})
target_link_libraries(testDisconnectOnFire cppevent gtest)
