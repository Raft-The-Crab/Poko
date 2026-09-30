# Poko Physics Engine - Production Implementation Plan

## Overview

This document outlines the complete production implementation plan for the Poko Physics Engine, targeting:
- **Indie A**: 100% (fully functional, robust, well-tested)
- **Double A**: 100% (production-grade, optimized, scalable)
- **Triple A**: 15% (selected advanced features where they provide clear value)

## Table of Contents

1. [Production Principles](#production-principles)
2. [Math Module](#math-module)
3. [Collision Shapes](#collision-shapes)
4. [Broadphase Collision Detection](#broadphase-collision-detection)
5. [Narrowphase Collision Detection](#narrowphase-collision-detection)
6. [Constraint Solver](#constraint-solver)
7. [Joints](#joints)
8. [Physics World and Rigid Body System](#physics-world-and-rigid-body-system)
9. [Sleeping and Island Management](#sleeping-and-island-management)
10. [Continuous Collision Detection (CCD)](#continuous-collision-detection-ccd)
11. [Character Controller](#character-controller)
12. [Ragdoll System](#ragdoll-system)
13. [Vehicle Physics](#vehicle-physics)
14. [Raycasting and Triggers](#raycasting-and-triggers)
15. [Debug Visualization](#debug-visualization)
16. [Numerical Robustness](#numerical-robustness)
17. [Performance Optimization](#performance-optimization)
18. [Testing Strategy](#testing-strategy)

---

## Production Principles

### Numerical Robustness

All math operations must handle:
- NaN/Inf propagation
- Near-singular matrices
- Precision loss accumulation
- Denormalized numbers
- Cancellation errors

**Tolerance Policy**:
- Use relative tolerances for comparisons
- Use absolute tolerances for near-zero values
- Scale tolerances with object size
- Document all epsilon values

### Memory Layout

**SoA (Structure of Arrays)** for hot paths:
- Position data: `struct Positions { float x[N]; float y[N]; float z[N]; }`
- Velocity data: `struct Velocities { float vx[N]; float vy[N]; float vz[N]; }`
- Enables SIMD vectorization
- Improves cache locality

**AoS (Array of Structures)** for cold paths:
- Collision pairs
- Contact manifolds
- Memory overhead acceptable

### Alignment

All physics types aligned to 16 bytes:
- Prevents false sharing
- Enables SIMD operations
- Improves memory throughput

### Fixed-Point Considerations

For determinism across platforms:
- Consider 32.32 fixed-point for positions (large world)
- Keep floating-point for performance-critical paths
- Document floating-point behavior
- Avoid platform-specific optimizations

---

## Math Module

### Vector3

**Production Features**:
- Cross-product matrix: CORRECTED for column-major layout
- Dot product for projections
- Length squared (avoids sqrt)
- Normalization with zero-length protection
- Distance calculations
- Vector operations (add, sub, mul, div)
- Direction constants (up, down, forward, back, right, left)

**Cross-Product Matrix Bug Fix**:
```cpp
// WRONG (before):
return Matrix3x3(0.0f, -vec.z, vec.y, vec.z, 0.0f, -vec.x, -vec.y, vec.x, 0.0f);

// CORRECT (after):
return Matrix3x3(0.0f, vec.z, -vec.y, -vec.z, 0.0f, vec.x, vec.y, -vec.x, 0.0f);
```

**Numerical Tolerances**:
- `EPSILON = 1e-6f` for general comparisons
- `LENGTH_EPSILON = 1e-4f` for normalization
- `DOT_EPSILON = 1e-6f` for parallel checks

### Matrix3x3

**Production Features**:
- Column-major storage (graphics API compatible)
- Matrix multiplication
- Matrix-vector multiplication
- Transpose operation
- Determinant calculation
- Inverse with `tryInverse()` for numerical safety
- Diagonal matrix constructor
- Inertia tensor constructor
- Cross-product matrix (CORRECTED)
- Identity and zero checks

**Inverse Implementation**:
```cpp
// BAD: Silent failure
Matrix3x3 inverse() const {
    if (std::abs(det) < 0.0001f) return ZERO;
    // ...
}

// GOOD: Explicit failure handling
bool tryInverse(Matrix3x3& result, float epsilon = 0.0001f) const noexcept {
    float det = determinant();
    if (std::abs(det) < epsilon) return false;
    // ... compute inverse
    return true;
}
```

**Additional Methods**:
- Orthonormal validation: `isOrthonormal(epsilon)`
- Orthonormalization: `orthonormalize()`
- Symmetric check: `isSymmetric(epsilon)`
- Diagonal check: `isDiagonal(epsilon)`
- Basis extraction: `getBasisX()`, `getBasisY()`, `getBasisZ()`
- Basis construction: `fromBasis(x, y, z)`

### Quaternion

**Production Features**:
- Axis-angle constructor
- Euler angles constructor (yaw, pitch, roll)
- Quaternion multiplication (composition)
- Scalar multiplication and division
- Dot product, length, normalization
- Conjugate and inverse
- Vector rotation: `rotateVector()`
- Spherical linear interpolation (slerp) - constant angular velocity
- Linear interpolation (lerp) - faster but not constant velocity
- Get axis and angle from quaternion
- Identity and normalized checks

**Slerp Implementation**:
```cpp
Quaternion slerp(const Quaternion& q1, const Quaternion& q2, float t) {
    float dot = q1.dot(q2);
    
    // Take shorter path
    Quaternion q2Temp = q2;
    if (dot < 0.0f) {
        q2Temp = -q2;
        dot = -dot;
    }
    
    // Linear interpolation for very close quaternions
    if (dot > 0.9995f) {
        return (q1 + (q2Temp - q1) * t).normalized();
    }
    
    float theta0 = std::acos(std::clamp(dot, -1.0f, 1.0f));
    float theta = theta0 * t;
    float sinTheta = std::sin(theta);
    float sinTheta0 = std::sin(theta0);
    
    float s0 = std::cos(theta) - dot * sinTheta / sinTheta0;
    float s1 = sinTheta / sinTheta0;
    
    return q1 * s0 + q2Temp * s1;
}
```

### AABB (Axis-Aligned Bounding Box)

**Production Features**:
- Min-max and center-extent constructors
- Sphere and box constructors
- Center, extent, size, volume, surface area calculations
- Valid and empty checks
- Expand to include point or AABB
- Fat AABB expansion (margin for broadphase)
- Translate and scale operations
- Point containment test
- AABB-AABB intersection test
- AABB-AABB containment test
- Ray-AABB intersection (Slab method)
- Union and intersection operations

**Ray-AABB Slab Method**:
```cpp
bool rayIntersect(const Vector3& origin, const Vector3& direction,
                   float& tMin, float& tMax) const noexcept {
    tMin = 0.0f;
    tMax = 1e30f;
    
    for (int i = 0; i < 3; ++i) {
        float minVal = (&min.x)[i];
        float maxVal = (&max.x)[i];
        float originVal = (&origin.x)[i];
        float dirVal = (&direction.x)[i];
        
        if (std::abs(dirVal) < 0.0001f) {
            if (originVal < minVal || originVal > maxVal) return false;
        } else {
            float t1 = (minVal - originVal) / dirVal;
            float t2 = (maxVal - originVal) / dirVal;
            
            if (t1 > t2) std::swap(t1, t2);
            
            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);
            
            if (tMin > tMax) return false;
        }
    }
    
    return true;
}
```

---

## Collision Shapes

### Sphere

**Production Features**:
- Center and radius
- AABB calculation (exact)
- Volume calculation: `(4/3) × π × r³`
- Mass from density: `mass = volume × density`
- Inertia tensor: `I = (2/5) × m × r²` (diagonal)
- Point containment test
- Sphere-sphere intersection test

### Box (OBB - Oriented Bounding Box)

**Production Features**:
- Center, half-extents, orientation
- AABB calculation (loose bound for OBB)
- Volume calculation: `8 × hx × hy × hz`
- Mass from density
- Inertia tensor (diagonal for axis-aligned):
  - `Ixx = m/12 × (hy² + hz²)`
  - `Iyy = m/12 × (hx² + hz²)`
  - `Izz = m/12 × (hx² + hy²)`
- Get 8 corners
- Get 6 face normals
- SAT (Separating Axis Theorem) collision detection

**OBB AABB Calculation**:
```cpp
AABB getAABB() const {
    if (orientation.isIdentity()) {
        return AABB::fromCenterExtent(center, halfExtents);
    }
    
    // For oriented boxes, compute projected extents
    Vector3 corners[8];
    getCorners(corners);
    
    AABB result;
    for (int i = 0; i < 8; ++i) {
        result.expand(corners[i]);
    }
    return result;
}
```

### Capsule

**Production Features**:
- Center, radius, height, orientation
- AABB calculation (from two spheres)
- Volume calculation: `π × r² × h + (4/3) × π × r³` (cylinder + sphere)
- Mass from density
- Inertia tensor (approximate: cylinder + sphere)
- Get capsule endpoints
- Line segment collision tests

### Plane

**Production Features**:
- Normal and distance from origin
- Point-normal constructor
- Three-point constructor
- Distance from point to plane
- Project point onto plane
- Point containment test
- Side determination (front/back)

---

## Broadphase Collision Detection

### Dynamic AABB Tree (BVH)

**Production Features**:
- Self-balancing bounding volume hierarchy
- Good for dynamic scenes with moving objects
- Complexity: Insert O(log n), Remove O(log n), Update O(log n), Query O(log n + m)
- Node pooling to avoid allocations
- Fat AABBs to reduce rebalancing
- Stack-based query to avoid recursion

**Node Structure**:
```cpp
struct Node {
    AABB aabb;
    uint64_t userData;
    uint32_t parent;
    uint32_t child1;
    uint32_t child2;
    int32_t height;
    bool isLeaf;
};
```

**Balance Heuristics**:
- Surface area heuristic (SAH) for insertion
- Rotate operations for rebalancing
- Height-based rotation triggers

**Performance Considerations**:
- Use free list for node allocation
- Pre-allocate node pool (default 16, grow ×2)
- Fat AABB margin (1-2% of AABB size)
- Bulk operations for static objects

### Sweep and Prune (SAP)

**Production Features**:
- Sort bodies along one axis (typically longest axis)
- Sweep for overlapping intervals
- Complexity: O(n log n) for sort, O(n) for sweep
- Good for mostly static scenes
- Incremental updates using insertion sort

**Implementation Notes**:
- Maintain sorted lists for X, Y, Z axes
- On frame start, quick-sort along primary axis
- For moved objects, use insertion sort to update
- Cache-friendly for spatial coherence

### Spatial Grid

**Production Features**:
- Uniform grid partitioning
- O(1) lookup for spatial queries
- Good for uniform object distribution
- Cell size based on average object size
- Grid coordinate hashing

**Implementation Notes**:
- Use spatial hash for sparse grids
- Dynamic grid resizing based on object count
- Multi-level grids for large variance in object sizes

---

## Narrowphase Collision Detection

### GJK (Gilbert-Johnson-Keerl)

**Production Features**:
- Convex shape collision detection
- Iterative algorithm with simplex
- No need for face normals
- Works for any convex shape
- Early termination for disjoint shapes

**Algorithm**:
```cpp
bool GJK(const Shape& a, const Shape& b, Vector3& contactNormal, float& penetration) {
    Simplex simplex;
    Vector3 direction = initialDirection(a, b);
    
    for (int i = 0; i < MAX_ITERATIONS; ++i) {
        Vector3 supportA = a.support(direction);
        Vector3 supportB = b.support(-direction);
        
        if (b.distanceTo(supportA) < 0.0f) {
            return false; // Separating axis found
        }
        
        Vector3 newPoint = supportA - supportB;
        if (simplex.add(newPoint)) {
            direction = simplex.getSearchDirection();
        } else {
            direction = newPoint.normalized();
        }
    }
    
    // Origin is inside simplex - shapes intersect
    return true;
}
```

### SAT (Separating Axis Theorem)

**Production Features**:
- Test face normals of both shapes
- Test edge cross products
- Early termination on first separating axis
- Penetration depth calculation
- Contact manifold generation

**For Box-Box Collision**:
- Test 6 face normals from each box (12 axes)
- Test 9 edge cross products (15 axes total)
- Find minimum penetration
- Use face normal of minimum penetration axis

### Contact Manifolds

**Production Features**:
- Up to 4 contact points per pair
- Persistent contacts (warm starting)
- Contact reduction (keep deepest contacts)
- Normal and tangent basis generation
- Friction impulses (2 tangent directions)
- Restitution handling

**Contact Point Structure**:
```cpp
struct ContactPoint {
    Vector3 position;      // Contact position in world space
    Vector3 normal;        // Contact normal (from A to B)
    float penetration;     // Penetration depth
    float normalImpulse;   // Accumulated normal impulse
    float tangentImpulse[2]; // Accumulated tangent impulses
};
```

---

## Constraint Solver

### Sequential Impulse Solver

**Production Features**:
- Projected Gauss-Seidel iteration
- Warm starting with previous impulses
- Accumulated impulse clamping
- Baumgarte stabilization (position correction)
- Configurable velocity and position iterations
- Friction constraints (Coulomb friction)

**Configuration**:
```cpp
struct SolverConfig {
    uint32_t velocityIterations = 8;
    uint32_t positionIterations = 3;
    float baumgarte = 0.2f;  // Position correction factor
    float slop = 0.01f;        // Allowed penetration
    float warmStartFactor = 0.8f;  // Impulse retention
};
```

**Velocity Solver**:
```cpp
void solveVelocityConstraints(ContactManifold& manifold) {
    for (ContactPoint& contact : manifold.contacts) {
        // Compute Jacobian
        // Compute effective mass
        // Compute lambda
        // Clamp lambda
        // Apply impulse
        // Update accumulated impulse
    }
}
```

**Position Solver**:
```cpp
void solvePositionConstraints(ContactManifold& manifold) {
    for (ContactPoint& contact : manifold.contacts) {
        float correction = std::max(contact.penetration - slop, 0.0f);
        correction *= baumgarte;
        
        // Apply position correction proportional to inverse mass
        // Move bodies apart along contact normal
    }
}
```

### Constraint Types

**Distance Constraint**:
- Fixed distance between two points
- Error: `error = distance(current, target) - restLength`
- Jacobian: direction vector

**Hinge Constraint**:
- Fixed angle around one axis
- Error: `error = angle(current, target) - restAngle`
- Jacobian: axis vector

**Spherical (Ball Socket) Constraint**:
- Fixed distance, free rotation
- Same as distance constraint

**Fixed Constraint**:
- Fixed position and rotation
- Zero velocity and angular velocity

**Slider (Prismatic) Constraint**:
- Free movement along one axis
- Locked movement on other axes

---

## Joints

### Distance Joint

**Production Features**:
- Connects two bodies at fixed distance
- Spring-damper optional
- Breakable threshold
- Soft limit enforcement

### Hinge Joint

**Production Features**:
- Rotation around one axis
- Lower and upper angle limits
- Motor for driven rotation
- Spring-damper for compliance

### Spherical Joint

**Production Features**:
- Ball-and-socket connection
- Free rotation in all axes
- Cone limit optional
- Spring-damper for compliance

### Fixed Joint

**Production Features**:
- Completely locks relative transform
- Used for rigid assemblies
- Zero degrees of freedom

---

## Physics World and Rigid Body System

### Rigid Body

**Production Features**:
- Position, linear velocity, angular velocity
- Rotation (quaternion)
- Mass, inverse mass
- Inertia tensor, inverse inertia tensor
- Collision layer and mask
- Static/kinematic/dynamic flags
- Sleeping state
- User data handle

**Body Handle System**:
```cpp
struct BodyHandle {
    uint32_t index;
    uint32_t generation;
};

// Generation validation prevents stale handles
bool isValid(BodyHandle handle) const {
    return handle.generation == generations[handle.index];
}
```

### Physics World

**Production Features**:
- Fixed timestep simulation with accumulator
- Semi-implicit Euler integration
- Broadphase manager
- Narrowphase manager
- Constraint solver
- Body storage with handles
- Event callbacks
- Query interfaces

**Simulation Loop**:
```cpp
void World::step(float deltaTime) {
    static float accumulator = 0.0f;
    accumulator += deltaTime;
    
    while (accumulator >= fixedDeltaTime) {
        float dt = fixedDeltaTime;
        
        // Integration
        integrateVelocities(dt);
        
        // Collision detection
        updateBroadphase();
        detectCollisions();
        
        // Constraint solving
        solveConstraints(dt);
        
        // Position integration
        integratePositions(dt);
        
        accumulator -= dt;
    }
}
```

---

## Sleeping and Island Management

### Sleeping

**Production Features**:
- Sleep threshold (velocity-based)
- Sleep timer
- Force threshold (keep sleeping if forces are small)
- Auto-sleep
- Manual wake-up on interaction

**Sleep Conditions**:
```cpp
bool shouldSleep(RigidBody& body) {
    if (!body.isSleepingEnabled) return false;
    if (body.getVelocity().length() < sleepThreshold) {
        body.sleepTimer += deltaTime;
        return body.sleepTimer > sleepTime;
    }
    body.sleepTimer = 0.0f;
    return false;
}
```

### Island Management

**Production Features**:
- Graph-based island construction
- Connected bodies form islands
- Static bodies are boundary nodes
- Island-local solving
- Wake propagation through graph
- Island-level sleep

**Island Graph**:
```cpp
struct Island {
    std::vector<BodyHandle> bodies;
    bool isSleeping;
    float sleepTimer;
};

void buildIslands() {
    // Build graph from contacts
    // Find connected components
    // Create islands
    // Solve each island independently
}
```

---

## Continuous Collision Detection (CCD)

### Speculative Contacts

**Production Features**:
- Extrude shapes along velocity
- Test for intersection at TOI (time of impact)
- Apply contact at TOI
- Reduce simulation instability for fast-moving objects

**CCD Configuration**:
```cpp
struct CCDConfig {
    bool enabled = true;
    float motionThreshold = 0.1f;   // Velocity threshold
    float maxSubsteps = 4;           // Maximum CCD iterations
    float timeBudget = 0.002f;       // Maximum CCD time per frame
};
```

### Swept Sphere

**Production Features**:
- Ray-cast with radius
- Cheap approximation for complex shapes
- Good for character controllers
- Fast sweep along velocity vector

---

## Character Controller

### Kinematic Character Controller

**Production Features**:
- Capsule collision shape
- Collide-and-slide movement
- Slope limit (max walkable slope)
- Step offset (step up stairs)
- Ground detection
- Moving platform handling
- Auto-step

**Collide-and-Slide**:
```cpp
Vector3 collideAndSlide(Vector3 velocity, float deltaTime) {
    Vector3 newPos = position + velocity * deltaTime;
    
    // Test for collisions
    ContactManifold manifold;
    if (detectCollision(newPos, manifold)) {
        // Project velocity onto contact plane
        Vector3 tangent = velocity - manifold.normal * velocity.dot(manifold.normal);
        velocity = tangent;
        
        // Apply friction
        velocity *= (1.0f - friction);
    }
    
    return velocity;
}
```

---

## Ragdoll System

### Articulated Bodies

**Production Analytics**:
- Reduced-coordinate constraints
- Joint limits and motors
- Constraint prioritization
- Mass distribution
- Damping and springs

### Constraints

**Production Features**:
- Ball-socket joints (spherical)
- Hinge joints (one-axis rotation)
- Universal joints (two-axis rotation)
- Prismatic joints (sliding)
- Motors for driven motion
- Spring-damper for compliance

---

## Vehicle Physics

### Raycast Vehicle

**Production Features**:
- Raycast wheels for suspension
- Suspension springs and dampers
- Tire friction model
- Engine torque
- Steering
- Braking

### Suspension Model

**Production Features**:
- Spring-damper system
- Anti-roll bars
- Limited travel
- Force distribution

---

## Raycasting and Triggers

### Raycasting

**Production Features**:
- Raycast against all shapes
- Closest hit
- Multiple hits (optional)
- Filter by collision layer
- Report normal and penetration

### Triggers

**Production Features**:
- Sensor volumes
- No physical response
- Event callbacks
- Layer-based filtering
- Enter/exit events

---

## Debug Visualization

### Debug Draw

**Production Features**:
- Wireframe shapes
- Contact points
- Contact normals
- AABB visualization
- Island visualization
- Sleep state visualization
- Performance metrics

---

## Numerical Robustness

### Tolerance Guidelines

**Absolute Tolerances**:
- `VELOCITY_EPSILON = 1e-6f` m/s
- `POSITION_EPSILON = 1e-4f` m
- `ANGLE_EPSILON = 1e-4f` rad
- `MASS_EPSILON = 1e-6f` kg

**Relative Tolerances**:
- `RELATIVE_EPSILON = 1e-4f` (0.01% of value)
- Used for comparisons with varying scales

### NaN/Inf Handling

**Policy**:
- Check for NaN/Inf after each operation
- Replace NaN with zero or default value
- Log warnings for Inf values
- Fallback to safe state on numerical error

### Precision Loss Prevention

**Techniques**:
- Use relative velocities when possible
- Accumulate impulses in higher precision
- Reorder operations to minimize cancellation
- Use compensated summation for critical paths

---

## Performance Optimization

### SIMD Vectorization

**Strategy**:
- Use SoA layout for hot paths
- Implement SIMD operations for:
  - Vector operations (add, sub, mul, dot, cross)
  - Matrix operations (mul, transpose)
  - AABB tests
- Use compiler intrinsics or portable SIMD libraries

### Cache Optimization

**Strategy**:
- Store related data contiguously
- Use struct-of-arrays for body arrays
- Prefetch data for predictable access patterns
- Minimize pointer chasing

### Job System Integration

**Strategy**:
- Parallel broadphase updates
- Parallel narrowphase tests
- Parallel constraint solving (within islands)
- Job dependencies for simulation stages

### Memory Pooling

**Strategy**:
- Pool allocations for:
  - Contact manifolds
  - Tree nodes
  - Constraint allocations
- Avoid fragmentation
- Reduce allocation overhead

---

## Testing Strategy

### Unit Tests

**Coverage**:
- Math correctness: Vector3, Matrix3x3, Quaternion operations
- Shape calculations: Volume, mass, inertia
- Collision detection: All shape pairs
- Solver: Constraint resolution
- Integration: Velocity, position integration

**Property-Based Tests**:
- Associativity: `(A + B) + C = A + (B + C)`
- Distributivity: `A × (B + C) = A × B + A × C`
- Identity: `A × I = A`, `I × A = A`
- Inverse: `A × A⁻¹ ≈ I`
- Determinant: `det(A × B) = det(A) × det(B)`

### Integration Tests

**Scenarios**:
- Stacking test (10, 100, 1000 boxes)
- Pendulum test (energy conservation)
- Rolling sphere test (friction)
- Collision response test (restitution)
- Character controller (slopes, steps)

### Regression Tests

**Policy**:
- Fix test cases for discovered bugs
- Run full test suite before commits
- Benchmark performance over time
- Detect performance regressions

### Stress Tests

**Scenarios**:
- 10,000 bodies in simulation
- Deep stacks (50+ bodies)
- Fast-moving objects (CCD test)
- Large scenes (broadphase scalability)
- Long-running stability (10+ minutes)

---

## Implementation Priority

### Phase 1: Core Foundation (Current)
- [x] Physics Math Module (Vector3, Matrix3x3, Quaternion, AABB)
- [x] Collision Shapes (Sphere, Box, Capsule, Plane)
- [x] Broadphase (Dynamic AABB Tree)
- [x] Narrowphase (Sphere-Sphere, Sphere-Box, Box-Box)
- [x] Sequential Impulse Solver
- [x] Physics World and Rigid Body System

### Phase 2: Production Validation
- [x] Fix all math bugs (cross-product matrix corrected)
- [x] Add numerical robustness (tryInverse, tolerances)
- [ ] Add orthonormal validation
- [ ] Add comprehensive tests
- [ ] Property-based testing
- [ ] Integration tests
- [ ] Stress tests

### Phase 3: Advanced Features (15% AAA)
- [ ] Sleeping and Island Management
- [ ] CCD (Speculative Contacts, Swept Sphere)
- [ ] Character Controller
- [ ] Ragdoll System
- [ ] Vehicle Physics
- [ ] Raycasting and Triggers
- [ ] Debug Visualization

### Phase 4: Optimization
- [ ] SIMD vectorization
- [ ] Cache optimization (SoA layout)
- [ ] Job system integration
- [ ] Memory pooling
- [ ] Performance profiling
- [ ] Benchmark suite

---

## References

### Research Sources
- PhysX Documentation
- Jolt Physics Source Code
- Bullet Physics Source Code
- Unity Physics Source Code
- Havok Physics Concepts
- Newton Physics Source Code
- Erin Catto's GDC Presentations
- Box2D Source Code
- Real-Time Collision Detection (Christer Ericson)
- Game Physics Engine Development (Ian Millington)

### Key Papers
- "Iterative Dynamics with Temporal Coherence" (Erin Catto)
- "Rigid Body Dynamics" (Kenny Erleben)
- "Continuous Collision Detection" (Young Kim)
- "Stable Contacts" (Erin Catto)
- "Modeling and Solving Constraints" (Erin Catto)

---

## Conclusion

This plan provides a comprehensive roadmap for building a production-quality physics engine targeting Indie A (100%), Double A (100%), and selected Triple A (15%) features. The emphasis is on numerical robustness, performance optimization, and extensive testing to ensure the physics system is trustworthy for production use.
