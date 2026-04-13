/*********************************************************************
 * Copyright (c) 2025, SoftBank Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ********************************************************************/
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"

// Simple node for forwarding pointcloud data (In order to view pointcloud from PCs that are not the host PC)
class PointCloudForwarder : public rclcpp::Node
{
public:
  PointCloudForwarder() : Node("pointcloud_forwarder")
  {
    // Declare QoS parameters with defaults
    this->declare_parameter<std::string>("qos_history", "keep_last");
    this->declare_parameter<int>("qos_depth", 10);
    this->declare_parameter<std::string>("qos_reliability", "best_effort");
    this->declare_parameter<std::string>("qos_durability", "volatile");

    // Get QoS parameters
    std::string qos_history = this->get_parameter("qos_history").as_string();
    int qos_depth = this->get_parameter("qos_depth").as_int();
    std::string qos_reliability = this->get_parameter("qos_reliability").as_string();
    std::string qos_durability = this->get_parameter("qos_durability").as_string();

    // Build QoS profile
    rclcpp::QoS qos_profile(qos_depth);

    // Set history policy
    if (qos_history == "keep_all") {
      qos_profile.keep_all();
    } else {
      qos_profile.keep_last(qos_depth);
    }

    // Set reliability
    if (qos_reliability == "reliable") {
      qos_profile.reliable();
    } else {
      qos_profile.best_effort();
    }

    // Set durability
    if (qos_durability == "transient_local") {
      qos_profile.transient_local();
    } else {
      qos_profile.durability_volatile();
    }

    RCLCPP_INFO(this->get_logger(), "Using QoS: reliability=%s, durability=%s, depth=%d",
                qos_reliability.c_str(), qos_durability.c_str(), qos_depth);

    // Subscriber to original pointcloud with QoS
    subscription_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
      "/livox/lidar", qos_profile, std::bind(&PointCloudForwarder::pointcloud_callback, this, std::placeholders::_1));

    // Publisher to forward topic with QoS
    publisher_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("/forwarded_pointcloud", qos_profile);

    RCLCPP_INFO(this->get_logger(), "PointCloudForwarder initialized.");
  }

private:
  void pointcloud_callback(const sensor_msgs::msg::PointCloud2::SharedPtr msg) { publisher_->publish(*msg); }

  rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr subscription_;
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr publisher_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PointCloudForwarder>());
  rclcpp::shutdown();
  return 0;
}
