cmake_minimum_required(VERSION 3.20)
project(lab1_nodes)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(lifecycle_msgs REQUIRED)
find_package(std_msgs REQUIRED)

# Завдання 1.2: Збирання компонента
add_library(my_component SHARED
  src/my_component.cpp
)

target_include_directories(my_component PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(my_component PUBLIC
  rclcpp::rclcpp
  rclcpp_components::component
)

rclcpp_components_register_nodes(my_component "lab1_ns::MyComponent")

install(TARGETS my_component
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/
  DESTINATION include
)

# Завдання 1.3: Керовані вузли та супервізор
add_executable(managed_talker src/managed_talker.cpp)
target_link_libraries(managed_talker PUBLIC
  rclcpp::rclcpp
  rclcpp_lifecycle::rclcpp_lifecycle
  ${std_msgs_TARGETS}
)

add_executable(managed_listener src/managed_listener.cpp)
target_link_libraries(managed_listener PUBLIC
  rclcpp::rclcpp
  rclcpp_lifecycle::rclcpp_lifecycle
  ${std_msgs_TARGETS}
)

add_executable(lifecycle_supervisor src/lifecycle_supervisor.cpp)
target_link_libraries(lifecycle_supervisor PUBLIC
  rclcpp::rclcpp
  ${lifecycle_msgs_TARGETS}
)

install(TARGETS
  managed_talker
  managed_listener
  lifecycle_supervisor
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()