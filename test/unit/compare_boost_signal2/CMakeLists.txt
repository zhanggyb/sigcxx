file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

find_package(Boost COMPONENTS signals)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  set(LIBS ${LIBS} ${Boost_LIBRARIES})
  add_definitions(-DUSE_BOOST_SIGNALS)
  message(STATUS "boost lib dir: ${Boost_LIBRARY_DIRS}")
endif()

add_executable(test_compare_boost_signal2 ${sources} ${headers})
target_link_libraries(test_compare_boost_signal2 sigcxx gtest common ${LIBS})
