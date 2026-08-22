/**
 * @file bgfx_renderer.h
 * @brief bgfx renderer header for PokoEngine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#pragma once

#include <cstdint>

namespace Poko {
namespace Rendering {

/**
 * @brief bgfx renderer class for 3D rendering
 */
class BgfxRenderer {
public:
    BgfxRenderer();
    ~BgfxRenderer();

    /**
     * @brief Initialize the renderer
     * @param windowHandle Native window handle
     * @param width Window width
     * @param height Window height
     * @return true if initialization succeeded
     */
    bool Initialize(void* windowHandle, uint32_t width, uint32_t height);

    /**
     * @brief Shutdown the renderer
     */
    void Shutdown();

    /**
     * @brief Begin a new frame
     */
    void BeginFrame();

    /**
     * @brief End the current frame and present
     */
    void EndFrame();

    /**
     * @brief Clear the screen
     * @param r Red component (0-1)
     * @param g Green component (0-1)
     * @param b Blue component (0-1)
     * @param a Alpha component (0-1)
     */
    void Clear(float r, float g, float b, float a);

    /**
     * @brief Check if renderer is initialized
     * @return true if initialized
     */
    bool IsInitialized() const;

private:
    bool m_initialized;
    uint32_t m_width;
    uint32_t m_height;
};

} // namespace Rendering
} // namespace Poko