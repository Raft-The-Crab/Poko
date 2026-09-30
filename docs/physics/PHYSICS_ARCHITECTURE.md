# Poko Physics Engine Architecture

## Overview

Poko Physics is a custom rigid-body physics engine designed for AAA/indie games. It provides deterministic simulation, efficient collision detection, a robust constraint solver, and advanced features like CCD, character controllers, and ragdolls.

**Target Quality:**
- 75% Indie-A (core systems fully functional)
- 25% AAA (advanced features for polish)

**Design Principles:**
- Production-quality from day one (no placeholders)
- Fine-grained translation units for incremental builds
- Thread-safe where applicable
- SIMD-optimized where beneficial
- Research-driven architecture (Box2D, Jolt, PhysX, Bullet, Havok)

---

## Phase 1: Physics Component (Built-in Component)

**Location:** `shared/engine/include/core/components/physics/`

The Physics Component is a data container attached to instances that stores physics properties. It does not perform simulation—that happens in the Physics Engine subsystem.

### Component Structure

```
physics/
├── physics.h                    # Main component class with all properties
├── math/                        # Physics-specific math types
│   ├── vector3.h               # 3D vector with physics operations
│   ├── matrix3x3.h             # 3x3 matrix for inertia tensors
│   ├── quaternion.h            # Quaternion for rotations
│   ├── matrix4x4.h             # 4x4 transform matrices
│   └── bounds.h                # AABB/OBB bounds
├── body_type.h                 # Body type enums (Static, Dynamic, Kinematic)
├── shape_type.h                # Shape type enums (Sphere, Box, Capsule, etc.)
└── material/                   # Material properties
    ├── friction.h              # Friction coefficient
    ├── restitution.h           # Bounciness
    └── density.h               # Mass density
```

### Source Files (Fine-Grained)

```
shared/engine/src/core/components/physics/
├── physics.cpp                 # Constructor and lifecycle
├── body_type.cpp               # Body type operations
├── shape_type.cpp              # Shape type operations
├── material/
│   ├── set_friction.cpp        # Friction setter with clamping
│   ├── set_restitution.cpp     # Restitution setter with clamping
│   └── set_density.cpp         # Density setter with clamping
├── mass/
│   ├── set_mass.cpp            # Mass setter with validation
│   ├── calculate_mass.cpp      # Calculate mass from density and shape
│   └── calculate_inertia.cpp   # Calculate inertia tensor
├── velocity/
│   ├── set_linear_velocity.cpp
│   ├── set_angular_velocity.cpp
│   ├── get_linear_velocity.cpp
│   └── get_angular_velocity.cpp
├── gravity/
│   ├── set_gravity_scale.cpp   # Per-body gravity multiplier
│   └── get_gravity_scale.cpp
├── collision/
│   ├── set_collision_layer.cpp
│   ├── set_collision_mask.cpp
│   └── should_collide.cpp     # Layer/mask collision check
├── sleeping/
│   ├── set_sleep_threshold.cpp
│   ├── set_awake.cpp
│   ├── set_sleep.cpp
│   └── is_sleeping.cpp
├── force/
│   ├── apply_force.cpp         # Apply force at center of mass
│   ├── apply_force_at_point.cpp
│   ├── apply_torque.cpp
│   ├── apply_impulse.cpp
│   └── apply_impulse_at_point.cpp
├── damping/
│   ├── set_linear_damping.cpp
│   ├── set_angular_damping.cpp
│   └── apply_damping.cpp
├── ccd/
│   ├── set_ccd_enabled.cpp
│   ├── set_ccd_motion_threshold.cpp
│   └── set_ccd_swept_sphere_radius.cpp
├── limits/
│   ├── set_max_linear_velocity.cpp
│   ├── set_max_angular_velocity.cpp
│   └── clamp_velocities.cpp
└── physics_registration.cpp    # Component registration
```

### Component Properties

**Body Properties:**
- Body type: Static, Dynamic, Kinematic
- Shape type: Sphere, Box, Capsule, Plane, ConvexHull, TriangleMesh
- Mass: Float with min/max limits
- Inertia tensor: 3x3 matrix (computed from shape and mass)
- Center of mass: Vector3 (computed from shape)

**Motion Properties:**
- Linear velocity: Vector3
- Angular velocity: Vector3
- Linear damping: Float (0-1)
- Angular damping: Float (0-1)
- Gravity scale: Float (default 1.0)

