# Poko Physics Engine — Full Architecture and Implementation Master Plan

## 0. Scope and Target

Poko Physics is a fully Poko-owned physics engine intended to provide:

* 100% Indie/A-grade physics capability.
* 100% AA-grade physics capability.
* Approximately 15% selected AAA-grade capabilities where they provide clear value.
* Strong performance on Windows 10+ and Android 11+.
* A public API that exposes Poko concepts only.
* No public dependency on Jolt, Bullet, PhysX, Havok, or another external physics API.
* A production implementation rather than a prototype assembled from isolated algorithms.
* A modular architecture that allows the engine to evolve from A-level to AA-level without rewriting the foundation.

The engine must prioritize:

1. Correctness.
2. Numerical stability.
3. Predictable behavior.
4. Memory safety.
5. Stable public APIs.
6. Debuggability.
7. Performance.
8. Scalability.
9. Deterministic/replay-capable execution where required.
10. Clean integration with the rest of Poko Engine.

The engine must be designed so that performance optimizations improve the implementation without changing the public programming model.

---

# 1. Core Architecture

## 1.1 Physics Layering

The physics engine is divided into explicit layers:

```text
Poko Engine
    │
    └── Poko Physics
          │
          ├── Public API
          │
          ├── Physics Core
          │
          ├── Math
          │
          ├── Geometry / Shapes
          │
          ├── Collision Detection
          │
          ├── Contacts
          │
          ├── Constraints
          │
          ├── Solver
          │
          ├── Simulation
          │
          ├── Queries
          │
          ├── Gameplay Physics
          │
          ├── Debug / Profiling
          │
          └── Backend / Platform
```

No lower-level implementation detail may leak upward.

---

## 1.2 Data Flow

The primary simulation data flow is:

```text
External Commands
      ↓
Command Buffer
      ↓
Pre-Simulation Synchronization
      ↓
Transform / Body Synchronization
      ↓
Broadphase Update
      ↓
Broadphase Pair Generation
      ↓
Narrowphase Dispatch
      ↓
Contact Generation
      ↓
Manifold Persistence / Contact Matching
      ↓
Island Construction
      ↓
Constraint Preparation
      ↓
Warm Start
      ↓
Velocity Solver
      ↓
Position Solver
      ↓
CCD / Additional Substeps
      ↓
Velocity / Position Integration
      ↓
Sleep / Wake Update
      ↓
Post-Simulation Synchronization
      ↓
Physics Events
      ↓
Queries / Replication / Gameplay
```

Each stage has an explicit ownership contract.

---

## 1.3 Stage Contracts

Every simulation stage must document:

* Inputs.
* Outputs.
* Read-only data.
* Writable data.
* Threading behavior.
* Required synchronization.
* Failure behavior.
* Memory allocation behavior.
* Performance budget.
* Determinism behavior.

No stage should depend on undocumented side effects from another stage.

---

# 2. Physics Module Boundaries

The engine must have explicit module ownership.

```text
physics/
├── core/
├── math/
├── geometry/
├── shapes/
├── materials/
├── bodies/
├── colliders/
├── broadphase/
├── narrowphase/
├── contacts/
├── constraints/
├── solver/
├── simulation/
├── ccd/
├── character/
├── ragdoll/
├── vehicle/
├── queries/
├── triggers/
├── events/
├── world/
├── memory/
├── threading/
├── debug/
├── profiling/
├── serialization/
├── replay/
├── deterministic/
├── cooking/
├── streaming/
├── validation/
├── platform/
├── tests/
└── reference/
```

Each subsystem owns its implementation rather than having a giant physics source file.

---

# 3. Fine-Grained Translation Unit Architecture

The source tree must use fine-grained translation units without becoming one-function-per-file.

Recommended pattern:

```text
body.h
body.cpp

body_storage.h
body_storage.cpp

body_create.cpp
body_destroy.cpp
body_transform.cpp
body_velocity.cpp
body_force.cpp
```

Use one `.cpp` per meaningful implementation responsibility.

Do not force every declaration into a `.cpp`.

Use:

```text
.h
```

for public declarations and core types.

Use:

```text
.inl
```

for templates and implementation that genuinely must remain header-visible.

Use:

```text
.cpp
```

for substantial implementation.

The architecture should avoid both:

* Giant monolithic translation units.
* Excessive one-function translation units.

---

# 4. Public API Boundary

Public APIs should expose:

```text
PhysicsWorld
RigidBody
Collider
Shape
Material
Constraint
Joint
PhysicsQuery
PhysicsEvent
```

Public APIs should never expose:

```text
Jolt types
Jolt IDs
internal pool nodes
solver rows
internal broadphase nodes
allocator implementation types
SIMD implementation types
internal manifold caches
private arrays
platform handles
internal thread primitives
```

---

# 5. Handle Architecture

Physics objects use generation-safe handles.

Required handle types:

```text
BodyHandle
ColliderHandle
ShapeHandle
ConstraintHandle
JointHandle
BroadphaseProxyHandle
MaterialHandle
```

Each handle contains at minimum:

```text
index
generation
```

The handle system must support:

* O(1)-average allocation.
* Free lists.
* Generation increments on reuse.
* Stale handle detection.
* Invalid-handle detection.
* Debug validation.
* Optional handle poisoning in debug builds.
* Stable serialization behavior.
* Deferred destruction support.

Handles must never become raw pointers in the public API.

---

# 6. Object Lifetime

Physics objects use explicit lifetimes:

```text
Created
    ↓
Registered
    ↓
Active
    ↓
Sleeping
    ↓
Woken
    ↓
PendingDestroy
    ↓
Destroyed
```

Destruction must be safe while the simulation is executing.

Use deferred destruction:

```text
Destroy Request
      ↓
Destroy Queue
      ↓
Safe Simulation Boundary
      ↓
Actual Destruction
```

No object may disappear from an array while another stage is iterating it.

---

# 7. Physics World

`PhysicsWorld` is the primary simulation owner.

It owns:

```text
BodyStorage
ColliderStorage
ShapeStorage
ConstraintStorage
MaterialRegistry
Broadphase
Narrowphase
ManifoldCache
IslandStorage
Solver
QuerySystem
EventBuffer
CommandBuffer
MemoryResources
SimulationSettings
Statistics
DebugState
```

Multiple independent physics worlds must be supported.

Examples:

```text
GameplayWorld
StudioPreviewWorld
EditorWorld
TestWorld
AuthorityWorld
```

World state must not rely on global static simulation variables.

---

# 8. Physics World Configuration

Define an explicit immutable/configuration object:

```text
PhysicsWorldSettings
```

Containing:

```text
gravity
fixedDelta
maxSubsteps
solver iterations
collision settings
CCD settings
sleep settings
world bounds
query limits
memory budgets
threading mode
determinism mode
quality level
```

Runtime configuration changes must be explicitly classified as:

* Safe immediately.
* Safe at simulation boundary.
* Requires world rebuild.

---

# 9. Time and Simulation Step

Use a fixed timestep simulation.

Architecture:

```text
Frame Delta
    ↓
Delta Clamp
    ↓
Accumulator
    ↓
Fixed Simulation Steps
```

The accumulator belongs to the world.

Never use function-static simulation accumulators.

Provide:

```text
fixedDelta
maxSubsteps
maxAccumulator
timeScale
```

Prevent simulation spiraling when frame time becomes unusually large.

---

# 10. Motion Integration

Initial integration:

* Semi-implicit Euler.

Support:

* Linear position integration.
* Angular integration.
* Quaternion normalization.
* Gravity.
* Force accumulation.
* Torque accumulation.
* Damping.
* Maximum velocity clamps.
* Maximum angular velocity clamps.

The integration system must distinguish:

```text
Static
Kinematic
Dynamic
```

and avoid doing unnecessary work for bodies that do not require integration.

---

# 11. Math Module

Core:

```text
Vector2
Vector3
Vector4
Point3
Quaternion
Matrix3x3
Matrix4x4
Transform
AABB
Ray
Plane
LineSegment
Triangle
DualQuaternion
```

Only include types that have direct engine value.

---

# 12. Vector3

Required:

* Addition.
* Subtraction.
* Multiplication.
* Division.
* Dot.
* Cross.
* Length.
* Length squared.
* Normalize.
* Safe normalize.
* Distance.
* Distance squared.
* Min/max.
* Absolute.
* Clamp.
* Projection.
* Rejection.
* Reflection.
* Lerp.
* Direction constants.

Validation:

* Zero-length normalization.
* NaN.
* Inf.
* Extremely large values.
* Extremely small values.

---

# 13. Matrix3x3

Required:

* Column-major convention if that remains the engine-wide convention.
* Construction.
* Multiplication.
* Matrix-vector multiplication.
* Transpose.
* Determinant.
* Inverse.
* `tryInverse`.
* Diagonal construction.
* Inertia tensor creation.
* Cross-product matrix.
* Basis extraction.
* Basis construction.
* Orthonormal testing.
* Orthonormalization.
* Symmetry testing.
* Diagonal testing.
* Approximate equality.

The cross-product convention must have dedicated tests.

The implementation must document whether:

```text
skew(v) * x = v × x
```

or the opposite convention is intended.

