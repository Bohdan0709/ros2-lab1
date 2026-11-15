#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "lifecycle_msgs/msg/transition.hpp"
#include "lifecycle_msgs/srv/change_state.hpp"

using namespace std::chrono_literals;

class LifecycleSupervisor : public rclcpp::Node
{
public:
    LifecycleSupervisor() : Node("lifecycle_supervisor") {}

    bool change_state(const std::string &target_node, std::uint8_t transition_id)
    {
        auto service_name = "/" + target_node + "/change_state";
        auto client = this->create_client<lifecycle_msgs::srv::ChangeState>(service_name);

        if (!client->wait_for_service(5s))
        {
            RCLCPP_ERROR(get_logger(), "Сервіс %s недоступний!", service_name.c_str());
            return false;
        }

        auto request = std::make_shared<lifecycle_msgs::srv::ChangeState::Request>();
        request->transition.id = transition_id;

        auto future = client->async_send_request(request);
        if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), future, 5s) !=
            rclcpp::FutureReturnCode::SUCCESS)
        {
            RCLCPP_ERROR(get_logger(), "Помилка виклику %s", service_name.c_str());
            return false;
        }

        bool ok = future.get()->success;
        RCLCPP_INFO(get_logger(), "Перехід для '%s' (id=%d): %s",
                    target_node.c_str(), transition_id, ok ? "УСПІШНО" : "ВІДХИЛЕНО");
        return ok;
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto supervisor = std::make_shared<LifecycleSupervisor>();

    std::vector<std::string> nodes = {"managed_listener", "managed_talker"};

    for (const auto &n : nodes)
    {
        supervisor->change_state(n, lifecycle_msgs::msg::Transition::TRANSITION_CONFIGURE);
    }
    for (const auto &n : nodes)
    {
        supervisor->change_state(n, lifecycle_msgs::msg::Transition::TRANSITION_ACTIVATE);
    }

    rclcpp::shutdown();
    return 0;
}