**Material Properties:**
- Friction: Float (0-1)
- Restitution: Float (0-1)
- Density: Float (for mass calculation)

**Collision Properties:**
- Collision layer: 32-bit mask
- Collision mask: 32-bit mask (layers to collide with)

**Sleeping Properties:**
- Sleep threshold: Float (velocity below which body can sleep)
- Sleep time threshold: Float (time below threshold before sleeping)
- Is sleeping: Boolean

**CCD Properties:**
- CCD enabled: Boolean
- CCD motion threshold: Float (velocity threshold for CCD)
- CCD swept sphere radius: Float

**Velocity Limits:**
- Max linear velocity: Float
- Max angular velocity: Float

---

## Phase 2: Physics Math Module

**Location:** `shared/engine/include/core/physics/math/`

Physics-specific math optimized for physics calculations. These are separate from Transform math to avoid conflicts and allow physics-specific optimizations.

### Math Types

```
math/
├── vector3.h                  # Physics 3D vector
├── vector3.cpp                # Vector operations
├── matrix3x3.h                # 3x3 matrix (inertia tensors)
├── matrix3x3.cpp              # Matrix operations
├── quaternion.h               # Quaternion for rotations
├── quaternion.cpp             # Quaternion operations
├── matrix4x4.h                # 4x4 transform matrices
├── matrix4x4.cpp              # Transform operations
├── bounds.h                   # AABB/OBB bounds
├── bounds.cpp                 # Bounds operations
└── constants.h                # Physics constants (epsilon, tolerance)
```

### Vector3 Operations

- Basic: +, -, *, /, dot, cross, length, normalize
- Physics-specific: lerp, clamp, distance, angle
- SIMD: Use SSE/AVX when available

### Matrix3x3 Operations

- Basic: +, -, *, transpose, inverse, determinant
- Physics-specific: diagonal extraction, rotation extraction
- Inertia: Compute inertia tensor from mass and shape

### Quaternion Operations

- Basic: +, -, *, conjugate, normalize, inverse
- Physics-specific: rotate vector, to euler, from euler
- Slerp: Spherical interpolation

### Bounds Operations

- AABB: min/max, center, extent, merge, contains, intersects
- OBB: center, axes, extents, transform, contains, intersects

---

## Phase 3: Collision Shapes

**Location:** `shared/engine/include/core/physics/collision/shapes/`

Collision shape definitions for broadphase and narrowphase collision detection.

### Shape Hierarchy

```
shapes/
├── shape.h                    # Base shape class
├── shape.cpp                  # Shape factory
├── sphere.h                   # Sphere shape
├── sphere.cpp                 # Sphere implementation
├── box.h                      # Box shape
├── box.cpp                    # Box implementation
├── capsule.h                  # Capsule shape
├── capsule.cpp                # Capsule implementation
├── plane.h                    # Infinite plane
├── plane.cpp                  # Plane implementation
├── convex_hull.h              # Convex hull
├── convex_hull.cpp            # Convex hull implementation
├── triangle_mesh.h            # Triangle mesh (static)
├── triangle_mesh.cpp          # Triangle mesh implementation
└── heightfield.h              # Heightfield (terrain)
    └── heightfield.cpp
```

### Shape Properties

**Common:**
- Type enum
- AABB bounds (computed from shape)
- Volume (for mass calculation)
- Center of mass

**Sphere:**
- Radius

**Box:**
- Half-extents (x, y, z)

**Capsule:**
- Radius
- Height (total height, not half)

**Plane:**
- Normal
- Distance from origin

**Convex Hull:**
- Vertices
- Faces
- Edges

**Triangle Mesh:**
- Vertices
- Indices
- BVH for acceleration

---

## Phase 4: Broadphase Collision Detection

**Location:** `shared/engine/include/core/physics/collision/broadphase/`

Broadphase quickly eliminates non-overlapping AABB pairs to reduce work for narrowphase.

### Algorithms

**Sweep and Prune (SAP):**
- Incremental sweep on sorted axes
- Best for scenes with many sleeping objects
- Poor for many moving objects
- 1-axis SAP (fastest, less accurate)
- 3-axis SAP (slower, more accurate)

**Dynamic AABB Tree (BVH):**
- Tree-based broadphase
- Good for dynamic scenes
- Active objects vs tree (Jolt approach)
- Trivially parallelizable

**Spatial Grid:**
- Uniform grid over world bounds
- Fast for uniform distribution
- Poor for clustered objects
- Fast updates

