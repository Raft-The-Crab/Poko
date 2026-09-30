/**
 * @file transform.h
 * @brief Transform component header
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_TRANSFORM_TRANSFORM_H
#define POKO_CORE_COMPONENTS_TRANSFORM_TRANSFORM_H

#include <cstdint>
#include <array>
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace transform {

// ============================================================================
// Vector3
// ============================================================================

/**
 * @brief 3D vector for position and scale
 */
struct Vector3 {
    float x;
    float y;
    float z;
    
    /**
     * @brief Default constructor - zero vector
     */
    constexpr Vector3() noexcept : x(0.0f), y(0.0f), z(0.0f) {}
    
    /**
     * @brief Construct from components
     */
    constexpr Vector3(float x_, float y_, float z_) noexcept : x(x_), y(y_), z(z_) {}
    
    /**
     * @brief Zero vector constant
     */
    static constexpr Vector3 zero() noexcept { return Vector3(0.0f, 0.0f, 0.0f); }
    
    /**
     * @brief One vector constant
     */
    static constexpr Vector3 one() noexcept { return Vector3(1.0f, 1.0f, 1.0f); }
    
    /**
     * @brief Up vector constant
     */
    static constexpr Vector3 up() noexcept { return Vector3(0.0f, 1.0f, 0.0f); }
    
    /**
     * @brief Down vector constant
     */
    static constexpr Vector3 down() noexcept { return Vector3(0.0f, -1.0f, 0.0f); }
    
    /**
     * @brief Forward vector constant
     */
    static constexpr Vector3 forward() noexcept { return Vector3(0.0f, 0.0f, 1.0f); }
    
    /**
     * @brief Back vector constant
     */
    static constexpr Vector3 back() noexcept { return Vector3(0.0f, 0.0f, -1.0f); }
    
    /**
     * @brief Right vector constant
     */
    static constexpr Vector3 right() noexcept { return Vector3(1.0f, 0.0f, 0.0f); }
    
    /**
     * @brief Left vector constant
     */
    static constexpr Vector3 left() noexcept { return Vector3(-1.0f, 0.0f, 0.0f); }
    
