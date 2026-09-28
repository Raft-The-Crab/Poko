/**
 * @file camera.h
 * @brief Camera system interface for Poko Engine
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#ifndef POKO_CAMERA_H
#define POKO_CAMERA_H

#ifdef __cplusplus
extern "C" {
#endif

// Camera handle (opaque)
typedef struct Camera Camera;

// Camera types
typedef enum {
    CAMERA_TYPE_PERSPECTIVE = 0,
    CAMERA_TYPE_ORTHOGRAPHIC = 1
} CameraType;

/**
 * Create a camera
 * @param type Camera type (perspective or orthographic)
 * @return Camera handle, or NULL on failure
 */
Camera* camera_create(CameraType type);

/**
 * Destroy a camera
 * @param camera Camera handle
 */
void camera_destroy(Camera* camera);

/**
 * Set camera position
 * @param camera Camera handle
 * @param x X position
 * @param y Y position
 * @param z Z position
 */
void camera_set_position(Camera* camera, float x, float y, float z);

/**
 * Get camera position
 * @param camera Camera handle
 * @param x Output X position
 * @param y Output Y position
 * @param z Output Z position
 */
void camera_get_position(const Camera* camera, float* x, float* y, float* z);

/**
 * Set camera target (look-at point)
 * @param camera Camera handle
 * @param x Target X position
 * @param y Target Y position
 * @param z Target Z position
 */
void camera_set_target(Camera* camera, float x, float y, float z);

/**
 * Get camera target
 * @param camera Camera handle
 * @param x Output target X position
 * @param y Output target Y position
 * @param z Output target Z position
 */
void camera_get_target(const Camera* camera, float* x, float* y, float* z);

/**
 * Set camera up vector
 * @param camera Camera handle
 * @param x Up X component
 * @param y Up Y component
 * @param z Up Z component
 */
void camera_set_up(Camera* camera, float x, float y, float z);

/**
 * Get camera up vector
 * @param camera Camera handle
 * @param x Output up X component
 * @param y Output up Y component
 * @param z Output up Z component
 */
void camera_get_up(const Camera* camera, float* x, float* y, float* z);

/**
 * Set camera field of view (perspective only)
 * @param camera Camera handle
 * @param fov Field of view in degrees
 */
void camera_set_fov(Camera* camera, float fov);

/**
 * Get camera field of view
 * @param camera Camera handle
 * @return Field of view in degrees
 */
float camera_get_fov(const Camera* camera);

/**
 * Set camera aspect ratio
 * @param camera Camera handle
 * @param aspect Aspect ratio (width / height)
 */
void camera_set_aspect_ratio(Camera* camera, float aspect);

/**
 * Get camera aspect ratio
 * @param camera Camera handle
 * @return Aspect ratio
 */
float camera_get_aspect_ratio(const Camera* camera);

/**
 * Set camera near plane distance
 * @param camera Camera handle
 * @param near_plane Near plane distance
 */
void camera_set_near_plane(Camera* camera, float near_plane);

/**
 * Get camera near plane distance
 * @param camera Camera handle
 * @return Near plane distance
 */
float camera_get_near_plane(const Camera* camera);

/**
 * Set camera far plane distance
 * @param camera Camera handle
 * @param far_plane Far plane distance
 */
void camera_set_far_plane(Camera* camera, float far_plane);

/**
 * Get camera far plane distance
 * @param camera Camera handle
 * @return Far plane distance
 */
float camera_get_far_plane(const Camera* camera);

/**
 * Set orthographic camera size
 * @param camera Camera handle
 * @param width Width
 * @param height Height
 */
void camera_set_ortho_size(Camera* camera, float width, float height);

/**
 * Get orthographic camera size
 * @param camera Camera handle
 * @param width Output width
 * @param height Output height
 */
void camera_get_ortho_size(const Camera* camera, float* width, float* height);

/**
 * Set camera viewport
 * @param camera Camera handle
 * @param x Viewport X (0.0 to 1.0)
 * @param y Viewport Y (0.0 to 1.0)
 * @param width Viewport width (0.0 to 1.0)
 * @param height Viewport height (0.0 to 1.0)
 */
void camera_set_viewport(Camera* camera, float x, float y, float width, float height);

/**
 * Get camera viewport
 * @param camera Camera handle
 * @param x Output viewport X
 * @param y Output viewport Y
 * @param width Output viewport width
 * @param height Output viewport height
 */
void camera_get_viewport(const Camera* camera, float* x, float* y, float* width, float* height);

/**
 * Set camera as active
 * @param camera Camera handle
 */
void camera_set_active(Camera* camera);

/**
 * Get the active camera
 * @return Active camera handle, or NULL if none
 */
Camera* camera_get_active(void);

/**
 * Make camera look at a point
 * @param camera Camera handle
 * @param target_x Target X position
 * @param target_y Target Y position
 * @param target_z Target Z position
 */
void camera_look_at(Camera* camera, float target_x, float target_y, float target_z);

/**
 * Move camera forward/backward
 * @param camera Camera handle
 * @param distance Distance to move (positive = forward, negative = backward)
 */
void camera_move_forward(Camera* camera, float distance);

/**
 * Move camera left/right
 * @param camera Camera handle
 * @param distance Distance to move (positive = right, negative = left)
 */
void camera_move_right(Camera* camera, float distance);

/**
 * Move camera up/down
 * @param camera Camera handle
 * @param distance Distance to move (positive = up, negative = down)
 */
void camera_move_up(Camera* camera, float distance);

/**
 * Rotate camera
 * @param camera Camera handle
 * @param yaw Yaw rotation in radians
 * @param pitch Pitch rotation in radians
 */
void camera_rotate(Camera* camera, float yaw, float pitch);

/**
 * Orbit camera around target
 * @param camera Camera handle
 * @param yaw Yaw rotation in radians
 * @param pitch Pitch rotation in radians
 * @param distance Distance from target (0 = keep current distance)
 */
void camera_orbit(Camera* camera, float yaw, float pitch, float distance);

/**
 * Get camera view matrix (4x4 column-major)
 * @param camera Camera handle
 * @param matrix Output matrix (16 floats)
 */
void camera_get_view_matrix(const Camera* camera, float* matrix);

/**
 * Get camera projection matrix (4x4 column-major)
 * @param camera Camera handle
 * @param matrix Output matrix (16 floats)
 */
void camera_get_projection_matrix(const Camera* camera, float* matrix);

/**
 * Get camera type
 * @param camera Camera handle
 * @return Camera type
 */
CameraType camera_get_type(const Camera* camera);

/**
 * Check if camera is active
 * @param camera Camera handle
 * @return true if active, false otherwise
 */
bool camera_is_active(const Camera* camera);

#ifdef __cplusplus
}
#endif

#endif // POKO_CAMERA_H