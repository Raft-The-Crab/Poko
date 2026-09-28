/**
 * @file camera_matrices.cpp
 * @brief Camera matrix calculations (view and projection)
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

void camera_get_view_matrix(const Camera* camera, float* matrix) {
    if (!camera || !matrix) return;
    
    const CameraImpl* impl = to_camera_impl(camera);
    
    // Calculate forward, right, and up vectors
    float forward[3];
    subtract_vectors(impl->target, impl->position, forward);
    normalize_vector(forward);
    
    float right[3];
    cross_product(forward, impl->up, right);
    normalize_vector(right);
    
    float up[3];
    cross_product(right, forward, up);
    
    // Build view matrix (column-major)
    matrix[0] = right[0];
    matrix[1] = up[0];
    matrix[2] = -forward[0];
    matrix[3] = 0.0f;
    
    matrix[4] = right[1];
    matrix[5] = up[1];
    matrix[6] = -forward[1];
    matrix[7] = 0.0f;
    
    matrix[8] = right[2];
    matrix[9] = up[2];
    matrix[10] = -forward[2];
    matrix[11] = 0.0f;
    
    matrix[12] = -dot_product(right, impl->position);
    matrix[13] = -dot_product(up, impl->position);
    matrix[14] = dot_product(forward, impl->position);
    matrix[15] = 1.0f;
}

void camera_get_projection_matrix(const Camera* camera, float* matrix) {
    if (!camera || !matrix) return;
    
    const CameraImpl* impl = to_camera_impl(camera);
    
    if (impl->type == CAMERA_TYPE_PERSPECTIVE) {
        // Perspective projection
        float fov_rad = impl->field_of_view * 3.14159265359f / 180.0f;
        float tan_half_fov = std::tan(fov_rad / 2.0f);
        
        matrix[0] = 1.0f / (impl->aspect_ratio * tan_half_fov);
        matrix[1] = 0.0f;
        matrix[2] = 0.0f;
        matrix[3] = 0.0f;
        
        matrix[4] = 0.0f;
        matrix[5] = 1.0f / tan_half_fov;
        matrix[6] = 0.0f;
        matrix[7] = 0.0f;
        
        matrix[8] = 0.0f;
        matrix[9] = 0.0f;
        matrix[10] = -(impl->far_plane + impl->near_plane) / (impl->far_plane - impl->near_plane);
        matrix[11] = -1.0f;
        
        matrix[12] = 0.0f;
        matrix[13] = 0.0f;
        matrix[14] = -(2.0f * impl->far_plane * impl->near_plane) / (impl->far_plane - impl->near_plane);
        matrix[15] = 0.0f;
    } else {
        // Orthographic projection
        float left = -impl->ortho_width / 2.0f;
        float right = impl->ortho_width / 2.0f;
        float bottom = -impl->ortho_height / 2.0f;
        float top = impl->ortho_height / 2.0f;
        
        matrix[0] = 2.0f / (right - left);
        matrix[1] = 0.0f;
        matrix[2] = 0.0f;
        matrix[3] = 0.0f;
        
        matrix[4] = 0.0f;
        matrix[5] = 2.0f / (top - bottom);
        matrix[6] = 0.0f;
        matrix[7] = 0.0f;
        
        matrix[8] = 0.0f;
        matrix[9] = 0.0f;
        matrix[10] = -2.0f / (impl->far_plane - impl->near_plane);
        matrix[11] = 0.0f;
        
        matrix[12] = -(right + left) / (right - left);
        matrix[13] = -(top + bottom) / (top - bottom);
        matrix[14] = -(impl->far_plane + impl->near_plane) / (impl->far_plane - impl->near_plane);
        matrix[15] = 1.0f;
    }
}

} // extern "C"