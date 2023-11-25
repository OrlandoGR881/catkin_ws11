#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>  // Include turtlesim/Pose for the Pose message type
#include <cstdlib>
#include <ctime>
#include <iomanip>

bool safezone = true;

void turtlePose(const turtlesim::Pose& msg) {
    ROS_INFO_STREAM(std::setprecision(2) << std::fixed
                    << "position=(" << msg.x << "," << msg.y << ")"
                    << " direction=" << msg.theta);

    // Check if the turtle is in the safe zone (5x5 square around the center)
    if (msg.x >= 3 && msg.x <= 8 && msg.y >= 3 && msg.y <= 8) {
        safezone = true;
    } else {
        safezone = false;
    }
}

int main(int argc, char** argv) {
    // Initialize the ROS system and become a node.
    ros::init(argc, argv, "safe_zone_vel_publisher1");
    ros::NodeHandle nh;

    // Create a publisher object.
    ros::Publisher pub = nh.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 1000);

    // Create a subscriber object.
    ros::Subscriber sub = nh.subscribe("turtle1/pose", 1000, &turtlePose);

    // Seed the random number generator.
    srand(time(0));

    // Loop at 2Hz until the node is shut down.
    ros::Rate rate(2);
    while (ros::ok()) {
        // Create and fill in the message. The other four
        // fields, which are ignored by turtlesim, default to 0.
        geometry_msgs::Twist msg;
            msg.linear.x = double(rand()) / double(RAND_MAX);
            msg.angular.z = 2 * double(rand()) / double(RAND_MAX) - 1;
        if (safezone) {
            // Publish a fixed linear velocity and a random angular velocity in the safe zone
            msg.linear.x = 1.0;
            msg.angular.z = 2 * double(rand()) / double(RAND_MAX) - 1;
        } else {
            // Publish random linear and angular velocities outside the safe zone
            msg.linear.x = double(rand()) / double(RAND_MAX);
            msg.angular.z = 2 * double(rand()) / double(RAND_MAX) - 1;
        }

        // Publish the message.
        pub.publish(msg);

        // Send a message to rosout with the details.
        ROS_INFO_STREAM("Sending random velocity command:"
                        << " linear=" << msg.linear.x
                        << " angular=" << msg.angular.z);

        // Wait until it's time for another iteration.
        rate.sleep();
    }

    return 0;
}