**Multi-Box Pruning (MBP):**
- Grid of SAP regions (PhysX approach)
- Best of both worlds
- Requires world bounds

### Structure

```
broadphase/
├── broadphase.h              # Broadphase interface
├── broadphase.cpp            # Broadphase factory
├── sweep_prune.h             # SAP implementation
├── sweep_prune.cpp
├── sweep_prune_1axis.h       # 1-axis SAP
├── sweep_prune_1axis.cpp
├── sweep_prune_3axis.h       # 3-axis SAP
├── sweep_prune_3axis.cpp
├── dynamic_aabb_tree.h       # BVH implementation
├── dynamic_aabb_tree.cpp
├── spatial_grid.h            # Grid implementation
├── spatial_grid.cpp
├── multi_box_pruning.h       # MBP implementation
├── multi_box_pruning.cpp
├── collision_pair.h          # Candidate pair struct
└── collision_pair.cpp       # Pair management
```

### Selection Strategy

**Adaptive Broadphase:**
- Select algorithm based on scene characteristics
- Metrics: object count, motion, distribution
- Switch algorithms at runtime
- Or: support multiple and let user choose

**Default:**
- Start with Dynamic AABB Tree (Jolt approach)
- It's simple, fast, and parallelizable

---

## Phase 5: Narrowphase Collision Detection

**Location:** `shared/engine/include/core/physics/collision/narrowphase/`

Narrowphase computes exact collision geometry and contact manifolds.

### Algorithms

**GJK (Gilbert-Johnson-Keerthi):**
- Distance between convex shapes
- Contact point generation
- Good for convex shapes

**SAT (Separating Axis Theorem):**
- Test separating axes
- Good for boxes, capsules
- Manifold generation

**Contact Manifolds:**
- Contact points, normals, penetration depths
- Feature tracking (face, edge, vertex)
- Warm starting for solver

### Structure

```
narrowphase/
├── narrowphase.h             # Narrowphase interface
├── narrowphase.cpp           # Narrowphase factory
├── gjk.h                     # GJK implementation
├── gjk.cpp
├── sat.h                     # SAT implementation
├── sat.cpp
├── contact_manifold.h        # Contact manifold
├── contact_manifold.cpp
├── contact_point.h           # Contact point struct
├── sphere_sphere.h           # Sphere-sphere collision
├── sphere_sphere.cpp
├── sphere_box.h              # Sphere-box collision
├── sphere_box.cpp
├── box_box.h                 # Box-box collision
├── box_box.cpp
├── capsule_capsule.h         # Capsule-capsule collision
├── capsule_capsule.cpp
├── convex_convex.h           # Convex-convex collision
└── convex_convex.cpp
```

### Contact Manifold

**Contact Point:**
- Position (world space)
- Normal (direction from A to B)
- Penetration depth
- Feature ID (for warm starting)

**Manifold:**
- Array of contact points (max 4 per pair)
- Body A and body B
- Friction and restitution from materials

---

## Phase 6: Constraint Solver

**Location:** `shared/engine/include/core/physics/solver/`

Constraint solver resolves contacts and joints using sequential impulses (Gauss-Seidel).

### Sequential Impulse Solver

**Algorithm (Erin Catto, Box2D):**
1. Compute relative velocity at contact
2. Compute jacobian (constraint gradient)
3. Compute effective mass (inverse mass matrix)
4. Compute lambda (impulse magnitude)
5. Clamp lambda to limits
6. Apply impulse to bodies
7. Iterate (usually 8-16 iterations)
8. Warm starting (reuse previous impulse)

**Sub-stepping:**
- Divide timestep into smaller steps
- More stable for high velocities
- More expensive

### Constraints

**Contact Constraint:**
- Normal constraint (prevent penetration)
- Friction constraint (tangential motion)
- Rolling friction constraint

**Joint Constraints:**
- Distance constraint (fixed distance)
- Hinge constraint (1 degree of freedom)
- Spherical constraint (ball and socket)
- Fixed constraint (fully rigid)
- Prismatic constraint (sliding)
- Generic constraint (custom limits)

### Structure

