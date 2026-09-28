/**
 * @file camera.cpp
 * @brief Camera core implementation
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "camera/core/camera_internal.h"
#include "core/logging/logger.h"
#include <cstring>
#include <cmath>
#include <map>
#include <mutex>

namespace poko {
namespace camera {

} // namespace camera
} // namespace poko

// C API implementation
extern "C" {

using namespace poko::camera;

Camera* camera_create(CameraType type) {
    static CameraManagerImpl* manager = nullptr;
    if (!manager) {
        manager = new CameraManagerImpl();
        manager->cameras = nullptr;
        manager->active_camera = nullptr;
        manager->next_camera_id = 1;
    }
    
    std::lock_guard<std::mutex> lock(manager->mutex);
    
    CameraImpl* camera = new CameraImpl();
    camera->id = manager->next_camera_id++;
    camera->type = type;
    camera->position[0] = 0.0f;
    camera->position[1] = 0.0f;
    camera->position[2] = 5.0f;
    camera->target[0] = 0.0f;
    camera->target[1] = 0.0f;
    camera->target[2] = 0.0f;
    camera->up[0] = 0.0f;
    camera->up[1] = 1.0f;
    camera->up[2] = 0.0f;
    camera->field_of_view = 60.0f;
    camera->aspect_ratio = 16.0f / 9.0f;
    camera->near_plane = 0.1f;
    camera->far_plane = 1000.0f;
    camera->ortho_width = 10.0f;
    camera->ortho_height = 10.0f;
    camera->viewport_x = 0.0f;
    camera->viewport_y = 0.0f;
    camera->viewport_width = 1.0f;
    camera->viewport_height = 1.0f;
    camera->is_active = false;
    
    // Add to linked list
    camera->next = manager->cameras;
    manager->cameras = camera;
    
    // Add to map
    manager->camera_map[camera->id] = camera;
    
    // Set as active if it's the first camera
    if (!manager->active_camera) {
        manager->active_camera = camera;
        camera->is_active = true;
    }
    
    POKO_LOG_INFO("Camera: Camera created with ID " + std::to_string(camera->id));
    return from_camera_impl(camera);
}

void camera_destroy(Camera* camera) {
    if (!camera) return;
    
    static CameraManagerImpl* manager = nullptr;
    if (!manager) return;
    
    std::lock_guard<std::mutex> lock(manager->mutex);
    
    CameraImpl* impl = to_camera_impl(camera);
    
    // Remove from linked list
    CameraImpl* prev = nullptr;
    CameraImpl* current = manager->cameras;
    while (current) {
        if (current == impl) {
            if (prev) {
                prev->next = current->next;
            } else {
                manager->cameras = current->next;
            }
            break;
        }
        prev = current;
        current = current->next;
    }
    
    // Remove from map
    manager->camera_map.erase(impl->id);
    
    // Update active camera if this was active
    if (manager->active_camera == impl) {
        manager->active_camera = manager->cameras;
        if (manager->active_camera) {
            manager->active_camera->is_active = true;
        }
    }
    
    delete impl;
    
    POKO_LOG_DEBUG("Camera: Camera destroyed");
}

void camera_set_active(Camera* camera) {
    if (!camera) return;
    
    static CameraManagerImpl* manager = nullptr;
    if (!manager) return;
    
    std::lock_guard<std::mutex> lock(manager->mutex);
    
    CameraImpl* impl = to_camera_impl(camera);
    
    // Deactivate previous active camera
    if (manager->active_camera) {
        manager->active_camera->is_active = false;
    }
    
    // Set new active camera
    manager->active_camera = impl;
    impl->is_active = true;
    
    POKO_LOG_DEBUG("Camera: Camera " + std::to_string(impl->id) + " set as active");
}

Camera* camera_get_active(void) {
    static CameraManagerImpl* manager = nullptr;
    if (!manager) return nullptr;
    
    std::lock_guard<std::mutex> lock(manager->mutex);
    return from_camera_impl(manager->active_camera);
}

CameraType camera_get_type(const Camera* camera) {
    if (!camera) return CAMERA_TYPE_PERSPECTIVE;
    
    const CameraImpl* impl = to_camera_impl(camera);
    return impl->type;
}

bool camera_is_active(const Camera* camera) {
    if (!camera) return false;
    
    const CameraImpl* impl = to_camera_impl(camera);
    return impl->is_active;
}

} // extern "C"