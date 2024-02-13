// Copyright (c) 2024 Boston Dynamics AI Institute LLC. All rights reserved.

#include <gmock/gmock.h>

#include <spot_driver/api/spot_api.hpp>
#include <spot_driver/robot_state/state_publisher_node.hpp>

#include <spot_driver/fake/fake_parameter_interface.hpp>
#include <spot_driver/mock/mock_logger_interface.hpp>
#include <spot_driver/mock/mock_node_interface.hpp>
#include <spot_driver/mock/mock_spot_api.hpp>
#include <spot_driver/mock/mock_state_client.hpp>
#include <spot_driver/mock/mock_tf_interface.hpp>
#include <spot_driver/mock/mock_timer_interface.hpp>

#include <rclcpp/node.hpp>
#include <rclcpp/node_options.hpp>

#include <exception>
#include <memory>
#include <tl_expected/expected.hpp>

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

namespace spot_ros2::test {

class MockStateMiddlewareHandle : public StatePublisher::MiddlewareHandle {
 public:
  MOCK_METHOD(void, publishRobotState, (const RobotState& robot_state), (override));
};

class StatePublisherNodeTest : public ::testing::Test {
 public:
  void SetUp() override {
    mock_node_interface = std::make_unique<MockNodeInterface>();

    fake_parameter_interface = std::make_unique<FakeParameterInterface>();
    mock_logger_interface = std::make_unique<spot_ros2::test::MockLoggerInterface>();
    mock_tf_interface = std::make_unique<spot_ros2::test::MockTfInterface>();
    mock_timer_interface = std::make_unique<spot_ros2::test::MockTimerInterface>();

    mock_spot_api = std::make_unique<spot_ros2::test::MockSpotApi>();
    mock_middleware_handle = std::make_unique<MockStateMiddlewareHandle>();
  }

  std::unique_ptr<MockNodeInterface> mock_node_interface;
  std::unique_ptr<FakeParameterInterface> fake_parameter_interface;
  std::unique_ptr<MockLoggerInterface> mock_logger_interface;
  std::unique_ptr<MockTfInterface> mock_tf_interface;
  std::unique_ptr<MockTimerInterface> mock_timer_interface;

  std::unique_ptr<MockSpotApi> mock_spot_api;
  std::unique_ptr<MockStateMiddlewareHandle> mock_middleware_handle;
};

TEST_F(StatePublisherNodeTest, ConstructionSuccessful) {
  // GIVEN a MiddlewareInterface and a SpotApi
  // GIVEN all steps to connect to the robot will succeed
  {
    InSequence seq;
    // THEN createRobot is called
    EXPECT_CALL(*mock_spot_api, createRobot).Times(1);
    // THEN we authenticate with the robot
    EXPECT_CALL(*mock_spot_api, authenticate).Times(1);
    // THEN we access the robot state client interface
    EXPECT_CALL(*mock_spot_api, stateClientInterface).Times(1);
    // THEN no error messages are logged
    EXPECT_CALL(*mock_logger_interface, logError).Times(0);
  }

  // WHEN constructing a StatePublisherNodeTest
  EXPECT_NO_THROW(StatePublisherNode(std::move(mock_node_interface), std::move(mock_spot_api),
                                     std::move(mock_middleware_handle), std::move(fake_parameter_interface),
                                     std::move(mock_logger_interface), std::move(mock_tf_interface),
                                     std::move(mock_timer_interface)));
}

TEST_F(StatePublisherNodeTest, ConstructionFailedCreateRobotFailure) {
  // GIVEN MiddlewareInterface and a SpotApi
  {
    InSequence seq;
    // GIVEN creating the interface to the robot will fail
    // THEN createRobot is called
    EXPECT_CALL(*mock_spot_api, createRobot).Times(1).WillOnce(Return(tl::make_unexpected("Create Robot Failed")));
    // THEN an error message is logged
    EXPECT_CALL(*mock_logger_interface, logError).Times(1);
    // THEN we do not attempt to authenticate with the robot
    EXPECT_CALL(*mock_spot_api, authenticate).Times(0);
    // THEN we do not access the robot state client interface
    EXPECT_CALL(*mock_spot_api, stateClientInterface).Times(0);
  }

  // WHEN constructing a StatePublisherNodeTest
  // THEN the constructor throws
  EXPECT_THROW(
      StatePublisherNode(std::move(mock_node_interface), std::move(mock_spot_api), std::move(mock_middleware_handle),
                         std::move(fake_parameter_interface), std::move(mock_logger_interface),
                         std::move(mock_tf_interface), std::move(mock_timer_interface)),
      std::exception);
}

TEST_F(StatePublisherNodeTest, ConstructionFailedAuthenticateFailure) {
  // GIVEN a MiddlewareInterface and a SpotApi
  {
    InSequence seq;
    // THEN createRobot is called
    EXPECT_CALL(*mock_spot_api, createRobot).Times(1);
    // GIVEN authentication will fail
    // THEN we attempt to authenticate with the robot
    EXPECT_CALL(*mock_spot_api, authenticate).Times(1).WillOnce(Return(tl::make_unexpected("Authenication Failed")));
    // THEN an error message is logged
    EXPECT_CALL(*mock_logger_interface, logError).Times(1);
    // THEN we do not access the robot state client interface
    EXPECT_CALL(*mock_spot_api, stateClientInterface).Times(0);
  }

  // WHEN constructing a StatePublisherNodeTest
  // THEN the constructor throws
  EXPECT_THROW(
      StatePublisherNode(std::move(mock_node_interface), std::move(mock_spot_api), std::move(mock_middleware_handle),
                         std::move(fake_parameter_interface), std::move(mock_logger_interface),
                         std::move(mock_tf_interface), std::move(mock_timer_interface)),
      std::exception);
}

}  // namespace spot_ros2::test