    /**
     * @brief Add two vectors
     */
    [[nodiscard]] constexpr Vector3 operator+(const Vector3& other) const noexcept {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    
    /**
     * @brief Subtract two vectors
     */
    [[nodiscard]] constexpr Vector3 operator-(const Vector3& other) const noexcept {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
    
    /**
     * @brief Multiply vector by scalar
     */
    [[nodiscard]] constexpr Vector3 operator*(float scalar) const noexcept {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    
    /**
     * @brief Divide vector by scalar
     */
    [[nodiscard]] constexpr Vector3 operator/(float scalar) const noexcept {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }
    
    /**
     * @brief Dot product
     */
    [[nodiscard]] constexpr float dot(const Vector3& other) const noexcept {
        return x * other.x + y * other.y + z * other.z;
    }
    
    /**
     * @brief Cross product
     */
    [[nodiscard]] Vector3 cross(const Vector3& other) const noexcept {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    
    /**
     * @brief Vector length squared
     */
    [[nodiscard]] constexpr float lengthSquared() const noexcept {
        return x * x + y * y + z * z;
    }
    
    /**
     * @brief Vector length
     */
    [[nodiscard]] float length() const noexcept {
        return std::sqrt(lengthSquared());
    }
    
    /**
     * @brief Normalize vector
     */
    [[nodiscard]] Vector3 normalized() const noexcept {
        float len = length();
        if (len > 0.0001f) {
            return *this / len;
        }
        return Vector3::zero();
    }
    
    /**
     * @brief Distance to another vector
     */
    [[nodiscard]] float distanceTo(const Vector3& other) const noexcept {
        return (*this - other).length();
    }
};

// ============================================================================
// Quaternion
// ============================================================================

/**
 * @brief Quaternion for rotation
 */
struct Quaternion {
    float w;
    float x;
    float y;
    float z;
    
    /**
     * @brief Default constructor - identity quaternion
     */
    constexpr Quaternion() noexcept : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
    
    /**
     * @brief Construct from components
     */
    constexpr Quaternion(float w_, float x_, float y_, float z_) noexcept 
        : w(w_), x(x_), y(y_), z(z_) {}
    
    /**
     * @brief Identity quaternion constant
     */
    static constexpr Quaternion identity() noexcept { return Quaternion(1.0f, 0.0f, 0.0f, 0.0f); }
    
    /**
     * @brief Multiply quaternions
     */
    [[nodiscard]] Quaternion operator*(const Quaternion& other) const noexcept {
        return Quaternion(
            w * other.w - x * other.x - y * other.y - z * other.z,
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w
        );
    }
    
    /**
     * @brief Quaternion length squared
     */
    [[nodiscard]] constexpr float lengthSquared() const noexcept {
        return w * w + x * x + y * y + z * z;
    }
    
    /**
     * @brief Quaternion length
     */
    [[nodiscard]] float length() const noexcept {
        return std::sqrt(lengthSquared());
    }
    
    /**
     * @brief Normalize quaternion
     */
    [[nodiscard]] Quaternion normalized() const noexcept {
        float len = length();
        if (len > 0.0001f) {
            return Quaternion(w / len, x / len, y / len, z / len);
        }
        return Quaternion::identity();
    }
    
    /**
     * @brief Conjugate quaternion
     */
    [[nodiscard]] Quaternion conjugate() const noexcept {
        return Quaternion(w, -x, -y, -z);
    }
    
    /**
     * @brief Rotate vector by quaternion
     */
    [[nodiscard]] Vector3 rotateVector(const Vector3& v) const noexcept {
        // q * v * q_conjugate
        Quaternion vQuat(0.0f, v.x, v.y, v.z);
        Quaternion result = (*this * vQuat) * conjugate();
        return Vector3(result.x, result.y, result.z);
    }
};

// ============================================================================
// Matrix4x4
// ============================================================================

/**
 * @brief 4x4 matrix for transform calculations
 */
struct Matrix4x4 {
    std::array<float, 16> data;
    
    /**
     * @brief Default constructor - identity matrix
     */
    constexpr Matrix4x4() noexcept : data{1.0f, 0.0f, 0.0f, 0.0f,
                                           0.0f, 1.0f, 0.0f, 0.0f,
                                           0.0f, 0.0f, 1.0f, 0.0f,
                                           0.0f, 0.0f, 0.0f, 1.0f} {}
    
    /**
     * @brief Identity matrix constant
     */
    static constexpr Matrix4x4 identity() noexcept { return Matrix4x4(); }
};

// ============================================================================
// Transform Component
// ============================================================================

/**
 * @brief Transform component for position, rotation, and scale
 * 
 * This component handles:
 * - Local position, rotation, and scale
 * - World position, rotation, and scale (computed from parent)
 * - Parent-child transform hierarchy
 * - Transform matrices for rendering and physics
 * 
 * @section thread_safety Thread Safety
 * Transform component is not thread-safe by default.
 * Access must be synchronized externally if used from multiple threads.
 */
class Transform {
public:
    /**
     * @brief Constructor
     */
    Transform() noexcept;
    
    /**
     * @brief Destructor
     */
    ~Transform() = default;
    
    // ============================================================================
    // Local Transform Accessors
    // ============================================================================
    
    /**
     * @brief Get local position
     * @return Local position
     */
    [[nodiscard]] const Vector3& getLocalPosition() const noexcept { return m_localPosition; }
    
    /**
     * @brief Set local position
     * @param position New local position
     */
    void setLocalPosition(const Vector3& position) noexcept;
    
    /**
     * @brief Get local rotation
     * @return Local rotation
     */
    [[nodiscard]] const Quaternion& getLocalRotation() const noexcept { return m_localRotation; }
    
    /**
     * @brief Set local rotation
     * @param rotation New local rotation
     */
    void setLocalRotation(const Quaternion& rotation) noexcept;
    
    /**
     * @brief Get local scale
     * @return Local scale
     */
    [[nodiscard]] const Vector3& getLocalScale() const noexcept { return m_localScale; }
    
    /**
     * @brief Set local scale
     * @param scale New local scale
     */
    void setLocalScale(const Vector3& scale) noexcept;
    
    // ============================================================================
    // World Transform Accessors
    // ============================================================================
    
    /**
     * @brief Get world position
     * @return World position
     */
    [[nodiscard]] const Vector3& getWorldPosition() const noexcept { return m_worldPosition; }
    
    /**
     * @brief Get world rotation
     * @return World rotation
     */
    [[nodiscard]] const Quaternion& getWorldRotation() const noexcept { return m_worldRotation; }
    
    /**
     * @brief Get world scale
     * @return World scale
     */
    [[nodiscard]] const Vector3& getWorldScale() const noexcept { return m_worldScale; }
    
    // ============================================================================
    // Transform Matrix
    // ============================================================================
    
    /**
     * @brief Get local-to-world matrix
     * @return Transform matrix
     */
    [[nodiscard]] const Matrix4x4& getLocalToWorldMatrix() const noexcept { return m_localToWorldMatrix; }
    
    /**
     * @brief Get world-to-local matrix
     * @return Inverse transform matrix
     */
    [[nodiscard]] const Matrix4x4& getWorldToLocalMatrix() const noexcept { return m_worldToLocalMatrix; }
    
    // ============================================================================
    // Parent-Child Hierarchy
    // ============================================================================
    
    /**
     * @brief Set parent transform
     * @param parent Parent transform (nullptr for no parent)
     */
    void setParent(Transform* parent) noexcept;
    
    /**
     * @brief Get parent transform
     * @return Parent transform (nullptr if no parent)
     */
    [[nodiscard]] Transform* getParent() const noexcept { return m_parent; }
    
    /**
     * @brief Check if has parent
     * @return True if has parent
     */
    [[nodiscard]] bool hasParent() const noexcept { return m_parent != nullptr; }
    
    // ============================================================================
    // Transform Updates
    // ============================================================================
    
    /**
     * @brief Update world transform from local transform and parent
     * @note Must be called after modifying local transform or parent
     */
    void updateWorldTransform() noexcept;
    
    /**
     * @brief Mark transform as dirty (needs update)
     */
    void markDirty() noexcept;
    
    /**
     * @brief Check if transform is dirty
     * @return True if transform needs update
     */
    [[nodiscard]] bool isDirty() const noexcept { return m_dirty; }
    
private:
    // Local transform
    Vector3 m_localPosition;
    Quaternion m_localRotation;
    Vector3 m_localScale;
    
    // World transform (computed)
    Vector3 m_worldPosition;
    Quaternion m_worldRotation;
    Vector3 m_worldScale;
    
    // Transform matrices
    Matrix4x4 m_localToWorldMatrix;
    Matrix4x4 m_worldToLocalMatrix;
    
    // Hierarchy
    Transform* m_parent;
    
    // Dirty flag
    bool m_dirty;
    
    /**
     * @brief Compute local-to-world matrix
     */
    void computeLocalToWorldMatrix() noexcept;
    
    /**
     * @brief Compute world-to-local matrix
     */
    void computeWorldToLocalMatrix() noexcept;
};

// ============================================================================
// Constants
// ============================================================================

/// Maximum transform hierarchy depth (for safety)
constexpr uint32_t MAX_TRANSFORM_DEPTH = 256;

/// Maximum scale value (for safety)
constexpr float MAX_SCALE = 1000000.0f;

/// Minimum scale value (for safety)
constexpr float MIN_SCALE = 0.0001f;

/// Maximum position value (for safety)
constexpr float MAX_POSITION = 1000000.0f;

} // namespace transform
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_TRANSFORM_TRANSFORM_H
