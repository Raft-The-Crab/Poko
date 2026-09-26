/**
 * @file renderer.h
 * @brief Renderer core for Poko Engine
 * @details Rendering architecture with Diligent Engine integration
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace poko {
namespace renderer {

/**
 * @enum RenderQualityTier
 * @brief Quality tiers for rendering
 */
enum class RenderQualityTier {
    VeryLow,    ///< Minimum quality
    Low,        ///< Low quality
    Medium,     ///< Medium quality
    High,       ///< High quality
    VeryHigh,   ///< Maximum quality
    Custom      ///< Custom settings
};

/**
 * @struct RenderSettings
 * @brief Render quality settings
 */
struct RenderSettings {
    RenderQualityTier quality_tier;
    int shadow_resolution;
    int texture_quality;
    int texture_filtering;
    int ssaa_samples;
    bool enable_vsync;
    bool enable_shadows;
    bool enable_post_processing;
    float render_scale;
    
    RenderSettings()
        : quality_tier(RenderQualityTier::Medium)
        , shadow_resolution(1024)
        , texture_quality(1)
        , texture_filtering(1)
        , ssaa_samples(1)
        , enable_vsync(true)
        , enable_shadows(true)
        , enable_post_processing(true)
        , render_scale(1.0f) {
    }
};

/**
 * @class RenderDevice
 * @brief Graphics device wrapper (Diligent Engine integration)
 */
class RenderDevice {
public:
    /**
     * @brief Construct a render device
     */
    RenderDevice();

    /**
     * @brief Destructor
     */
    ~RenderDevice();

    /**
     * @brief Initialize the render device
     * @param window_handle Native window handle
     * @return true if successful
     */
    bool initialize(void* window_handle);

    /**
     * @brief Shutdown the render device
     */
    void shutdown();

    /**
     * @brief Check if device is initialized
     * @return true if initialized
     */
    bool is_initialized() const { return initialized_; }

    /**
     * @brief Get the device name
     * @return Device name string
     */
    std::string get_device_name() const;

private:
    bool initialized_;
    void* native_device_;
};

/**
 * @class Swapchain
 * @brief Swapchain for presentation
 */
class Swapchain {
public:
    /**
     * @brief Construct a swapchain
     * @param device Render device
     */
    explicit Swapchain(RenderDevice* device);

    /**
     * @brief Destructor
     */
    ~Swapchain();

    /**
     * @brief Create the swapchain
     * @param width Width in pixels
     * @param height Height in pixels
     * @param vsync Enable vsync
     * @return true if successful
     */
    bool create(int width, int height, bool vsync);

    /**
     * @brief Resize the swapchain
     * @param width New width in pixels
     * @param height New height in pixels
     */
    void resize(int width, int height);

    /**
     * @brief Present the frame
     */
    void present();

    /**
     * @brief Get width
     * @return Width in pixels
     */
    int width() const { return width_; }

    /**
     * @brief Get height
     * @return Height in pixels
     */
    int height() const { return height_; }

private:
    RenderDevice* device_;
    int width_;
    int height_;
    bool vsync_;
    void* native_swapchain_;
};

/**
 * @class Renderer
 * @brief Main renderer class
 */
class Renderer {
public:
    /**
     * @brief Construct a renderer
     */
    Renderer();

    /**
     * @brief Destructor
     */
    ~Renderer();

    /**
     * @brief Initialize the renderer
     * @param window_handle Native window handle
     * @param settings Render settings
     * @return true if successful
     */
    bool initialize(void* window_handle, const RenderSettings& settings);

    /**
     * @brief Shutdown the renderer
     */
    void shutdown();

    /**
     * @brief Begin a new frame
     */
    void begin_frame();

    /**
     * @brief End the current frame
     */
    void end_frame();

    /**
     * @brief Render a frame
     */
    void render();

    /**
     * @brief Get the render device
     * @return Pointer to render device
     */
    RenderDevice* get_device() { return device_.get(); }

    /**
     * @brief Get the swapchain
     * @return Pointer to swapchain
     */
    Swapchain* get_swapchain() { return swapchain_.get(); }

    /**
     * @brief Get current render settings
     * @return Current settings
     */
    const RenderSettings& get_settings() const { return settings_; }

    /**
     * @brief Update render settings
     * @param settings New settings
     */
    void set_settings(const RenderSettings& settings);

private:
    std::unique_ptr<RenderDevice> device_;
    std::unique_ptr<Swapchain> swapchain_;
    RenderSettings settings_;
    bool initialized_;
};

} // namespace renderer
} // namespace poko
