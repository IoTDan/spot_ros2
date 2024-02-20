// Copyright (c) 2024 Boston Dynamics AI Institute LLC. All rights reserved.

#pragma once

#include <gmock/gmock.h>
#include <spot_driver/api/time_sync_api.hpp>
#include <string>

namespace spot_ros2::test {
class MockTimeSyncApi : public TimeSyncApi {
 public:
  MOCK_METHOD((tl::expected<builtin_interfaces::msg::Time, std::string>), convertRobotTimeToLocalTime,
              (const google::protobuf::Timestamp& robot_timestamp), (override));
  MOCK_METHOD((tl::expected<google::protobuf::Duration, std::string>), getClockSkew, (), (override));
};
}  // namespace spot_ros2::test