---

# 14. Quaternion

Required:

* Identity.
* Axis-angle.
* Euler construction.
* Multiplication.
* Conjugate.
* Inverse.
* Normalize.
* Safe normalize.
* Dot.
* Rotate vector.
* Slerp.
* Lerp.
* Axis-angle extraction.
* Angular distance.
* Approximate equality.

Handle:

* Antipodal quaternions.
* Nearly identical quaternions.
* Zero/near-zero quaternion.
* Numerical drift.

---

# 15. Transform

Dedicated type:

```text
Transform
    position
    rotation
```

Optional scale should not be part of the rigid-body transform if doing so creates unnecessary physics complexity.

Required:

* compose.
* inverse.
* transform point.
* transform vector.
* transform direction.
* local/world conversion.
* interpolation.

---

# 16. Numerical Policy

Create centralized tolerance configuration.

Categories:

```text
GeometryTolerance
CollisionTolerance
SolverTolerance
SleepTolerance
CCDTolerance
QueryTolerance
NormalizationTolerance
```

Every epsilon must document:

```text
meaning
unit
default
absolute/relative
scale behavior
usage
```

Avoid arbitrary local magic constants.

---

# 17. Numerical Failure Semantics

Do not silently hide numerical corruption.

Development builds:

```text
assert
diagnostic
error report
debug break
```

Shipping builds:

```text
reject invalid operation
quarantine invalid body/state
fallback to safe state where appropriate
record diagnostic
```

Never silently transform arbitrary NaN values into zero without diagnostics.

Detect:

```text
NaN
Inf
negative mass
negative inertia
invalid quaternion
zero-area shape
degenerate triangle
invalid AABB
invalid constraint
invalid transform
solver divergence
```

---

# 18. Floating-Point Policy

Define:

* Float precision.
* Floating-point assumptions.
* Flush-to-zero behavior.
* Denormal handling.
* Fast-math policy.
* Compiler floating-point flags.
* SIMD floating-point behavior.

Do not enable aggressive compiler behavior that invalidates required numerical guarantees.

---

# 19. Determinism Architecture

Provide two conceptual modes:

```text
Performance Mode
Deterministic Mode
```

Deterministic mode should define:

* Stable body ordering.
* Stable pair ordering.
* Stable constraint ordering.
* Stable event ordering.
* Deterministic command application.
* Controlled SIMD behavior.
* Controlled randomization.
* Deterministic replay inputs.
* Deterministic serialization.

Parallel execution must not accidentally introduce order-dependent behavior in deterministic mode.

Cross-platform bit-identical results should only be claimed where actually demonstrated by tests.

---

# 20. Large World / Precision Architecture

Do not require double precision for every physics operation.

Separate:

```text
World Position
Simulation Position
```

Potential architecture:

```text
High-precision world coordinates
        ↓
Local physics origin
        ↓
Float-based simulation
```

Support future:

* Origin rebasing.
* World partitioning.
* Large-world streaming.
* Precision-safe transforms.
* Local simulation regions.

Fixed-point should remain a deliberate optional technology rather than becoming a mandatory foundation unless determinism requirements justify it.

---

# 21. Shape System

Core shapes:

```text
Sphere
Box
Capsule
Plane
ConvexHull
Triangle
TriangleMesh
HeightField
CompoundShape
```

Optional future:

```text
Cylinder
Cone
RoundedBox
CustomConvex
```

Each shape defines:

```text
shape type
local bounds
support mapping where applicable
mass properties
ray intersection
cast support
overlap support
serialization
validation
```

---

# 22. Shape Ownership

Separate:

```text
Shape Definition
```

from:

```text
Collider Instance
```

Example:

```text
Shape
    ↓
Collider
    ↓
RigidBody
```

A shape should be reusable across many colliders when immutable.

This reduces memory usage.

---

# 23. Compound Shapes

Support compound colliders made from child shapes.

Required:

* Child shape storage.
* Child local transforms.
* Combined bounds.
* Combined mass properties.
* Child filtering.
* Broadphase proxy strategy.
* Serialization.
* Runtime validation.

Avoid generating unnecessary standalone rigid bodies for every child.

---

# 24. Convex Hull Cooking

Provide offline/asset-time convex cooking:

```text
Input Mesh
    ↓
Validate
    ↓
Weld vertices
    ↓
Remove degenerate geometry
    ↓
Generate convex hull
    ↓
Optimize hull
    ↓
Generate support structure
    ↓
Serialize cooked shape
```

Runtime should prefer cooked data over expensive mesh processing.

---

# 25. Triangle Mesh Cooking

For static/mostly-static geometry:

```text
Mesh
 ↓
Validation
 ↓
Triangle cleanup
 ↓
Winding validation
 ↓
Bounds generation
 ↓
BVH construction
 ↓
Optional quantization/compression
 ↓
Cooked Physics Asset
```

Triangle mesh should initially prioritize static collision.

Dynamic arbitrary triangle-mesh rigid bodies should not be treated as a default path.

---

# 26. Heightfield

Support terrain-oriented physics:

* Height samples.
* Holes.
* Material/region IDs.
* Bounds.
* Collision filtering.
* Raycasting.
* Sweep queries.
* Cooking.
* Streaming.

---

# 27. Shape Mass Properties

Every physical shape must have defined:

```text
volume
center of mass
inertia tensor
bounds
```

Support:

```text
density → mass
mass → density
```

Validate:

* finite mass.
* positive mass.
* positive inertia.
* nondegenerate geometry.

Compound mass properties must account for transforms and parallel-axis effects.

---

# 28. Material System

Dedicated physics materials.

Properties:

```text
friction
restitution
rolling resistance
surface type
combine rules
```

Optional:

```text
anisotropic friction
material velocity
special impact behavior
```

Define deterministic material-combination rules:

```text
average
minimum
maximum
multiply
custom policy
```

Do not allow arbitrary user callbacks inside the low-level solver hot path.

---

# 29. Collision Filtering

Support:

```text
collision layer
collision mask
query layer
query mask
trigger filtering
self-collision rules
group filtering
```

Filtering occurs before expensive narrowphase work where possible.

Provide multiple levels:

```text
Broadphase filter
Narrowphase filter
Query filter
Gameplay filter
```

---

# 30. Broadphase Architecture

Create:

```text
IBroadphase
BroadphaseManager
```

Implement:

```text
DynamicAABBTree
SweepAndPrune
SpatialGrid / SpatialHash
```

The public engine interacts with the interface.

---

# 31. Dynamic AABB Tree

Required:

* insertion.
* removal.
* update.
* fat AABB.
* balancing.
* rotations.
* SAH-based insertion heuristics.
* pooled nodes.
* iterative queries.
* pair generation.
* validation.
* debug visualization.

Define node ownership and index validity explicitly.

---

# 32. Sweep and Prune

Support:

* X/Y/Z axis ordering.
* Primary-axis selection.
* Incremental updates.
* Pair generation.
* Static optimization.

Do not assume SAP is always faster than the tree.

Benchmark based on workload.

---

# 33. Spatial Grid

Support:

* Uniform grid.
* Sparse hash.
* Multi-level grid if needed.
* Cell insertion/removal.
* Query.
* Pair generation.

Define behavior for:

* giant bodies.
* huge world coordinates.
* extremely dense cells.
* empty sparse spaces.

---

# 34. Broadphase Selection

The BroadphaseManager decides which structure is used.

Selection may be:

```text
World-configured
Scene-configured
Workload-selected
Benchmark-selected
```

Do not dynamically switch structures every frame without a demonstrated reason.

---

# 35. Narrowphase Architecture

Create:

```text
CollisionDispatcher
PairAlgorithm
ContactGenerator
```

Architecture:

```text
Shape A
Shape B
   ↓
Dispatcher
   ↓
Pair Algorithm
   ↓
Contact Result
```

Use a dispatch table rather than giant conditional logic.

---

# 36. Shape Pair Algorithms

Dedicated algorithms for common pairs:

```text
Sphere × Sphere
Sphere × Plane
Sphere × Box
Sphere × Capsule
Capsule × Plane
Capsule × Capsule
Capsule × Box
Box × Plane
Box × Box
Convex × Convex
Convex × Triangle
Capsule × Triangle
```

Add algorithms according to actual feature requirements.

Common analytic pairs should avoid unnecessarily invoking GJK.

---

# 37. GJK

Implement a robust GJK implementation for convex collision/distance.

Required:

* support mapping.
* simplex management.
* termination criteria.
* duplicate-point detection.
* degenerate simplex handling.
* iteration limit.
* numerical tolerance.
* initial direction handling.
* separation result.
* intersection result.

Do not use simplistic pseudocode as the final implementation.

---

# 38. EPA

Use EPA or another explicit penetration solver where required after GJK intersection.

Required:

* robust polytope construction.
* face management.
* convergence criteria.
* degenerate handling.
* maximum iteration count.
* fallback behavior.

EPA must have dedicated failure tests.

---

# 39. SAT

Use SAT for appropriate polyhedral pairs.

Required:

* axis generation.
* separation testing.
* penetration measurement.
* minimum penetration axis.
* numerical handling.
* feature identification.

For box-box:

