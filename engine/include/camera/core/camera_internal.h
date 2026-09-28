/**
 * @file camera_internal.h
 * @brief Internal camera implementation structures
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#ifndef POKO_CAMERA_INTERNAL_H
#define POKO_CAMERA_INTERNAL_H

#include "camera/camera.h"
#include <cstdint>
#include <map>
#include <mutex>

namespace poko {
namespace camera {

// Camera implementation
struct CameraImpl {
    uint64_t id;
    CameraType type;
    float position[3];
    float target[3];
    float up[3];
    float field_of_view;
    float aspect_ratio;
    float near_plane;
    float far_plane;
    float ortho_width;
    float ortho_height;
    float viewport_x;
    float viewport_y;
    float viewport_width;
    float viewport_height;
    bool is_active;
    CameraImpl* next;
};

// Camera manager
struct CameraManagerImpl {
    CameraImpl* cameras;
    CameraImpl* active_camera;
    std::map<uint64_t, CameraImpl*> camera_map;
    std::mutex mutex;
    uint64_t next_camera_id;
};

// Convert to internal implementation (inline to avoid multiple definition warnings)
inline CameraImpl* to_camera_impl(Camera* camera) {
    return reinterpret_cast<CameraImpl*>(camera);
}

inline const CameraImpl* to_camera_impl(const Camera* camera) {
    return reinterpret_cast<const CameraImpl*>(camera);
}

inline Camera* from_camera_impl(CameraImpl* impl) {
    return reinterpret_cast<Camera*>(impl);
}

} // namespace camera
} // namespace poko

#endif // POKO_CAMERA_INTERNAL_H