```
solver/
├── solver.h                  # Solver interface
├── solver.cpp                # Solver factory
├── sequential_impulse.h       # Sequential impulse solver
├── sequential_impulse.cpp
├── constraint.h              # Constraint base class
├── constraint.cpp
├── contact_constraint.h       # Contact constraint
├── contact_constraint.cpp
├── joint_distance.h          # Distance joint
├── joint_distance.cpp
├── joint_hinge.h             # Hinge joint
├── joint_hinge.cpp
├── joint_spherical.h         # Spherical joint
├── joint_spherical.cpp
├── joint_fixed.h             # Fixed joint
├── joint_fixed.cpp
├── joint_prismatic.h         # Prismatic joint
├── joint_prismatic.cpp
├── joint_generic.h           # Generic constraint
├── joint_generic.cpp
├── warm_starting.h           # Warm starting system
├── warm_starting.cpp
└── solver_config.h           # Solver configuration
```

### Solver Configuration

**Parameters:**
- Iteration count (velocity: 8-16, position: 4-8)
- Solver tolerance (velocity threshold)
- Warm starting factor (0-1)
- Baugur factor (stabilization)
- Split impulse (penetration correction)

---

## Phase 7: Physics World and Bodies

**Location:** `shared/engine/include/core/physics/world/`

Physics world manages simulation, bodies, and integration.

### Physics World

**Responsibilities:**
- Add/remove bodies
- Step simulation (fixed timestep)
- Manage gravity
- Broadphase integration
- Narrowphase integration
- Solver integration
- Sleeping management
- Island management

### Rigid Body

**Responsibilities:**
- Store state (position, rotation, velocity, mass)
- Apply forces/torques
- Update position/velocity (integration)
- Collision shape reference
- Material reference

### Integration

**Semi-Implicit Euler:**
```cpp
velocity += (force / mass) * dt
position += velocity * dt
```

- Preserves energy better than explicit Euler
- Standard in games (Box2D, Jolt, PhysX)
- Small phase error

**Velocity Verlet:**
- More accurate
- More expensive
- Option for high precision

### Structure

```
world/
├── world.h                   # Physics world
├── world.cpp                 # World implementation
├── rigid_body.h              # Rigid body
├── rigid_body.cpp            # Body implementation
├── body_state.h              # Body state struct
├── body_state.cpp
├── integration.h             # Integration methods
├── integration.cpp
├── island.h                  # Sleeping island
├── island.cpp                # Island management
├── world_config.h            # World configuration
└── world_config.cpp
```

### World Configuration

**Parameters:**
- Gravity (Vector3, default -9.81)
- Timestep (Float, default 1/60)
- Max substeps (Int, default 8)
- Broadphase type (Enum)
- Solver iterations (Int)
- Sleeping enabled (Boolean)

---

## Phase 8: Sleeping and Island Management

**Location:** `shared/engine/include/core/physics/sleeping/`

Sleeping reduces work for stable bodies by grouping them into islands.

### Sleeping Logic

**Sleep Threshold:**
- Bodies with velocity below threshold can sleep
- Threshold configurable per body

**Sleep Time:**
- Bodies must be below threshold for time before sleeping
- Time configurable per body

**Islands:**
- Group of connected bodies (contacts, joints)
- If any body in island wakes, all wake
- Parallelize islands across threads

### Structure

```
sleeping/
├── island.h                  # Island class
├── island.cpp
├── island_manager.h          # Island manager
├── island_manager.cpp
├── sleep_tracker.h           # Track sleep time
├── sleep_tracker.cpp
└── wake_policy.h             # Wake conditions
```

---

## Phase 9: Continuous Collision Detection (CCD)

**Location:** `shared/engine/include/core/physics/ccd/`

CCD prevents tunneling for fast-moving bodies.

### CCD Methods

**Speculative CCD:**
- Increase AABB based on motion
- Generate potential contacts
- Feed to solver
- Fast, but can cause ghost collisions

**Swept CCD:**
- Sweep shape from old to new position
- Compute time of impact (TOI)
- Move to TOI, then substep
- More accurate, more expensive
- Linear sweep (translational)
- Non-linear sweep (rotational + translational)

### Structure

```
ccd/
├── ccd.h                     # CCD interface
├── ccd.cpp                   # CCD factory
├── speculative_ccd.h         # Speculative CCD
├── speculative_ccd.cpp
├── swept_ccd.h               # Swept CCD
├── swept_ccd.cpp
├── time_of_impact.h          # TOI computation
├── time_of_impact.cpp
├── swept_sphere.h            # Swept sphere (fast path)
└── swept_sphere.cpp
```

### CCD Configuration

**Parameters:**
- CCD enabled (per body)
- Motion threshold (velocity to enable CCD)
- Swept sphere radius (for fast path)
- Max CCD substeps
- Linear vs non-linear