```text
3 face axes A
3 face axes B
9 edge cross-product axes
```

with robust handling when cross-product axes are near zero.

---

# 40. Contact Generation

Collision detection must generate a standardized internal result:

```text
CollisionResult
    normal
    penetration
    points
    feature IDs
```

Do not allow each collision algorithm to produce unrelated result formats.

---

# 41. Contact Manifold System

Support:

* Up to configurable contact points.
* Contact reduction.
* Contact persistence.
* Feature IDs.
* Normal.
* Tangent basis.
* Penetration.
* Previous impulses.

The manifold is the bridge between collision detection and solver.

---

# 42. Contact Persistence

Maintain a manifold cache keyed by stable body/collider pair identity.

Pipeline:

```text
Previous Manifold
       ↓
New Contact Candidates
       ↓
Contact Matching
       ↓
Impulse Transfer
       ↓
New Persistent Manifold
```

Matching should use feature identifiers and geometric tolerance.

---

# 43. Contact Reduction

If more contacts are generated than the solver wants:

* deepest point.
* widest spread.
* stable feature selection.
* center coverage.

Do not simply take the first N contacts.

---

# 44. Contact Events

Generate:

```text
ContactBegin
ContactPersist
ContactEnd
```

Events must have deterministic ordering rules.

Store them in an event buffer.

Do not immediately invoke arbitrary game/Mute callbacks from deep inside collision detection.

---

# 45. Rigid Body Architecture

RigidBody contains hot simulation state:

```text
position
rotation
linearVelocity
angularVelocity
inverseMass
inverseInertia
flags
sleep state
motion type
```

Cold data should remain outside hot arrays where possible.

---

# 46. Collider Architecture

Collider owns:

```text
shape handle
material handle
local transform
collision filter
trigger state
sensor state
```

The collider connects shape data to a body.

---

# 47. Static / Kinematic / Dynamic

Static:

* no integration.
* no solver movement.

Kinematic:

* externally controlled transform/velocity.
* affects dynamic bodies.

Dynamic:

* fully simulated.

Rules must define interactions between all three.

---

# 48. Forces and Impulses

Support:

```text
force
torque
impulse
angular impulse
gravity
```

Separate:

```text
persistent force
one-frame force
direct velocity modification
teleport
```

Define exactly how each enters the simulation.

---

# 49. Transform Synchronization

Integrate physics transforms back into the engine object model only at explicit synchronization points.

Architecture:

```text
Physics State
    ↓
Sync Buffer
    ↓
Engine Instance Transform
```

Do not allow every physics body to directly mutate engine Instances from worker threads.

---

# 50. Center of Mass

RigidBody must distinguish:

```text
Body Transform
Center-of-Mass Transform
Collider Local Transform
```

Support:

* center-of-mass offsets.
* inertia around COM.
* transformed inertia.
* compound COM calculation.

---

# 51. Inertia System

Support:

* local inertia.
* inverse inertia.
* world inertia rotation.
* dynamic update when shape changes.

Avoid computing inverse inertia repeatedly when body state has not changed.

Cache where appropriate.

---

# 52. Sleeping

Support:

* velocity thresholds.
* angular velocity thresholds.
* sleep timer.
* force thresholds.
* manual wake.
* wake propagation.
* disable sleep.
* island sleep.

Sleeping bodies should be cheap.

---

# 53. Island Management

Build connectivity graph:

```text
Body
 ↕
Contact / Constraint
 ↕
Body
```

Create islands.

Support:

* island construction.
* island-local storage.
* island wake propagation.
* island sleep.
* island solver dispatch.
* island profiling.

Static bodies can act as graph boundaries rather than forcing every static body into active island work.

---

# 54. Island Solver

Solve independent islands independently where safe.

Architecture:

```text
Island Builder
      ↓
Island List
      ↓
Parallel Island Jobs
      ↓
Solver
```

Keep deterministic mode ordering stable.

---

# 55. Constraint Architecture

Separate:

```text
Constraint Definition
Constraint Runtime State
Constraint Solver Rows
```

Pipeline:

```text
Constraint
    ↓
Prepare
    ↓
Solver Rows
    ↓
Warm Start
    ↓
Solve
```

---

# 56. Generic Constraint Row

Internal solver representation should contain:

```text
linear Jacobian A
angular Jacobian A
linear Jacobian B
angular Jacobian B
effective mass
bias
lower limit
upper limit
accumulated impulse
flags
```

The solver should operate on rows rather than having separate solving code for every joint.

---

# 57. Constraint Types

Required:

```text
Distance
Fixed/Weld
Ball Socket
Hinge
Prismatic/Slider
Spring
Rope
```

Then:

```text
Cone
Twist
6DOF
```

as needed for ragdolls/advanced constraints.

---

# 58. Constraint Features

Support:

* limits.
* motors.
* springs.
* damping.
* softness.
* break thresholds.
* enable/disable.
* collision between connected bodies.
* local anchors.
* local axes.

---

# 59. Sequential Impulse Solver

Base solver:

* Projected Gauss-Seidel style iterative solving.
* Warm starting.
* Accumulated impulse clamping.
* Effective mass.
* Bias.
* Restitution.
* Friction.

Configurable:

```text
velocityIterations
positionIterations
warmStartFactor
slop
baumgarte
restitutionThreshold
```

---

# 60. Contact Solver

Support:

* normal impulse.
* tangent impulses.
* friction limits.
* restitution.
* penetration bias.
* accumulated impulse.

Use specialized handling for common small contact blocks.

---

# 61. Friction

Support:

```text
static/dynamic-equivalent Coulomb behavior
two tangent directions
friction coefficient
```

Optional future:

```text
rolling friction
anisotropic friction
```

---

# 62. Restitution

Define:

* material restitution.
* combine rule.
* minimum impact velocity.
* maximum useful restitution.
* solver interaction.

Prevent tiny impacts from causing jittering bounces.

---

# 63. Position Stabilization

Support:

* Baumgarte stabilization.
* penetration slop.
* bias clamping.
* optional split impulse.

Tune through tests, not arbitrary values.

---

# 64. Stack Stability

Dedicated architecture/testing for:

```text
2-box stack
10-box stack
100-box stack
1000-box stack
```

Measure:

* jitter.
* sinking.
* drift.
* solver convergence.
* performance.

---

# 65. Shock Propagation / Advanced Stability

Reserve optional support for:

* stack stabilization.
* mass ratio handling.
* shock propagation.

Only enable if benchmarked and validated.

---

# 66. Continuous Collision Detection

CCD subsystem:

```text
CCDManager
Sweep
TOI solver
Speculative contact generator
```

Support:

* motion thresholds.
* continuous bodies.
* swept sphere.
* shape casts.
* speculative contacts.
* maximum substeps.
* time budgets.

---

# 67. CCD Failure Handling

Protect against:

* infinite TOI loops.
* zero-time collisions.
* numerical stagnation.
* repeated impact loops.
* excessive substeps.

Enforce:

```text
maxIterations
maxSubsteps
timeBudget
```

---

# 68. Rotational CCD

Initial implementation may prioritize linear CCD.

Reserve architecture for:

* angular motion.
* rotating thin objects.
* improved TOI.

Only move this into required production scope after actual gameplay evidence.

---

# 69. Character Controller

Character physics should be a separate subsystem.

Do not force character movement to behave exactly like ordinary dynamic rigid bodies.

Support:

* capsule.
* sweeps.
* ground detection.
* slope limits.
* step offset.
* collide-and-slide.
* moving platforms.
* depenetration.
* ceiling handling.
* sliding.
* grounded state.
* air movement.

---

# 70. Character Controller Queries

Dedicated:

```text
CapsuleCast
GroundProbe
StepProbe
CeilingProbe
DepenetrationQuery
```

The controller must use the standard query system instead of duplicating collision logic.

---

# 71. Moving Platforms

Character controller must support:

```text
kinematic platforms
moving rigid bodies
rotating platforms
platform velocity
platform transform tracking
```

Avoid teleporting characters unpredictably when platforms move.

---

# 72. Ragdoll System

Ragdolls use rigid bodies + constraints initially.

Architecture:

```text
Skeleton
   ↓
Ragdoll Definition
   ↓
Rigid Bodies
   ↓
Joint Constraints
```

Support:

* bone-body mapping.
* joint limits.
* mass distribution.
* motors.
* springs.
* damping.
* activation/deactivation.
* blending back to animation.

---

# 73. Articulated Bodies

Reserve an advanced architecture for:

* reduced-coordinate articulation.
* multi-body chains.
* joint-space solving.

Do not make reduced-coordinate articulation mandatory for ordinary rigid bodies.

---

# 74. Vehicle Physics

Raycast vehicle subsystem:

```text
Vehicle
 ├── Wheel
 ├── Suspension
 ├── Tire
 ├── Steering
 ├── Brake
 └── Powertrain
```

Support:

* raycast wheels.
* suspension.
* spring/damper.
* steering.
* braking.
* traction.
* drive force.
* differential/basic power distribution.
* anti-roll bars.

Do not attempt a full AAA tire simulator unless it becomes necessary.

---

# 75. Queries

Dedicated query subsystem:

