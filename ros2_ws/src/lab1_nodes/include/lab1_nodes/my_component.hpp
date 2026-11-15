#ifndef LAB1_NODES__MY_COMPONENT_HPP_
#define LAB1_NODES__MY_COMPONENT_HPP_

#include "rclcpp/rclcpp.hpp"

namespace lab1_ns
{

class MyComponent : public rclcpp::Node
{
public:
  explicit MyComponent(const rclcpp::NodeOptions & options);
};

}  // namespace lab1_ns

#endif  // LAB1_NODES__MY_COMPONENT_HPP_