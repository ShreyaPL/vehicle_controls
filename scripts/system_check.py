"""Read-only compatibility inventory; run with .venv/bin/python."""
import importlib.metadata
import json
import platform
import shutil
import subprocess


def probe(command):
    try:
        result = subprocess.run(command, text=True, capture_output=True, timeout=15, check=False)
        return {'exit_code': result.returncode, 'output': (result.stdout + result.stderr).strip()}
    except (OSError, subprocess.TimeoutExpired) as error:
        return {'error': str(error)}


report = {'os': platform.platform(), 'architecture': platform.machine(),
          'python': platform.python_version(),
          'tools': {name: shutil.which(name) for name in ['g++', 'cmake', 'ninja', 'ros2', 'colcon', 'git']},
          'checks': {name: probe(command) for name, command in {
              'compiler': ['g++', '--version'], 'cmake': ['cmake', '--version'],
              'ros_rclcpp': ['ros2', 'pkg', 'prefix', 'rclcpp'],
              'ros_geometry_msgs': ['ros2', 'pkg', 'prefix', 'geometry_msgs'],
              'ros_nav_msgs': ['ros2', 'pkg', 'prefix', 'nav_msgs'],
          }.items()},
          'packages': {name: importlib.metadata.version(name) for name in
                       ['mujoco', 'numpy', 'clang-format', 'clang-tidy']}}
print(json.dumps(report, indent=2))
