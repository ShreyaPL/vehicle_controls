# Source from bash: source scripts/activate.bash
vc_project_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source /opt/ros/humble/setup.bash
source "$vc_project_root/.venv/bin/activate"
unset vc_project_root
