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
    // Subscriber to original pointcloud
    subscription_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
      "/livox/lidar", 10, std::bind(&PointCloudForwarder::pointcloud_callback, this, std::placeholders::_1));

    // Publisher to forward topic
    publisher_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("/forwarded_pointcloud", 10);

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
