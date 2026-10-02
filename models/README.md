# Lane-turn visualization scene

Open `lane_turn.xml` in MuJoCo; it includes `vehicle.xml`.

## Geometry and coordinates

All lengths are meters and angles are radians. World +z is up. The vehicle initially
faces +x; positive yaw turns left toward +y.

| Item | Value |
|---|---|
| First straight | (0, 0) to (30, 0) |
| Left turn | Circle centered at (30, 30), radius 30, angle -pi/2 to 0 |
| Final straight | (60, 30) to (60, 60) |
| Lane width | 3.6 m |
| Total centerline length | 60 + 15*pi, approximately 107.12 m |
| Vehicle body | 4.5 m long, 1.8 m wide |
| Wheelbase / wheel-center track | 2.8 m / 1.5 m |
| Initial rear axle ground projection | (5, 0), yaw 0 |

White lines mark lane boundaries. The dashed yellow line is a learning reference
centerline, not a divider between two traffic lanes. Green/red markers show path start/end.
The curved road is a polygonal approximation at 1.5-degree intervals. Your future
reference path should use the analytical geometry above rather than reading rendered pixels.
Straight/arc joins are tangent-continuous but have curvature jumps; smoothing is a later exercise.

## Vehicle pose interface for your implementation

The `vehicle` body is a MuJoCo mocap body, with no generalized coordinates or actuators.
Its origin corresponds to the rear axle center projected onto the road. Root height is
0.025 m to place the visual wheels above the road. Body forward is local +x.

Your future C++ program can find the body by name, read its `body_mocapid`, and write the
corresponding `mjData.mocap_pos` and `mjData.mocap_quat` entries. MuJoCo quaternions use
(w, x, y, z) order. Update forward kinematics before displaying the new pose.
Do not assume the mocap ID will remain zero if you later add more objects.

The wheels are fixed visual geometry; steering and rolling are not animated. Collision
response is disabled, and mass, tires, brakes, suspension and actuator dynamics are not
modeled. Pressing play advances time but does not drive the vehicle. These files provide
only the requested scene and vehicle appearance, leaving the motion model and controls to you.

The fixed `overview` camera provides a top-down view of the complete road. The default
free camera is angled; use the viewer camera selector for `overview`, or zoom in on the car.
