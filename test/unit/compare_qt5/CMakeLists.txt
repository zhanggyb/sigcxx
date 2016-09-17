# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Core)

file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

add_executable(test_compare_qt5 ${sources} ${headers})
target_link_libraries(test_compare_qt5 sigcxx gtest common Qt5::Core)
