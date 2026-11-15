#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "std_msgs/msg/string.hpp"

using CallbackReturn = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

class ManagedListener : public rclcpp_lifecycle::LifecycleNode
{
public:
    explicit ManagedListener(const std::string &node_name)
        : rclcpp_lifecycle::LifecycleNode(node_name), is_active_(false)
    {
        RCLCPP_INFO(get_logger(), "Створено ManagedListener (стан: Unconfigured)");
    }

    CallbackReturn on_configure(const rclcpp_lifecycle::State &) override
    {
        RCLCPP_INFO(get_logger(), "on_configure() -> створюємо підписку");
        sub_ = this->create_subscription<std_msgs::msg::String>(
            "lifecycle_chatter", 10,
            std::bind(&ManagedListener::topic_callback, this, std::placeholders::_1));
        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_activate(const rclcpp_lifecycle::State &state) override
    {
        LifecycleNode::on_activate(state);
        RCLCPP_INFO(get_logger(), "on_activate() -> вузол активний, слухаємо топік");
        is_active_ = true;
        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_deactivate(const rclcpp_lifecycle::State &state) override
    {
        LifecycleNode::on_deactivate(state);
        RCLCPP_INFO(get_logger(), "on_deactivate() -> ігноруємо вхідні повідомлення");
        is_active_ = false;
        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_cleanup(const rclcpp_lifecycle::State &) override
    {
        RCLCPP_INFO(get_logger(), "on_cleanup() -> видаляємо підписку");
        is_active_ = false;
        sub_.reset();
        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_shutdown(const rclcpp_lifecycle::State &) override
    {
        RCLCPP_INFO(get_logger(), "on_shutdown() -> завершення роботи");
        is_active_ = false;
        sub_.reset();
        return CallbackReturn::SUCCESS;
    }

private:
    void topic_callback(const std_msgs::msg::String::SharedPtr msg)
    {
        if (!is_active_)
        {
            return;
        }
        RCLCPP_INFO(get_logger(), "Отримано: '%s'", msg->data.c_str());
    }

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
    bool is_active_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::executors::SingleThreadedExecutor exe;
    auto node = std::make_shared<ManagedListener>("managed_listener");
    exe.add_node(node->get_node_base_interface());
    exe.spin();
    rclcpp::shutdown();
    return 0;
}