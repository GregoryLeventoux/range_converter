#include "rclcpp/rclcpp.hpp"
#include "anchor_msgs/msg/range_with_covariance.hpp"
#include "range_msgs/msg/range.hpp"

#include <cmath>


class Converter : public rclcpp::Node
{
  public:
    Converter()
    : Node("converter")
    {

        subscriber = this->create_subscription<anchor_msgs::msg::RangeWithCovariance>(
                    "/r2d2/ranges", 10,  std::bind(&Converter::callback, this, std::placeholders::_1));

        for(int i=0; i<4; i++)
        {
            std::string topic_name = "/range";
            topic_name.append(std::to_string(i));
            publishers.push_back(this->create_publisher<range_msgs::msg::Range>(topic_name,10));
        }
    }


  private:

    void callback(const anchor_msgs::msg::RangeWithCovariance::SharedPtr msg)
    {
        range_msgs::msg::Range range_converted;

        range_converted.header.frame_id = msg->header.frame_id;
        range_converted.header.stamp= msg->header.stamp;
        range_converted.range  = pow(msg->range,2);
        range_converted.covariance = (range_converted.range)*(msg->range);
        int number = (msg->child_frame_id).back() - 48 - 1;

        publishers[number]->publish(range_converted);
    }

    rclcpp::Subscription<anchor_msgs::msg::RangeWithCovariance>::SharedPtr subscriber;
    std::vector<rclcpp::Publisher<range_msgs::msg::Range>::SharedPtr> publishers;
};



int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Converter>());
  rclcpp::shutdown();
  return 0;
}
