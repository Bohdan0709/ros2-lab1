#include "lab1_nodes/my_component.hpp"
#include "rclcpp_components/register_node_macro.hpp"

namespace lab1_ns
{

    MyComponent::MyComponent(const rclcpp::NodeOptions &options)
        : Node("my_component_node", options)
    {
        RCLCPP_INFO(this->get_logger(), "Вузол-компонент MyComponent успішно завантажено!");
    }

} // namespace lab1_ns

RCLCPP_COMPONENTS_REGISTER_NODE(lab1_ns::MyComponent)