/**
 * @file render_pipeline.h
 * @brief Render pipeline header for PokoEngine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#pragma once

#include <memory>
#include <vector>
#include <functional>

namespace Poko {
namespace Rendering {

// Forward declarations
class BgfxRenderer;

/**
 * @brief Render pass interface
 */
class RenderPass {
public:
    virtual ~RenderPass() = default;
    virtual void Execute() = 0;
    virtual const char* GetName() const = 0;
};

/**
 * @brief Render pipeline for managing rendering passes
 */
class RenderPipeline {
public:
    RenderPipeline();
    ~RenderPipeline();

    /**
     * @brief Initialize the render pipeline
     * @param renderer Pointer to bgfx renderer
     * @return true if initialization succeeded
     */
    bool Initialize(BgfxRenderer* renderer);

    /**
     * @brief Shutdown the render pipeline
     */
    void Shutdown();

    /**
     * @brief Execute the render pipeline
     */
    void Execute();

    /**
     * @brief Add a render pass to the pipeline
     * @param pass Render pass to add
     */
    void AddPass(std::unique_ptr<RenderPass> pass);

    /**
     * @brief Remove a render pass by name
     * @param name Name of the pass to remove
     */
    void RemovePass(const char* name);

    /**
     * @brief Check if pipeline is initialized
     * @return true if initialized
     */
    bool IsInitialized() const;

    /**
     * @brief Get number of render passes
     * @return Number of passes
     */
    size_t GetPassCount() const;

private:
    bool m_initialized;
    BgfxRenderer* m_renderer;
    std::vector<std::unique_ptr<RenderPass>> m_passes;
};

} // namespace Rendering
} // namespace Poko