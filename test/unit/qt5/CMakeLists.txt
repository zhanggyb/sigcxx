# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Core)

file(GLOB sources "*.cpp")

add_executable(testCompareQt5 ${sources})
target_link_libraries(testCompareQt5 cppevent gtest Qt5::Core)
