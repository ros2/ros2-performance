cmake_minimum_required(VERSION 3.28)
project(performance_test_factory)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

find_package(ament_cmake REQUIRED)

find_package(rcpputils REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(performance_test REQUIRED)

## Build external dependencies
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(external/json)

set(LIBRARY_DEPENDENCIES
  rcpputils
  rclcpp
  rclcpp_lifecycle
  performance_test
)

set(LIBRARY_SRC
  src/cli_options.cpp
  src/factory.cpp
  src/load_plugins.cpp
  src/names_utilities.cpp
)

set(LIBRARY_NAME ${PROJECT_NAME})
add_library(${LIBRARY_NAME} SHARED ${LIBRARY_SRC})
target_include_directories(
  ${LIBRARY_NAME}
  PUBLIC
    external/cxxopts/include
    include
    ${performance_test_INCLUDE_DIRS}
)

target_link_libraries(${LIBRARY_NAME}
  rcpputils::rcpputils
  rclcpp::rclcpp
  rclcpp_lifecycle::rclcpp_lifecycle
  ${performance_test_LIBRARIES}
  nlohmann_json::nlohmann_json
)

install(TARGETS
  ${LIBRARY_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  include/
  external/json/single_include/
  external/cxxopts/include/
  DESTINATION include/
)

##
## build the unit tests
##
if(BUILD_TESTING)
  add_subdirectory(test)

  # Linters
  find_package(ament_cmake_cppcheck REQUIRED)
  find_package(ament_cmake_cpplint REQUIRED)
  find_package(ament_cmake_uncrustify REQUIRED)
  find_package(ament_cmake_xmllint REQUIRED)

  set(_factory_lint_dirs include src test)
  ament_cppcheck(${_factory_lint_dirs} LANGUAGE C++)
  ament_cpplint(${_factory_lint_dirs})
  ament_uncrustify(${_factory_lint_dirs} LANGUAGE C++)
  ament_xmllint()
endif()

ament_export_include_directories(include)
ament_export_libraries(${LIBRARY_NAME})
ament_export_dependencies(${LIBRARY_DEPENDENCIES})

ament_package()