```text
Raycast
SphereCast
CapsuleCast
BoxCast
ShapeCast
OverlapSphere
OverlapBox
OverlapCapsule
OverlapShape
ClosestPoint
```

Each query supports:

```text
collision filter
trigger filter
max distance
max hits
sort behavior
layer mask
user filter
```

---

# 76. Query Result Buffers

Avoid allocations during queries.

Support:

```text
single-result API
caller-provided fixed buffer
span/output buffer
bounded multi-hit API
```

Example:

```text
Raycast
 ↓
Broadphase candidates
 ↓
Shape tests
 ↓
Filter
 ↓
Output buffer
```

---

# 77. Trigger/Sensor System

Support:

* sensor shapes.
* no physical response.
* enter.
* exit.
* persistent overlap.
* filtering.
* events.

Triggers should share broadphase infrastructure with physics bodies.

---

# 78. Trigger Persistence

Maintain overlap state so that:

```text
Enter
Stay
Exit
```

can be generated reliably.

Do not infer exits only from currently visible overlaps without tracking prior state.

---

# 79. Physics Event System

Central event buffer:

```text
ContactBegin
ContactPersist
ContactEnd

TriggerBegin
TriggerPersist
TriggerEnd

BodyWake
BodySleep

ConstraintBreak

CCDImpact
```

Event ordering must be documented.

---

# 80. Event Dispatch

Physics writes events.

Engine/gameplay consumes events after the safe simulation boundary.

This prevents:

```text
script destroys body
while solver is iterating body
```

---

# 81. Command Buffer

External changes enter physics through commands:

```text
CreateBody
DestroyBody
CreateCollider
DestroyCollider
SetTransform
SetVelocity
ApplyForce
ApplyImpulse
SetMaterial
SetShape
SetCollisionFilter
SetKinematicTarget
Wake
Sleep
CreateConstraint
DestroyConstraint
```

Commands are processed at defined synchronization points.

---

# 82. Threading Model

Do not create:

```text
one thread per body
one thread per NPC
one thread per connection
```

Use the Poko Job System.

Physics execution:

```text
Physics Main Stage
       ↓
Broadphase Jobs
       ↓
Narrowphase Jobs
       ↓
Island Build
       ↓
Island Solver Jobs
       ↓
Integration
       ↓
Events
```

---

# 83. Thread Ownership

Define:

```text
Main thread safe
Physics thread only
Worker safe
Read-only concurrently
Write-exclusive
```

Every physics data structure should have documented access rules.

---

# 84. Parallel Broadphase

Parallelize:

* proxy updates.
* spatial updates where safe.
* pair generation partitions.

Avoid races in shared pair buffers.

Use:

```text
worker-local buffers
merge stage
```

rather than many threads pushing into one shared vector.

---

# 85. Parallel Narrowphase

Partition candidate pairs.

Each worker writes to its own temporary contact buffer.

Merge deterministically when deterministic mode is enabled.

---

# 86. Parallel Solver

Parallelize independent islands.

Within large islands, optionally parallelize constraint work later.

Do not parallelize a tiny island if synchronization costs exceed the computation.

---

# 87. Memory Architecture

Dedicated physics allocators:

```text
PhysicsPersistentAllocator
PhysicsFrameAllocator
PhysicsScratchAllocator
PhysicsWorkerAllocator
```

Pools:

```text
BodyPool
ColliderPool
ShapePool
ConstraintPool
ContactPool
BroadphaseNodePool
IslandPool
```

---

# 88. Allocation Rules

Simulation hot paths must avoid general-purpose allocations.

Allowed:

```text
preallocated pools
arenas
worker-local scratch
persistent caches
```

Avoid:

```text
new
delete
malloc
shared_ptr
```

inside hot loops.

---

# 89. Memory Budgets

PhysicsWorld should expose configurable budgets:

```text
maxBodies
maxColliders
maxShapes
maxContacts
maxPairs
maxConstraints
maxIslands
maxBroadphaseNodes
maxQueriesPerFrame
maxCCDSubsteps
```

Budget exhaustion must have deterministic behavior.

---

# 90. Cache Architecture

Organize hot state for access patterns.

Potential SoA:

```text
positions[]
rotations[]
linearVelocities[]
angularVelocities[]
inverseMasses[]
inverseInertias[]
flags[]
```

Keep cold data elsewhere.

Do not assume SoA is universally superior; benchmark actual workloads.

---

# 91. SIMD Architecture

Create a platform-independent math layer with platform backends:

```text
Scalar
SSE
AVX/AVX2
NEON
```

Select according to target architecture.

SIMD specialization should not leak into gameplay APIs.

Only types and loops that benefit from SIMD should be aligned specifically for SIMD.

Do not blanket-align every physics type merely because it is a physics type.

---

# 92. Mobile Optimization

Android-specific priorities:

* lower memory footprint.
* fewer worker threads when appropriate.
* thermal adaptation.
* reduced CCD.
* physics LOD.
* lower solver iterations where quality allows.
* sleeping aggressiveness.
* broadphase adaptation.
* allocation reduction.

---

# 93. Physics LOD

Optional distance/workload-based levels:

```text
Full
High
Medium
Low
Sleeping/Disabled
```

LOD may reduce:

* solver iterations.
* CCD.
* update frequency.
* sleeping thresholds.
* collision detail.

Never silently disable gameplay-critical collisions.

---

# 94. Physics Quality Modes

Support:

```text
Performance
Balanced
Quality
Custom
```

Settings may control:

* solver iterations.
* CCD.
* substeps.
* broadphase policy.
* contact limits.
* ragdoll quality.
* vehicle quality.
* character query detail.

---

# 95. Physics Budgeting

Measure per-world:

```text
broadphase time
narrowphase time
solver time
integration time
CCD time
queries
events
memory
job wait time
```

Provide hard or soft budgets.

---

# 96. Time Budget Protection

If a world exceeds its frame budget:

* limit CCD work.
* limit optional queries.
* reduce optional physics LOD.
* defer noncritical work.
* report budget violation.

Never silently destabilize core simulation.

---

# 97. Profiling Architecture

Every major subsystem must have profiling zones:

```text
PhysicsStep
Broadphase
Narrowphase
Manifold
IslandBuild
ConstraintPrepare
Solver
Integration
CCD
Queries
Events
```

Expose counters:

```text
body count
active bodies
sleeping bodies
pair count
contact count
constraint count
island count
CCD body count
query count
allocation count
```

---

# 98. Debug Visualization

Required:

* rigid-body shapes.
* collider shapes.
* AABBs.
* broadphase tree.
* contact points.
* contact normals.
* friction directions.
* joint anchors.
* joint limits.
* constraint impulses.
* islands.
* sleeping bodies.
* CCD paths.
* character controller sweeps.
* raycasts.
* shape casts.
* vehicle wheels/suspension.

---

# 99. Physics Inspector

Poko Studio should be able to inspect:

```text
Body
Collider
Shape
Material
Constraint
Island
Contact
Broadphase proxy
```

Show:

```text
transform
velocity
mass
inertia
sleeping
collision filters
material
contacts
constraints
solver state
```

---

# 100. Runtime Physics Diagnostics

Provide warnings for:

```text
degenerate collider
extreme mass ratio
invalid inertia
excessive penetration
solver instability
CCD exhaustion
too many contacts
too many constraints
body teleport
NaN state
Inf state
budget overflow
```

---

# 101. Physics Validation Layer

Dedicated validator:

```text
PhysicsValidator
```

Checks:

```text
body handles
collider handles
shape references
broadphase links
manifold references
constraint references
island membership
NaN/Inf
AABB validity
pool corruption
generation validity
```

Debug builds may validate more frequently.

Shipping builds use cheaper validation where necessary.

---

# 102. Internal Assertions

Assertions should verify invariants such as:

```text
handle generation matches
body index is active
AABB min <= max
mass > 0
inverseMass >= 0
quaternion normalized within tolerance
inertia invertible where required
contact penetration >= expected bounds
constraint rows reference valid bodies
```

---

# 103. Serialization

Physics assets and persistent definitions require versioned serialization.

Serialize:

```text
shape definitions
materials
body definitions
collider definitions
constraint definitions
physics configuration
```

Runtime snapshots may additionally serialize:

```text
body transforms
velocities
sleep state
constraint impulses
```

Never serialize raw memory layouts.

---

# 104. Physics Snapshot System

Support:

```text
WorldSnapshot
BodySnapshot
ConstraintSnapshot
```

Used for:

* debugging.
* replay.
* rollback research.
* tests.
* authority diagnostics.

---

# 105. Replay System

Create:

```text
PhysicsReplay
```

Record:

```text
initial state
commands
simulation settings
fixed timestep
engine/physics version
optional random state
```

Replay produces the same observable sequence where deterministic mode guarantees apply.

---

# 106. Desync Diagnostics

Authority/client debugging can compare:

```text
body transform
velocity
sleep state
contact count
constraint state
```

At selected simulation checkpoints.

When divergence occurs:

```text
step N
body X
field Y
expected
actual
```

Record enough information to reproduce the divergence.

---

# 107. Differential Backend Testing

Keep Jolt as an optional reference backend.

Architecture:

```text
Poko Physics API
      │
      ├── Poko Physics
      │
      └── Reference Backend
             └── Jolt
```