---

## Phase 10: Character Controller

**Location:** `shared/engine/include/core/physics/character/`

Character controller for player/NPC movement.

### Character Controller Types

**Kinematic Character Controller:**
- Movement computed outside physics
- Uses collision detection only
- Slide, step up/down
- More expensive, more control
- Collide and slide algorithm

**Dynamic Character Controller:**
- Modeled as rigid body
- Physics simulation computes movement
- Faster, less control
- Set velocity directly

### Structure

```
character/
├── character_controller.h    # Base controller
├── character_controller.cpp
├── kinematic_controller.h   # Kinematic controller
├── kinematic_controller.cpp
├── dynamic_controller.h      # Dynamic controller
├── dynamic_controller.cpp
├── capsule_shape.h          # Character capsule
├── capsule_shape.cpp
├── slide.h                   # Slide algorithm
├── slide.cpp
├── step_up.h                 # Step up algorithm
├── step_up.cpp
├── step_down.h               # Step down algorithm
└── step_down.cpp
```

### Character Controller Properties

**Kinematic:**
- Capsule shape (radius, height)
- Move speed
- Jump force
- Gravity
- Slope limit (max walkable slope)
- Step height (max step height)
- Collision layer/mask

**Dynamic:**
- Rigid body with locked rotation
- Capsule shape
- Velocity directly set
- Friction control

---

## Phase 11: Ragdoll System

**Location:** `shared/engine/include/core/physics/ragdoll/`

Ragdoll for articulated characters.

### Ragdoll Structure

**Skeleton:**
- Hierarchical bone structure
- Joint types between bones
- Rest poses

**Ragdoll:**
- Collection of rigid bodies (one per bone)
- Collection of constraints (joints between bones)
- Driven by animation or physics

### Stabilization

**Constraint Priorities:**
- Higher priority for root (hips)
- Lower priority for leaves (fingers)
- Improves solver convergence

**Mass Ratios:**
- Limit mass ratios between connected bodies
- Prevent numerical instability

### Structure

```
ragdoll/
├── ragdoll.h                 # Ragdoll class
├── ragdoll.cpp
├── ragdoll_settings.h        # Ragdoll blueprint
├── ragdoll_settings.cpp
├── skeleton.h                # Skeleton structure
├── skeleton.cpp
├── skeleton_pose.h           # Pose data
├── skeleton_pose.cpp
├── part.h                    # Single ragdoll part
├── part.cpp
├── additional_constraint.h   # Extra constraints
├── additional_constraint.cpp
├── stabilization.h           # Stabilization methods
├── stabilization.cpp
└── drive_to_pose.h           # Motor control
```

---

## Phase 12: Vehicle Physics

**Location:** `shared/engine/include/core/physics/vehicle/`

Vehicle physics for cars, bikes, etc.

### Vehicle Components

**Raycast Vehicle:**
- Wheels are raycasts
- Fast, simple
- Good for arcade games

**Constraint Vehicle:**
- Wheels are rigid bodies with suspension joints
- More realistic
- More expensive

### Structure

```
vehicle/
├── vehicle.h                 # Vehicle base
├── vehicle.cpp
├── raycast_vehicle.h         # Raycast vehicle
├── raycast_vehicle.cpp
├── constraint_vehicle.h      # Constraint vehicle
├── constraint_vehicle.cpp
├── wheel.h                   # Wheel
├── wheel.cpp
├── suspension.h              # Suspension
├── suspension.cpp
├── engine.h                  # Engine
├── engine.cpp
├── transmission.h            # Transmission
└── transmission.cpp
```

---

## Phase 13: Raycasting and Triggers

**Location:** `shared/engine/include/core/physics/query/`

Spatial queries for gameplay.

### Raycasting

**Raycast:**
- Cast ray from point in direction
- Return hit or not
- First hit or all hits

**Raycast Result:**
- Hit position
- Hit normal
- Hit body
- Distance

### Triggers

**Trigger Volumes:**
- Non-physical colliders
- Detect overlap events
- No collision response

### Structure

```
query/
├── raycast.h                 # Raycast interface
├── raycast.cpp
├── raycast_result.h          # Result struct
├── raycast_result.cpp
├── sphere_cast.h             # Sphere cast
├── sphere_cast.cpp
├── box_cast.h                # Box cast
├── box_cast.cpp
├── overlap_query.h           # Overlap queries
├── overlap_query.cpp
├── trigger.h                 # Trigger volume
├── trigger.cpp
└── trigger_manager.h         # Trigger event system
```

