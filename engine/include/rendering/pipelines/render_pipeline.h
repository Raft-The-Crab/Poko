/**
 * @file render_pipeline.h
 * @brief Render pipeline header for PokoEngine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#pragma once

namespace Poko {
namespace Rendering {

/**
 * @brief Render pipeline for managing rendering passes
 */
class RenderPipeline {
public:
    RenderPipeline();
    ~RenderPipeline();

    /**
     * @brief Initialize the render pipeline
     * @return true if initialization succeeded
     */
    bool Initialize();

    /**
     * @brief Shutdown the render pipeline
     */
    void Shutdown();

    /**
     * @brief Execute the render pipeline
     */
    void Execute();

    /**
     * @brief Check if pipeline is initialized
     * @return true if initialized
     */
    bool IsInitialized() const;

private:
    bool m_initialized;
};

} // namespace Rendering
} // namespace Poko