Same test input:

```text
Input
 ↓
Poko Physics
Jolt Reference
 ↓
Result Comparison
```

Compare where meaningful:

* collision existence.
* normals.
* penetration.
* transforms.
* impulses.
* sleep behavior.
* query results.

Do not make Jolt behavior the definition of correctness.

---

# 108. Cooking and Offline Pipeline

Physics cooking should happen during asset processing where practical.

Pipeline:

```text
Source Asset
 ↓
Validate
 ↓
Extract Geometry
 ↓
Generate Collision
 ↓
Optimize
 ↓
Generate Bounds
 ↓
Generate BVH
 ↓
Generate Mass Properties
 ↓
Compress
 ↓
Store Cooked Physics Data
```

Runtime should prefer cooked resources.

---

# 109. Asset Formats

Physics assets should have explicit versioned runtime formats.

Example conceptual resources:

```text
.pkxcollision
.pkxphysics
```

They may be embedded into or referenced by PKX.

The physics format must be:

* versioned.
* endian-defined.
* pointer-free.
* bounds-checked.
* integrity-checked.
* safely loadable.

---

# 110. Safe Runtime Loading

Never trust physics asset bytes.

Validate:

```text
magic
version
size
offsets
counts
enum values
AABB values
vertex counts
index counts
NaN/Inf
allocation limits
```

Protect against intentionally malformed assets.

---

# 111. Network / Authority Integration

Physics must provide an authority-friendly mode.

Authority should be able to use:

```text
smaller physics world
limited collision feature set
predictable request validation
server-side critical collision tests
```

Do not require the complete client renderer/game simulation in the authority process.

---

# 112. Client Physics

Client responsibilities may include:

* local presentation.
* interpolation.
* prediction.
* local character movement.
* visual physics.
* non-authoritative simulation.

The client is not trusted.

---

# 113. Authority Physics

Authority responsibilities:

* verify legal movement.
* verify collision-relevant requests.
* verify server-owned state.
* prevent impossible transformations.
* apply game-authoritative rules.

Authority should use deterministic/controlled simulation where required.

---

# 114. Physics Request Validation

Requests can be validated against:

```text
maximum movement
maximum velocity
allowed teleport
collision state
ownership
body state
cooldowns
authority state
```

Use invariant-based validation instead of trusting reported physics state.

---

# 115. Ownership

Each body may have an ownership model:

```text
Server
Client
Shared
Physics-only
```

Ownership controls who may issue certain commands.

---

# 116. Streaming

Physics streaming should support:

```text
loaded
active
sleeping
unloaded
proxy
```

For very large worlds:

```text
World Partition
    ↓
Physics Region
```

Only active regions should consume expensive physics resources.

---

# 117. Physics Proxies

For far/streamed areas, optionally use reduced collision proxies:

```text
full collider
simplified collider
query-only proxy
disabled
```

This must be explicit state, not accidental missing physics.

---

# 118. Sleeping and Streaming Interaction

A streamed region must define:

* what happens to sleeping bodies.
* how state is serialized.
* how bodies wake when region reloads.
* how contacts across boundaries work.

---

# 119. Boundary Handling

Define behavior when objects approach:

```text
world limits
streaming boundaries
physics region boundaries
precision-rebase boundaries
```

---

# 120. Debug/Editor Simulation

Poko Studio must use the real Poko Physics runtime.

Support:

```text
Play
Pause
Single Step
Slow Motion
Frame Advance
Reset
Restart Physics
Freeze Body
Wake Body
Force Body Awake
```

No fake editor-only physics system should exist.

---

# 121. Physics Editor

Studio tools should support:

* collider generation.
* primitive collider editing.
* convex hull editing.
* material assignment.
* collision filter editing.
* mass/inertia preview.
* joint editing.
* ragdoll setup.
* vehicle setup.
* character controller setup.

---

# 122. Contact Debugger

Studio debugger:

```text
select body
 ↓
show contacts
 ↓
show contact manifold
 ↓
show penetration
 ↓
show impulses
 ↓
show solver rows
```

---

# 123. Constraint Debugger

Show:

```text
constraint type
anchors
axes
limits
motor state
spring state
current error
impulse
break threshold
```

---

# 124. Broadphase Debugger

Show:

```text
AABBs
fat AABBs
tree nodes
tree height
pairs
proxy counts
grid cells
SAP ordering
```

---

# 125. Solver Debugger

Show:

```text
velocity iterations
position iterations
constraint rows
effective masses
bias values
impulses
clamp limits
convergence
```

---

# 126. Numerical Debug Mode

Optional expensive mode:

```text
check every body after stage
check every manifold
check every solver result
check quaternion normalization
check finite values
```

Used in tests and development.

---

# 127. Testing Architecture

Testing is divided into:

```text
Unit Tests
Property Tests
Algorithm Tests
Integration Tests
Regression Tests
Stress Tests
Fuzz Tests
Differential Tests
Replay Tests
Determinism Tests
Performance Tests
Soak Tests
```

---

# 128. Math Tests

Test:

* Vector operations.
* Matrix multiplication.
* Matrix inverse.
* Determinant.
* Quaternion operations.
* Transform composition.
* AABB operations.
* Ray intersections.

Test both expected outputs and invariants.

---

# 129. Property-Based Math Testing

Examples:

```text
A + 0 ≈ A
A * I ≈ A
I * A ≈ A
transpose(transpose(A)) ≈ A
A * inverse(A) ≈ I
det(A * B) ≈ det(A) * det(B)
```

Cross-product property:

```text
crossMatrix(v) * x ≈ cross(v, x)
```

when that is the selected convention.

---

# 130. Geometry Tests

For every shape:

* bounds.
* volume.
* mass.
* inertia.
* containment.
* support mapping.
* ray tests.
* cast tests.

Test degenerate and extreme values.

---

# 131. Collision Pair Matrix

Create a test matrix for every supported pair:

```text
Sphere/Sphere
Sphere/Box
Sphere/Capsule
...
```

Each pair must have:

```text
separated
touching
slightly penetrating
deep penetration
large scale
small scale
rotated
degenerate edge cases
```

---

# 132. Broadphase Tests

Test:

* insertion.
* removal.
* movement.
* fat AABB.
* pair generation.
* duplicate pair prevention.
* missed pair prevention.
* tree balancing.
* rebuild.
* mass insertion.
* random movement.

Compare against a brute-force O(n²) oracle in tests.

---

# 133. Narrowphase Oracle Tests

For manageable random cases:

```text
Brute/reference algorithm
        vs
Poko implementation
```

Catch false positives and false negatives.

---

# 134. Manifold Tests

Test:

* persistent contacts.
* contact matching.
* feature IDs.
* manifold reduction.
* warm-start transfer.
* contact expiration.
* rotating/translated bodies.

---

# 135. Solver Tests

Test:

* two-body collision.
* restitution.
* friction.
* resting contact.
* stacking.
* joints.
* motors.
* limits.
* broken constraints.
* extreme mass ratios.

---

# 136. Stability Tests

Required scenarios:

```text
single box resting
10 box stack
100 box stack
1000 box stack
sphere rolling
sphere bounce
pendulum
hinge
rope
spring
vehicle suspension
character slope
character step
```

Measure drift and instability.

---

# 137. Stress Tests

At minimum test:

```text
10,000 bodies
large numbers of colliders
dense contact scenes
deep stacks
large broadphase scenes
many queries
many constraints
fast-moving objects
long-running simulation
```

Your original plan already calls for a 10,000-body stress target and long-running stability testing.

---

# 138. Fuzz Testing

Fuzz:

```text
shape values
ray values
AABB values
quaternions
mesh topology
convex hulls
collision filters
constraint parameters
serialized physics assets
commands
```

The engine must reject malformed data without corrupting memory.

---

# 139. Determinism Tests

Run the same simulation:

```text
N times
```

and compare snapshots/events.

Also test:

```text
single-thread
multi-thread
deterministic mode
different batch sizes
```

where deterministic equivalence is promised.

---

# 140. Replay Tests

Store fixed replay files in the repository.

Use them as permanent regression tests.

Example:

```text
replays/
    stack_001
    vehicle_001
    character_001
    ragdoll_001
    ccd_001
```

---

# 141. Regression Policy

Every fixed physics bug receives:

```text
bug description
minimal reproduction
regression test
optional replay
```

A bug must not be considered permanently fixed without a regression test when practical.

---

# 142. Benchmark Architecture

Dedicated benchmark executable:

```text
poko_physics_bench
```

Benchmarks:

```text
math
AABB
broadphase
narrowphase
manifolds
solver
island building
queries
CCD
character
vehicles
memory
```

Record:

```text
mean
median
p95
p99
allocations
memory
```

---

# 143. Benchmark Scenes

Standardized scenes:

```text
Empty
100 bodies
1,000 bodies
10,000 bodies
Stack
Ragdoll field
Vehicle scene
Character scene
Dense contacts
Large sparse world
CCD scene
Query-heavy scene
```

---

# 144. Performance Regression

CI should detect:

```text
frame-time regression
memory regression
allocation regression
pair-count regression
solver regression
query regression
```

Do not optimize against a single machine only.

