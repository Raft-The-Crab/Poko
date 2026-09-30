/**
 * @file shape_type.h
 * @brief Shape type enumeration
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_SHAPES_SHAPE_TYPE_H
#define POKO_CORE_COMPONENTS_PHYSICS_SHAPES_SHAPE_TYPE_H

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace shapes {

/**
 * @brief Shape type enumeration
 */
enum class ShapeType : uint32_t {
    Sphere,
    Box,
    Capsule,
    Plane,
    ConvexHull,
    Triangle,
    TriangleMesh,
    HeightField,
    Compound,
    Cylinder,
    Cone,
    RoundedBox,
    CustomConvex,
    Unknown
};

} // namespace shapes
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_SHAPES_SHAPE_TYPE_H
