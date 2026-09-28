/**
 * @file camera_properties.cpp
 * @brief Camera property setters/getters
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "camera/core/camera_internal.h"
#include "core/logging/logger.h"
#include <cmath>

namespace poko {
namespace camera {

} // namespace camera
} // namespace poko

// C API implementation
extern "C" {

using namespace poko::camera;

void camera_set_position(Camera* camera, float x, float y, float z) {
    if (!camera) return;
    
    CameraImpl* impl = to_camera_impl(camera);
    impl->position[0] = x;
    impl->position[1] = y;
    impl->position[2] = z;
}

void camera_get_position(const Camera* camera, float* x, float* y, float* z) {
    if (!camera) return;
    
    const CameraImpl* impl = to_camera_impl(camera);
    if (x) *x = impl->position[0];
    if (y) *y = impl->position[1];
    if (z) *z = impl->position[2];
}

void camera_set_target(Camera* camera, float x, float y, float z) {
    if (!camera) return;
    
    CameraImpl* impl = to_camera_impl(camera);
    impl->target[0] = x;
    impl->target[1] = y;
    impl->target[2] = z;
}

void camera_get_target(const Camera* camera, float* x, float* y, float* z) {
    if (!camera) return;
    
    const CameraImpl* impl = to_camera_impl(camera);
    if (x) *x = impl->target[0];
    if (y) *y = impl->target[1];
    if (z) *z = impl->target[2];
}

void camera_set_up(Camera* camera, float x, float y, float z) {
    if (!camera) return;
    
    CameraImpl* impl = to_camera_impl(camera);
    impl->up[0] = x;
    impl->up[1] = y;
    impl->up[2] = z;
}

void camera_get_up(const Camera* camera, float* x, float* y, float* z) {
    if (!camera) return;
    
    const CameraImpl* impl = to_camera_impl(camera);
    if (x) *x = impl->up[0];
    if (y) *y = impl->up[1];
    if (z) *z = impl->up[2];
}

void camera_set_fov(Camera* camera, float fov) {
    if (!camera) return;
    
    CameraImpl* impl = to_camera_impl(camera);
    impl->field_of_view = std::max(1.0f, std::min(179.0f, fov));
}

float camera_get_fov(const Camera* camera) {
    if (!camera) return 60.0f;
    
    const CameraImpl* impl = to_camera_impl(camera);
    return impl->field_of_view;
}

void camera_set_aspect_ratio(Camera* camera, float aspect) {
    if (!camera) return;
    
    CameraImpl* impl = to_camera_impl(camera);
    impl->aspect_ratio = std::max(0.1f, aspect);
}

float camera_get_aspect_ratio(const Camera* camera) {
    if (!camera) return 16.0f / 9.0f;
    
    const CameraImpl* impl = to_camera_impl(camera);
    return impl->aspect_ratio;
}

void camera_set_near_plane(Camera* camera, float near_plane) {
    if (!camera) return;
    
    CameraImpl* impl = to_camera_impl(camera);
    impl->near_plane = std::max(0.01f, near_plane);
}

float camera_get_near_plane(const Camera* camera) {
    if (!camera) return 0.1f;
    
    const CameraImpl* impl = to_camera_impl(camera);
    return impl->near_plane;
}

void camera_set_far_plane(Camera* camera, float far_plane) {
    if (!camera) return;
    
    CameraImpl* impl = to_camera_impl(camera);
    impl->far_plane = std::max(impl->near_plane + 0.1f, far_plane);
}

float camera_get_far_plane(const Camera* camera) {
    if (!camera) return 1000.0f;
    
    const CameraImpl* impl = to_camera_impl(camera);
    return impl->far_plane;
}

void camera_set_ortho_size(Camera* camera, float width, float height) {
    if (!camera) return;
    
    CameraImpl* impl = to_camera_impl(camera);
    impl->ortho_width = std::max(0.1f, width);
    impl->ortho_height = std::max(0.1f, height);
}

void camera_get_ortho_size(const Camera* camera, float* width, float* height) {
    if (!camera) return;
    
    const CameraImpl* impl = to_camera_impl(camera);
    if (width) *width = impl->ortho_width;
    if (height) *height = impl->ortho_height;
}

void camera_set_viewport(Camera* camera, float x, float y, float width, float height) {
    if (!camera) return;
    
    CameraImpl* impl = to_camera_impl(camera);
    impl->viewport_x = std::max(0.0f, x);
    impl->viewport_y = std::max(0.0f, y);
    impl->viewport_width = std::max(0.0f, width);
    impl->viewport_height = std::max(0.0f, height);
}

void camera_get_viewport(const Camera* camera, float* x, float* y, float* width, float* height) {
    if (!camera) return;
    
    const CameraImpl* impl = to_camera_impl(camera);
    if (x) *x = impl->viewport_x;
    if (y) *y = impl->viewport_y;
    if (width) *width = impl->viewport_width;
    if (height) *height = impl->viewport_height;
}

} // extern "C"