---

# 145. Reference Device Matrix

Performance should be measured on representative:

```text
low-end Android
mid-range Android
desktop CPU
higher-end desktop
```

Exact devices can be selected later, but the benchmark architecture should exist from the beginning.

---

# 146. Build Architecture

Physics must build independently enough to allow:

```text
unit-test build
benchmark build
debug build
shipping build
reference-backend build
tools build
```

---

# 147. Sanitizers

Development CI should support appropriate:

```text
AddressSanitizer
UndefinedBehaviorSanitizer
ThreadSanitizer where compatible
```

Use them especially for:

* handles.
* pools.
* threading.
* command buffers.
* serialization.

---

# 148. Static Analysis

Use static analysis appropriate to the Poko C++ toolchain.

Check:

* lifetime bugs.
* integer overflow.
* narrowing.
* unreachable code.
* unsafe casts.
* invalid ownership.
* missing initialization.
* dead code.

---

# 149. API Verification

Compile-time tests should verify:

* public API availability.
* type sizes where specified.
* ABI assumptions.
* no external backend types exposed.
* serialization interfaces.
* handle semantics.

---

# 150. Documentation

Every public physics type/function should use Doxygen.

Document:

```text
purpose
parameters
return
units
threading
lifetime
failure behavior
invariants
performance notes
```

Mathematical conventions must be documented.

Examples:

```text
coordinate handedness
matrix layout
quaternion convention
cross-product convention
units
gravity convention
angle units
```

---

# 151. Units

Physics uses documented physical units.

Recommended baseline:

```text
distance = meters
mass = kilograms
time = seconds
angle = radians
velocity = meters/second
acceleration = meters/second²
```

Every API involving quantities should document units.

---

# 152. Coordinate System

Define:

```text
right
up
forward
handedness
```

and use the same convention everywhere.

No subsystem should silently assume a different orientation.

---

# 153. API Naming Rules

Use a consistent style for:

```text
create
destroy
get
set
try
is
has
reset
validate
```

Examples:

```text
tryInverse()
isValid()
setTransform()
getVelocity()
```

Avoid API drift such as exposing one name in the header and implementing another.

---

# 154. Error Reporting

Use an explicit error model.

Possible categories:

```text
InvalidHandle
InvalidShape
InvalidParameter
CapacityExceeded
NumericalFailure
UnsupportedOperation
SerializationError
InternalInvariantFailure
```

Avoid using exceptions in simulation hot paths if the rest of Poko Engine does not use them there.

---

# 155. Logging

Physics diagnostics should use the Poko logging system.

Levels:

```text
Trace
Debug
Info
Warning
Error
Fatal
```

Physics warnings should identify:

```text
world
body
collider
shape
constraint
simulation step
```

where possible.

---

# 156. Statistics API

Expose runtime statistics:

```text
bodyCount
activeBodyCount
sleepingBodyCount
colliderCount
shapeCount
pairCount
contactCount
constraintCount
islandCount
queryCount
ccdCount
solverIterations
broadphaseHeight
memoryUsage
allocationCount
```

---

# 157. Capacity Management

Pools should support:

* initial capacity.
* growth.
* maximum capacity.
* reserve.
* shrink where safe.

Avoid uncontrolled doubling for massive physics worlds.

---

# 158. Hot/Cold Data Separation

Hot:

```text
position
rotation
velocity
inverse mass
inverse inertia
flags
```

Warm:

```text
contact impulses
constraint state
island links
```

Cold:

```text
debug metadata
editor state
user metadata
rare configuration
```

This separation should be designed before aggressive optimization.

---

# 159. Pointer Policy

Internal pointers may exist where they are justified.

They must not be used as persistent identity.

Persistent identity uses:

```text
Handle
ID
index/generation
```

This prevents stale pointer problems.

---

# 160. Container Policy

Prefer purpose-built storage for hot physics state.

Avoid heavy general-purpose structures in hot loops.

For each container choice document:

```text
why it exists
expected access pattern
allocation behavior
thread behavior
```

---

# 161. Temporary Memory

Per-frame scratch memory should be:

```text
PhysicsFrameArena
```

Cleared at frame boundaries.

Worker scratch:

```text
PhysicsWorkerArena
```

Avoid sharing mutable temporary buffers between workers.

---

# 162. Contact Memory

Contact/manifold allocation must have bounded behavior.

Avoid creating heap objects for every contact.

Prefer:

```text
pool
fixed-capacity arrays
inline small buffers
```

---

# 163. Broadphase Memory

Dynamic tree:

```text
NodePool
FreeList
```

should avoid per-node heap allocation.

---

# 164. Constraint Memory

Constraints should use stable handles and pooled storage.

Solver rows can be frame-temporary data.

---

# 165. Solver Memory

Separate:

```text
Constraint Definition
Persistent Runtime State
Frame Solver Data
```

This prevents solver scratch data from polluting persistent body storage.

---

# 166. Simulation State Machine

The world owns explicit state:

```text
Idle
Stepping
Paused
Resetting
Destroying
```

Prevent unsafe API calls during incompatible phases.

---

# 167. Safe API During Simulation

Define what is allowed during:

```text
BeforeStep
DuringStep
AfterStep
```

Example:

```text
get state
```

may be allowed during simulation.

While:

```text
destroy body
```

may be queued until the correct boundary.

---

# 168. Multiple Physics Worlds

Support multiple worlds simultaneously without global state.

Each world has:

```text
own allocator context
own timestep
own broadphase
own body storage
own events
own configuration
```

---

# 169. World Cloning

Useful for:

* prediction.
* testing.
* Studio previews.
* replay branching.

Architecture should permit future cloning/snapshotting without depending on raw pointer graphs.

---

# 170. Rollback Architecture Reservation

Full rollback networking does not need to be V1.

However, the architecture should not make it impossible.

Reserve:

```text
world snapshots
command history
deterministic stepping
state restore
```

---

# 171. Physics Prediction

Client prediction can use:

```text
snapshot
input commands
simulate
reconcile
```

The physics API should be compatible with replaying commands.

---

# 172. Interpolation

Rendering may interpolate:

```text
previous physics state
current physics state
render alpha
```

Physics itself remains fixed timestep.

---

# 173. Teleport Semantics

Define a dedicated teleport operation.

Teleport must specify:

* whether velocity is preserved.
* whether contacts are reset.
* whether CCD state is reset.
* whether broadphase is immediately updated.
* whether events are generated.

---

# 174. Kinematic Target Semantics

Kinematic bodies should support:

```text
set target transform
```

rather than forcing gameplay code to directly manipulate internal body state.

---

# 175. Force Field / Gameplay Extensions

Reserve an extension layer for optional gameplay physics:

```text
Buoyancy
Force Zones
Wind
Gravity Volumes
Explosion Impulses
Drag Volumes
Conveyors
```

These should be built on top of the core force/constraint/query APIs rather than deeply embedded into the solver.

---

# 176. Buoyancy

Optional A/AA gameplay feature:

* volume sampling.
* buoyancy force.
* drag.
* center of buoyancy.

Keep water simulation itself outside rigid-body physics.

---

# 177. Mechanical Systems

Optional extensions:

```text
gears
motors
rope systems
springs
winches
mechanisms
```

Use generic constraints where possible.

---

# 178. Soft Body Reservation

Do not make deformable/soft-body simulation part of the core V1 scope.

Reserve an extension boundary for future:

```text
SoftBodyWorld
Cloth
Deformables
```

These should not complicate rigid-body architecture.

---

# 179. Fluid Reservation

Do not build a fluid simulator into the core engine.

Reserve interfaces for future interaction:

```text
water volume
buoyancy
drag
surface queries
```

---

# 180. Destruction / Fracture Reservation

Do not embed mesh destruction into the rigid-body core.

Reserve support for future:

```text
fracture asset
fragment creation
compound replacement
```

using normal shape/body APIs.

---

# 181. Collision Geometry Simplification

Provide optional runtime simplification:

```text
high-detail visual mesh
 ↓
physics proxy
```

Physics should not normally collide against render triangles unless explicitly requested.

---

# 182. Collision Proxy Generation

Asset cooking can produce:

```text
box proxy
capsule proxy
convex proxy
compound proxy
triangle mesh proxy
```

Developers can select the desired quality.

---

# 183. Physics Streaming Data

Cooked assets should support streaming:

```text
collision data
BVH chunks
heightfield chunks
compound children
```

Only load what is required.

---

# 184. Collision Cache

Optional cached data:

```text
transformed bounds
support points
mass properties
shape-specific acceleration structures
```

Invalidate caches when source parameters change.

---

# 185. Broadphase Coherence

Take advantage of temporal coherence:

```text
previous AABB
fat AABB
previous pair
previous manifold
previous simplex direction
```

where safe.

---

# 186. Narrowphase Coherence

Allow optional warm-starting for algorithms:

* previous separating axis.
* previous GJK direction.
* previous feature pair.

Do not allow cache corruption to affect correctness.

---

# 187. Solver Warm Starting

Persist previous impulses by contact/constraint identity.

Use warm-start factor.

Validate transferred impulses against current limits.

---

# 188. Sleep/Wake Coherence

Waking behavior must propagate through:

