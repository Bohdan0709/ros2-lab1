#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;
using CallbackReturn = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

class ManagedTalker : public rclcpp_lifecycle::LifecycleNode
{
public:
    explicit ManagedTalker(const std::string &node_name)
        : rclcpp_lifecycle::LifecycleNode(node_name), count_(0)
    {
        RCLCPP_INFO(get_logger(), "Створено ManagedTalker (стан: Unconfigured)");
    }

    CallbackReturn on_configure(const rclcpp_lifecycle::State &) override
    {
        RCLCPP_INFO(get_logger(), "on_configure() -> створюємо паблішер і таймер");
        pub_ = this->create_publisher<std_msgs::msg::String>("lifecycle_chatter", 10);
        timer_ = this->create_wall_timer(1s, std::bind(&ManagedTalker::publish_msg, this));
        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_activate(const rclcpp_lifecycle::State &state) override
    {
        LifecycleNode::on_activate(state);
        RCLCPP_INFO(get_logger(), "on_activate() -> активуємо публікацію");
        pub_->on_activate();
        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_deactivate(const rclcpp_lifecycle::State &state) override
    {
        LifecycleNode::on_deactivate(state);
        RCLCPP_INFO(get_logger(), "on_deactivate() -> призупиняємо публікацію");
        pub_->on_deactivate();
        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_cleanup(const rclcpp_lifecycle::State &) override
    {
        RCLCPP_INFO(get_logger(), "on_cleanup() -> звільняємо ресурси");
        timer_.reset();
        pub_.reset();
        return CallbackReturn::SUCCESS;
    }

    CallbackReturn on_shutdown(const rclcpp_lifecycle::State &) override
    {
        RCLCPP_INFO(get_logger(), "on_shutdown() -> завершення роботи");
        timer_.reset();
        pub_.reset();
        return CallbackReturn::SUCCESS;
    }

private:
    void publish_msg()
    {
        if (!pub_ || !pub_->is_activated())
        {
            return;
        }
        auto msg = std_msgs::msg::String();
        msg.data = "Lifecycle повідомлення #" + std::to_string(++count_);
        RCLCPP_INFO(get_logger(), "Публікація: '%s'", msg.data.c_str());
        pub_->publish(msg);
    }

    std::shared_ptr<rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::String>> pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    size_t count_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::executors::SingleThreadedExecutor exe;
    auto node = std::make_shared<ManagedTalker>("managed_talker");
    exe.add_node(node->get_node_base_interface());
    exe.spin();
    rclcpp::shutdown();
    return 0;
}