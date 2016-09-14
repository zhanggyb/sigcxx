file(GLOB Source_Files "*.cpp")
file(GLOB Header_Files "*.hpp")

add_library (common ${Header_Files} ${Source_Files})