---

## Phase 14: Debug Visualization

**Location:** `shared/engine/include/core/physics/debug/`

Debug rendering for physics objects.

### Visualization

**Draw:**
- AABBs (broadphase)
- Collision shapes
- Contact points and normals
- Constraints and joints
- Sleeping islands
- CCD sweeps

### Structure

```
debug/
├── debug_draw.h              # Debug draw interface
├── debug_draw.cpp
├── shape_visualizer.h        # Shape visualization
├── shape_visualizer.cpp
├── contact_visualizer.h      # Contact visualization
├── contact_visualizer.cpp
├── constraint_visualizer.h   # Constraint visualization
└── constraint_visualizer.cpp
```

---

## Implementation Order

### Phase 1: Physics Component (Current)
1. Physics component header with all properties
2. Fine-grained source files for each subsystem
3. Physics math module (Vector3, Matrix3x3, Quaternion, Bounds)
4. Component registration
5. Unit tests
6. CMake integration
7. Build and verify

### Phase 2: Physics Math Module
1. Vector3 implementation
2. Matrix3x3 implementation
3. Quaternion implementation
4. Matrix4x4 implementation
5. Bounds implementation
6. SIMD optimizations
7. Unit tests

### Phase 3: Collision Shapes
1. Shape base class
2. Sphere shape
3. Box shape
4. Capsule shape
5. Plane shape
6. Convex hull
7. Triangle mesh
8. Unit tests

### Phase 4: Broadphase
1. Collision pair struct
2. Sweep and prune (1-axis)
3. Sweep and prune (3-axis)
4. Dynamic AABB tree
5. Spatial grid
6. Multi-box pruning
7. Unit tests and benchmarks

### Phase 5: Narrowphase
1. Contact manifold
2. GJK implementation
3. SAT implementation
4. Sphere-sphere collision
5. Sphere-box collision
6. Box-box collision
7. Capsule-capsule collision
8. Convex-convex collision
9. Unit tests

### Phase 6: Constraint Solver
1. Sequential impulse solver
2. Contact constraint
3. Distance joint
4. Hinge joint
5. Spherical joint
6. Fixed joint
7. Warm starting
8. Unit tests

### Phase 7: Physics World
1. Rigid body
2. Body state
3. Integration (semi-implicit Euler)
4. Physics world
5. World configuration
6. Unit tests

### Phase 8: Sleeping
1. Island management
2. Sleep tracker
3. Wake policy
4. Integration with world
5. Unit tests

### Phase 9: CCD
1. Speculative CCD
2. Swept CCD (linear)
3. Swept CCD (non-linear)
4. Time of impact
5. Integration with world
6. Unit tests

### Phase 10: Character Controller
1. Kinematic controller
2. Dynamic controller
3. Slide algorithm
4. Step up/down
5. Unit tests

### Phase 11: Ragdoll
1. Skeleton structure
2. Ragdoll settings
3. Ragdoll instance
4. Stabilization
5. Motor control
6. Unit tests

### Phase 12: Vehicle
1. Raycast vehicle
2. Constraint vehicle
3. Wheel and suspension
4. Engine and transmission
5. Unit tests

### Phase 13: Raycasting and Triggers
1. Raycast
2. Sphere cast
3. Box cast
4. Overlap queries
5. Trigger volumes
6. Unit tests

### Phase 14: Debug Visualization
1. Debug draw interface
2. Shape visualizer
3. Contact visualizer
4. Constraint visualizer
5. Integration with renderer

---

## References

**Research Sources:**
- Box2D (Erin Catto) - Sequential impulse solver
- Jolt Physics (Jorrit Rouwe) - Modern architecture, SIMD, lock-free broadphase
- PhysX (NVIDIA) - MBP, CCD, character controllers
- Bullet (Erwin Coumans) - Sequential impulse, SIMD
- Havok - Joint types, articulations
- DigitalRune - Character controllers
- Unity Physics - CCD methods

**Papers:**
- "Iterative Dynamics with Temporal Coherence" (Erin Catto, GDC 2005)
- "Stop my Constraints from Blowing Up!" (Oliver Strunk)
- "Comparison between Projected Gauss Seidel and Sequential Impulse Solvers"

**Documentation:**
- Unity Physics Manual
- PhysX Documentation
- Jolt Physics Documentation
- Bullet Physics Wiki
