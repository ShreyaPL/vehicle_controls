"""Replay C++ CSV output in MuJoCo; visualization only, no Python controller."""
import argparse
import csv
from pathlib import Path
import time

import mujoco
import mujoco.viewer

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('csv', type=Path)
args = parser.parse_args()
model = mujoco.MjModel.from_xml_path(str(Path(__file__).resolve().parents[1] / 'models/truck.xml'))
data = mujoco.MjData(model)
with args.csv.open() as source, mujoco.viewer.launch_passive(model, data) as viewer:
    start = time.monotonic()
    for row in csv.DictReader(source):
        if not viewer.is_running():
            break
        delay = float(row['time_s']) - (time.monotonic() - start)
        if delay > 0:
            time.sleep(delay)
        with viewer.lock():
            data.qpos[:] = [float(row['x_m']), float(row['y_m']), float(row['yaw_rad']),
                            float(row['trailer_yaw_rad']) - float(row['yaw_rad'])]
            data.time = float(row['time_s'])
            mujoco.mj_forward(model, data)
            viewer.cam.lookat[:] = data.xpos[1]
            viewer.cam.distance = 30
        viewer.sync()
