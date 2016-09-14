file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(testDisconnectOnFire ${sources} ${headers})
target_link_libraries(testDisconnectOnFire cppevent gtest common)
