"""Local DDS integration test. Run with /usr/bin/python3 after sourcing ROS Humble."""
import os
from pathlib import Path
import signal
import subprocess
import time

import rclpy
from geometry_msgs.msg import TwistStamped
from nav_msgs.msg import Odometry
from rclpy.qos import qos_profile_sensor_data

root = Path(__file__).resolve().parents[1]
os.environ.setdefault('ROS_DOMAIN_ID', '87')
os.environ.setdefault('ROS_LOCALHOST_ONLY', '1')
os.environ['ROS_LOG_DIR'] = str(root / 'log')
process = None
node = None
rclpy.init()
try:
    node = rclpy.create_node('truck_integration_probe')
    publisher = node.create_publisher(TwistStamped, '/planning/command', qos_profile_sensor_data)
    samples = []
    subscription = node.create_subscription(
        Odometry, '/sim/odometry', lambda msg: samples.append(msg.twist.twist.linear.x), 10)
    process = subprocess.Popen([str(root / 'build/dev/truck_ros_sim')], cwd=root)
    deadline = time.monotonic() + 10
    while publisher.get_subscription_count() == 0 and time.monotonic() < deadline:
        if process.poll() is not None:
            raise RuntimeError('Simulation process exited before discovery')
        rclpy.spin_once(node, timeout_sec=0.02)
    if publisher.get_subscription_count() == 0:
        raise RuntimeError('DDS discovery timed out')
    command = TwistStamped()
    command.twist.linear.x = 2.0
    deadline = time.monotonic() + 3
    while time.monotonic() < deadline:
        publisher.publish(command)
        rclpy.spin_once(node, timeout_sec=0.01)
        time.sleep(0.01)
    if not samples or max(samples) < 1.0:
        raise RuntimeError('Vehicle did not accelerate through ROS command interface')
    count_before_dropout = len(samples)
    deadline = time.monotonic() + 2
    while time.monotonic() < deadline:
        rclpy.spin_once(node, timeout_sec=0.02)
    if len(samples) <= count_before_dropout or samples[-1] != 0.0:
        raise RuntimeError('Command dropout did not reach standstill')
    deadline = time.monotonic() + 0.5
    while time.monotonic() < deadline:
        publisher.publish(command)
        rclpy.spin_once(node, timeout_sec=0.01)
        time.sleep(0.01)
    if samples[-1] != 0.0 or process.poll() is not None:
        raise RuntimeError('Latched stop or simulation liveness check failed')
    print(f'PASS: {len(samples)} odometry samples; acceleration, timeout braking, stop latch verified')
finally:
    if process is not None and process.poll() is None:
        process.send_signal(signal.SIGINT)
        try:
            process.wait(timeout=5)
        except subprocess.TimeoutExpired:
            process.kill()
            process.wait()
    if node is not None:
        node.destroy_node()
    rclpy.shutdown()