```text
contact graph
constraint graph
moving platforms
external forces
teleports
kinematic targets
queries requiring activation if explicitly requested
```

---

# 189. Contact Cache Limits

Manifold cache must have bounded memory.

When over capacity:

```text
eviction policy
diagnostic
graceful fallback
```

No unbounded growth.

---

# 190. Collision Pair Limits

Broadphase pair buffers must also have bounded behavior.

A pathological scene should not produce an uncontrolled allocation explosion.

---

# 191. DoS Resistance

Because Poko can run untrusted games/requests, physics must guard against:

```text
body explosions
massive constraint counts
query floods
tiny timestep abuse
huge world coordinates
degenerate geometry
CCD abuse
manifold explosions
```

Use budgets and rate/complexity controls.

---

# 192. Game Script Integration

Mute sees high-level physics APIs.

Example conceptual API:

```text
body:applyImpulse(...)
body:getVelocity()
body:setLinearVelocity(...)
workspace:raycast(...)
constraint:setMotor(...)
```

The Mute API must not expose internal arrays/pointers.

---

# 193. Capability Integration

Physics APIs must respect Mute capabilities.

Examples:

```text
CLIENT
SERVER
SHARED
STUDIO
PLUGIN
```

Authority-sensitive operations cannot be performed from unauthorized contexts.

---

# 194. Reflection / Metadata

Physics types should integrate with Poko's metadata system.

Metadata can define:

```text
property type
default
limits
unit
serialization
replication
editor visibility
documentation
```

This allows Studio and Mute tooling to use the same source of truth.

---

# 195. Studio Property Integration

Example:

```text
RigidBody
    Mass
    Friction
    Restitution
    CollisionLayer
    CollisionMask
    CCD
    Sleeping
```

Properties should update the real physics runtime.

---

# 196. Script/Property Synchronization

Studio should treat physics properties as the same underlying object state.

Avoid maintaining separate fake physics properties.

---

# 197. Physics Asset Browser

Studio should show:

```text
collision shape
convex hull
mesh collision
heightfield
material
ragdoll asset
vehicle definition
```

with preview information.

---

# 198. Physics Cooking Diagnostics

Studio should explain:

```text
convex hull failed
degenerate mesh
too many triangles
invalid winding
large collider
poor collision approximation
```

rather than silently failing.

---

# 199. API Versioning

Public physics APIs must have stable versioning.

Internal algorithms can change.

Public semantics should only change intentionally.

---

# 200. Runtime Compatibility

Cooked physics assets must declare compatibility with:

```text
physics format version
engine version
physics runtime version
```

Do not load incompatible binary data silently.

---

# 201. Feature Flags

Physics features should support explicit feature configuration.

Examples:

```text
enableCCD
enableSleeping
enableVehicles
enableRagdolls
enableAdvancedQueries
enableDeterminism
```

Avoid excessive feature flags inside the hottest loops.

---

# 202. Build-Time Configuration

Support:

```text
Debug
Development
Shipping
Testing
Benchmark
```

Debug can include:

```text
validation
assertions
extra diagnostics
```

Shipping removes unnecessary checks.

---

# 203. Reference Implementations

For difficult algorithms, keep simple reference versions for tests.

Examples:

```text
brute-force broadphase
scalar math
simple collision implementation
single-thread solver
```

Production paths can be optimized versions.

Reference implementations are test or diagnostic backends, not necessarily shipped paths.

---

# 204. Scalar vs SIMD Validation

Test:

```text
Scalar result
SIMD result
```

within accepted numerical tolerance.

This is especially important for mobile vs desktop behavior.

---

# 205. Optimizer Safety

Physics optimizations must preserve:

```text
collision results
solver stability
event semantics
determinism promises
API behavior
```

Optimization is only accepted after correctness tests.

---

# 206. Compiler Optimization

Use compiler optimizations strategically:

* LTO where appropriate.
* function specialization where useful.
* inlining based on profiling.
* hot/cold separation.
* PGO when real benchmark profiles exist.

Do not rely on optimization flags to compensate for bad data structures.

---

# 207. PGO

Profile:

```text
hot collision pairs
hot solver loops
hot query paths
hot shapes
hot constraints
```

Use profiles to tune:

```text
inlining
layout
dispatch
branching
SIMD
```

---

# 208. Branch Reduction

Optimize only after profiling.

Potential techniques:

```text
type-specialized loops
branchless math
precomputed flags
dispatch tables
```

Do not make algorithms unreadable merely to remove insignificant branches.

---

# 209. Cache-Aware Dispatch

Common shape pair paths should be cheap:

```text
SphereSphere
SpherePlane
BoxBox
CapsuleCapsule
```

Avoid dynamic polymorphism in the innermost collision loop where possible.

---

# 210. Virtual Dispatch Policy

Virtual interfaces are acceptable at subsystem boundaries.

Hot pair/solver loops should prefer:

```text
tables
templates
specialized functions
compact type IDs
```

where profiling justifies it.

---

# 211. Physics Job Graph

Physics stages should integrate with the Poko Job Graph.

Represent:

```text
BroadphaseUpdate
PairGeneration
Narrowphase
ManifoldMerge
IslandBuild
ConstraintPrepare
VelocitySolve
PositionSolve
Integration
Events
```

as explicit jobs/dependencies.

---

# 212. Synchronization Boundaries

Document synchronization after:

```text
commands
broadphase
narrowphase
manifold
islands
solver
integration
events
```

Minimize global barriers.

---

# 213. Worker-Local Data

Use worker-local:

```text
contact buffers
pair buffers
scratch memory
temporary manifold data
solver scratch
```

Merge at controlled boundaries.

---

# 214. Debug Determinism

Debug visualization itself should never alter simulation.

No debug-only sorting, logging, or drawing code should unintentionally mutate physics state.

---

# 215. Logging Cost Control

Heavy logs should be disabled in shipping hot paths.

Use:

```text
conditional diagnostics
sampling
ring buffers
```

for recurring numerical issues.

---

# 216. Physics Capture

Add an optional frame capture:

```text
PhysicsFrameCapture
```

Containing:

```text
body states
colliders
contacts
constraints
solver statistics
queries
events
```

Can be saved and inspected in Studio.

---

# 217. Frame Comparison

Studio should compare two captured physics frames.

Useful for:

```text
before bug fix
after bug fix
Poko vs reference
client vs authority
frame N vs frame N+1
```

---

# 218. Physics Scene Import Tests

Every major imported physics asset type gets an automated test.

Examples:

```text
simple box
complex mesh
large mesh
convex mesh
heightfield
compound shape
```

---

# 219. Long-Term Stability Tests

Run simulations for:

```text
10 minutes
1 hour
long soak test
```

where practical.

Watch for:

```text
drift
memory growth
handle exhaustion
contact cache growth
floating point instability
sleep/wake loops
```

---

# 220. Regression Corpus

Maintain permanent difficult scenes:

```text
HugeMassRatio
DeepStack
ThinObjects
FastProjectile
RotatingBox
DenseRagdoll
VehiclePile
CharacterStairs
ManyTriggers
LargeWorld
```

Every release runs the corpus.

---

# 221. Release Qualification

A physics release is not complete merely because all features compile.

Qualification requires:

```text
unit tests pass
property tests pass
integration tests pass
stress tests pass
fuzz tests pass
determinism tests pass where applicable
benchmarks within budget
sanitizers pass
Doxygen builds
public API compiles
runtime package loads
```

---

# 222. Phase 0 — Architecture

Before implementing algorithms:

* finalize module boundaries.
* finalize data ownership.
* finalize handles.
* finalize public API.
* finalize units/conventions.
* finalize simulation pipeline.
* finalize thread ownership.
* finalize error semantics.
* finalize memory policy.
* finalize serialization strategy.
* finalize testing framework.

Deliverable:

```text
Compiling but mostly-empty production architecture.
```

---

# 223. Phase 1 — Math and Core

Implement:

```text
Vector
Quaternion
Matrix
Transform
AABB
Ray
Plane
handles
pools
tolerances
world configuration
```

Immediately add tests.

Do not proceed with untrusted math.

---

# 224. Phase 2 — Shapes and Geometry

Implement:

```text
Sphere
Box
Capsule
Plane
Convex Hull
Triangle
Compound
```

Then mass properties and support functions.

---

# 225. Phase 3 — Broadphase

Implement:

```text
Broadphase interface
Dynamic AABB Tree
```

Then:

```text
SAP
Spatial Grid
```

only where useful.

Validate against brute force.

---

# 226. Phase 4 — Narrowphase

Implement:

```text
collision dispatcher
analytic pairs
GJK
EPA
SAT
contact generation
```

Create pair matrix tests.

---

# 227. Phase 5 — Contact System

Implement:

```text
ContactPoint
Manifold
ManifoldCache
ContactMatching
ContactReduction
```

Then warm starting.

---

# 228. Phase 6 — Rigid Bodies

Implement:

```text
BodyPool
ColliderPool
material
forces
integration
gravity
sleeping-disabled baseline
```

Get basic dynamic simulation stable first.

---

# 229. Phase 7 — Solver

Implement:

```text
constraint rows
contact constraints
velocity solver
position solver
friction
restitution
warm start
```

