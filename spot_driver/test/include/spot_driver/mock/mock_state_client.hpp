// Copyright (c) 2024 Boston Dynamics AI Institute LLC. All rights reserved.

#pragma once

#include <gmock/gmock.h>

#include <spot_driver/api/state_client_interface.hpp>

#include <string>

namespace spot_ros2::test {
class MockStateClient : public StateClientInterface {
 public:
  MOCK_METHOD((tl::expected<RobotState, std::string>), getRobotState, (const std::string& preferred_odom_frame),
              (override));
};
}  // namespace spot_ros2::test
