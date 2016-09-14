file(GLOB sources "*.cpp")
file(GLOB headers "*.hpp")

find_package(Boost COMPONENTS signals)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  set(LIBS ${LIBS} ${Boost_LIBRARIES})
  add_definitions(-DUSE_BOOST_SIGNALS)
  message(STATUS "boost lib dir: ${Boost_LIBRARY_DIRS}")
endif()

add_executable(testPerformance ${sources} ${headers})
target_link_libraries(testPerformance cppevent gtest common ${LIBS})