Then stacking validation.

---

# 230. Phase 8 — Constraints and Joints

Implement:

```text
Distance
Fixed
BallSocket
Hinge
Prismatic
Spring
Rope
```

Then:

```text
motors
limits
breakable constraints
```

---

# 231. Phase 9 — Islands and Sleeping

Implement:

```text
graph
island build
wake propagation
sleep
parallel island execution
```

---

# 232. Phase 10 — Queries

Implement:

```text
Raycast
SphereCast
CapsuleCast
BoxCast
Overlap
ShapeCast
```

Use shared broadphase/narrowphase infrastructure.

---

# 233. Phase 11 — Triggers and Events

Implement:

```text
trigger tracking
contact tracking
event buffer
event dispatch
```

---

# 234. Phase 12 — CCD

Implement:

```text
speculative contacts
sweeps
TOI
CCD budgets
```

Then add more advanced rotational CCD only after necessity is proven.

---

# 235. Phase 13 — Character Controller

Implement:

```text
capsule
sweep
ground
slope
step
slide
platforms
```

---

# 236. Phase 14 — Ragdoll

Implement:

```text
body/bone mapping
joints
limits
motors
spring/damping
animation blending
```

---

# 237. Phase 15 — Vehicles

Implement:

```text
raycast wheels
suspension
friction
steering
braking
power
anti-roll
```

---

# 238. Phase 16 — Cooking and Assets

Implement:

```text
convex cooking
mesh BVH
heightfield
compound cooking
PKX physics resources
runtime loading validation
```

---

# 239. Phase 17 — Debugging and Studio

Implement:

```text
debug draw
physics inspector
contact debugger
constraint debugger
broadphase debugger
capture/replay
single-step controls
```

---

# 240. Phase 18 — Performance

Only after correctness:

```text
SoA
SIMD
cache optimization
pool tuning
parallelism
job integration
PGO
specialization
```

---

# 241. Phase 19 — Authority Integration

Implement:

```text
authority physics subset
request validation
server-owned bodies
snapshot support
desync diagnostics
```

---

# 242. Phase 20 — Production Hardening

Final pass:

```text
fuzzing
sanitizers
long soak tests
capacity tests
serialization corruption tests
network abuse tests
benchmark regression
API review
documentation review
```

---

# 243. Definition of Done — Math

Math is complete only when:

```text
all operations implemented
all edge cases handled
numerical policy documented
property tests passing
randomized tests passing
scalar/SIMD validation passing
```

---

# 244. Definition of Done — Collision

Collision is complete only when:

```text
pair matrix covered
degenerate geometry handled
false positives tested
false negatives tested
manifold quality tested
broadphase validated against oracle
```

---

# 245. Definition of Done — Solver

Solver is complete only when:

```text
contacts stable
friction stable
restitution stable
stacks stable
joints stable
warm starting stable
extreme mass ratios bounded
performance measured
```

---

# 246. Definition of Done — World

World is complete only when:

```text
handles safe
lifecycle safe
commands safe
multiple worlds safe
serialization safe
sleeping stable
threading validated
```

---

# 247. Definition of Done — Production Engine

The entire engine is production-ready only when:

```text
correctness proven by tests
performance measured on target devices
memory behavior bounded
threading validated
API stable
debugging available
serialization versioned
replay available
authority integration validated
shipping build hardened
```

---

# 248. Recommended Repository Structure

```text
engine/
└── physics/
    ├── include/
    │   └── poko/
    │       └── physics/
    │           ├── physics_world.h
    │           ├── rigid_body.h
    │           ├── collider.h
    │           ├── shape.h
    │           ├── material.h
    │           ├── constraint.h
    │           ├── query.h
    │           └── physics_events.h
    │
    ├── src/
    │   ├── core/
    │   │   ├── handles/
    │   │   ├── memory/
    │   │   ├── configuration/
    │   │   └── validation/
    │   │
    │   ├── math/
    │   │   ├── vector/
    │   │   ├── matrix/
    │   │   ├── quaternion/
    │   │   ├── transform/
    │   │   └── aabb/
    │   │
    │   ├── shapes/
    │   ├── materials/
    │   ├── bodies/
    │   ├── colliders/
    │   ├── broadphase/
    │   ├── narrowphase/
    │   ├── contacts/
    │   ├── constraints/
    │   ├── solver/
    │   ├── simulation/
    │   ├── ccd/
    │   ├── character/
    │   ├── ragdoll/
    │   ├── vehicle/
    │   ├── queries/
    │   ├── triggers/
    │   ├── events/
    │   ├── world/
    │   ├── cooking/
    │   ├── serialization/
    │   ├── replay/
    │   ├── streaming/
    │   ├── threading/
    │   ├── debug/
    │   ├── profiling/
    │   ├── platform/
    │   └── reference/
    │
    └── tests/
        ├── unit/
        ├── property/
        ├── geometry/
        ├── collision/
        ├── solver/
        ├── integration/
        ├── regression/
        ├── fuzz/
        ├── differential/
        ├── determinism/
        ├── replay/
        ├── stress/
        └── performance/
```

---

# 249. Architecture Rules

The following rules are treated as permanent unless intentionally changed through an architecture review:

### Rule 1

Public Poko Physics APIs never expose third-party backend types.

### Rule 2

Physics identity uses generation-safe handles, not raw pointers.

### Rule 3

Simulation hot paths do not perform uncontrolled general-purpose allocations.

### Rule 4

External gameplay mutations enter physics through defined synchronization/command mechanisms.

### Rule 5

Physics events are buffered and dispatched at safe boundaries.

### Rule 6

Every major algorithm has a testable contract.

### Rule 7

Every important numerical tolerance has a documented meaning.

### Rule 8

Performance optimizations must be backed by profiling or benchmarks.

### Rule 9

Reference implementations are allowed for validation and debugging.

### Rule 10

The renderer, Studio, PokoX, and Authority consume the same core physics architecture rather than parallel fake implementations.

### Rule 11

A difficult feature should extend an existing subsystem rather than duplicate it.

### Rule 12

Correctness comes before optimization.

### Rule 13

Deterministic behavior is only promised where it is actually tested.

### Rule 14

The engine must remain debuggable after optimization.

### Rule 15

Every persistent binary format is versioned and pointer-free.

---

# 250. Final Architecture

The final conceptual architecture is:

```text
                     POKO PHYSICS
                           │
          ┌────────────────┴────────────────┐
          │                                 │
      Public API                        Tooling API
          │                                 │
          └────────────────┬────────────────┘
                           │
                    PhysicsWorld
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                  │
    Body/Collider       Shape/Material      Commands
        │                  │                  │
        └──────────────────┼──────────────────┘
                           │
                       Broadphase
                           │
                     Pair Generation
                           │
                      Narrowphase
                           │
                    Contact Generation
                           │
                   Manifold Persistence
                           │
                    Island Construction
                           │
                  Constraint Preparation
                           │
                       Warm Start
                           │
                    Velocity Solver
                           │
                    Position Solver
                           │
                         CCD
                           │
                      Integration
                           │
                     Sleep / Wake
                           │
                       Event Buffer
                           │
          ┌────────────────┼─────────────────┐
          │                │                 │
      Game/Engine       Authority         Queries
          │                │                 │
        Mute          Validation        Characters
      Studio            Replay            Vehicles
      PokoX            Snapshots           AI
```

The implementation then sits on:

```text
Memory
Handles
Job System
SIMD
Platform Layer
Serialization
Profiling
Validation
```

with optional reference validation through:

```text
Jolt Reference Backend
```

and optional future extension points for:

```text
soft bodies
fluids
destruction
advanced articulation
advanced vehicle simulation
```

without making those systems dependencies of the rigid-body core.

---

# 251. The Actual Development Philosophy

The engine should be built in this order:

```text
Architecture
    ↓
Math correctness
    ↓
Geometry correctness
    ↓
Collision correctness
    ↓
Stable contacts
    ↓
Stable solver
    ↓
Stable world
    ↓
Gameplay features
    ↓
Tooling
    ↓
Multithreading
    ↓
SIMD/cache optimization
    ↓
Production hardening
```

Not:

```text
write every feature
    ↓
hope it works
    ↓
optimize everything
```

The physics engine should become trustworthy subsystem by subsystem.

---

# 252. Final Target

The finished Poko Physics Engine should be capable of handling:

```text
ordinary gameplay physics
stacked rigid bodies
vehicles
characters
ragdolls
mechanical joints
moving platforms
projectiles
triggers
raycasts
shape casts
large scenes
streamed worlds
authority verification
Studio editing
debugging
replay
performance profiling
```

while retaining:

```text
stable APIs
bounded memory
safe handles
predictable execution
strong numerical behavior
thread-safe architecture
target-platform performance
```

That is the full architecture I would build around the feature plan.
I would also make three changes to the original plan itself while adding this: treat the current GJK pseudocode as conceptual rather than implementation-ready, make numerical error handling explicitly reject/quarantine bad state instead of silently replacing NaNs with zero, and change the blanket “all physics types 16-byte aligned” rule to targeted alignment for SIMD-relevant data. Those changes make the architecture stronger without changing your Indie/A/AA scope.