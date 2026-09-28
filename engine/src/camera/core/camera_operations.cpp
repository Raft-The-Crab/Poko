/**
 * @file camera_operations.cpp
 * @brief Camera movement and rotation operations
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "camera/core/camera_internal.h"
#include "core/logging/logger.h"
#include <cmath>

namespace poko {
namespace camera {

// Matrix operations
static void normalize_vector(float* v) {
    float length = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (length > 0.0001f) {
        v[0] /= length;
        v[1] /= length;
        v[2] /= length;
    }
}

static void cross_product(const float* a, const float* b, float* result) {
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];
}

static float dot_product(const float* a, const float* b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

static void subtract_vectors(const float* a, const float* b, float* result) {
    result[0] = a[0] - b[0];
    result[1] = a[1] - b[1];
    result[2] = a[2] - b[2];
}

} // namespace camera
} // namespace poko

// C API implementation
extern "C" {

using namespace poko::camera;

void camera_look_at(Camera* camera, float target_x, float target_y, float target_z) {
    if (!camera) return;
    
    CameraImpl* impl = to_camera_impl(camera);
    impl->target[0] = target_x;
    impl->target[1] = target_y;
    impl->target[2] = target_z;
}

void camera_move_forward(Camera* camera, float distance) {
    if (!camera) return;
    
    CameraImpl* impl = to_camera_impl(camera);
    
    float forward[3];
    subtract_vectors(impl->target, impl->position, forward);
    normalize_vector(forward);
    
    impl->position[0] += forward[0] * distance;
    impl->position[1] += forward[1] * distance;
    impl->position[2] += forward[2] * distance;
    
    impl->target[0] += forward[0] * distance;
    impl->target[1] += forward[1] * distance;
    impl->target[2] += forward[2] * distance;
}

void camera_move_right(Camera* camera, float distance) {
    if (!camera) return;
    
    CameraImpl* impl = to_camera_impl(camera);
    
    float forward[3];
    subtract_vectors(impl->target, impl->position, forward);
    normalize_vector(forward);
    
    float right[3];
    cross_product(forward, impl->up, right);
    normalize_vector(right);
    
    impl->position[0] += right[0] * distance;
    impl->position[1] += right[1] * distance;
    impl->position[2] += right[2] * distance;
    
    impl->target[0] += right[0] * distance;
    impl->target[1] += right[1] * distance;
    impl->target[2] += right[2] * distance;
}

void camera_move_up(Camera* camera, float distance) {
    if (!camera) return;
    
    CameraImpl* impl = to_camera_impl(camera);
    
    impl->position[0] += impl->up[0] * distance;
    impl->position[1] += impl->up[1] * distance;
    impl->position[2] += impl->up[2] * distance;
    
    impl->target[0] += impl->up[0] * distance;
    impl->target[1] += impl->up[1] * distance;
    impl->target[2] += impl->up[2] * distance;
}

void camera_rotate(Camera* camera, float yaw, float pitch) {
    if (!camera) return;
    
    CameraImpl* impl = to_camera_impl(camera);
    
    // Calculate forward vector
    float forward[3];
    subtract_vectors(impl->target, impl->position, forward);
    normalize_vector(forward);
    
    // Calculate right vector
    float right[3];
    cross_product(forward, impl->up, right);
    normalize_vector(right);
    
    // Apply yaw rotation (around up axis)
    float cos_yaw = std::cos(yaw);
    float sin_yaw = std::sin(yaw);
    float rotated_forward[3];
    rotated_forward[0] = forward[0] * cos_yaw + right[0] * sin_yaw;
    rotated_forward[1] = forward[1] * cos_yaw + right[1] * sin_yaw;
    rotated_forward[2] = forward[2] * cos_yaw + right[2] * sin_yaw;
    
    // Apply pitch rotation (around right axis)
    float cos_pitch = std::cos(pitch);
    float sin_pitch = std::sin(pitch);
    float final_forward[3];
    final_forward[0] = rotated_forward[0] * cos_pitch - impl->up[0] * sin_pitch;
    final_forward[1] = rotated_forward[1] * cos_pitch - impl->up[1] * sin_pitch;
    final_forward[2] = rotated_forward[2] * cos_pitch - impl->up[2] * sin_pitch;
    
    // Update target
    impl->target[0] = impl->position[0] + final_forward[0];
    impl->target[1] = impl->position[1] + final_forward[1];
    impl->target[2] = impl->position[2] + final_forward[2];
}

void camera_orbit(Camera* camera, float yaw, float pitch, float distance) {
    if (!camera) return;
    
    CameraImpl* impl = to_camera_impl(camera);
    
    // Calculate current distance to target
    float current_dist_vec[3];
    subtract_vectors(impl->position, impl->target, current_dist_vec);
    float current_distance = std::sqrt(dot_product(current_dist_vec, current_dist_vec));
    
    // Apply distance override if provided
    if (distance > 0.0f) {
        current_distance = distance;
    }
    
    // Calculate spherical coordinates
    float x = impl->position[0] - impl->target[0];
    float y = impl->position[1] - impl->target[1];
    float z = impl->position[2] - impl->target[2];
    
    float radius = current_distance;
    float theta = std::atan2(z, x) + yaw;
    float phi = std::acos(y / radius) + pitch;
    
    // Clamp phi to avoid gimbal lock
    phi = std::max(0.01f, std::min(3.13f, phi));
    
    // Calculate new position
    impl->position[0] = impl->target[0] + radius * std::sin(phi) * std::cos(theta);
    impl->position[1] = impl->target[1] + radius * std::cos(phi);
    impl->position[2] = impl->target[2] + radius * std::sin(phi) * std::sin(theta);
}

} // extern "C"