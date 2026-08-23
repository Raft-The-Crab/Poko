/**
 * @file render_pipeline.cpp
 * @brief Render pipeline implementation for PokoEngine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "rendering/pipelines/render_pipeline.h"
#include "rendering/bgfx/bgfx_renderer.h"
#include "core/logging/logger.h"
#include <cstring>
#include <algorithm>

namespace Poko {
namespace Rendering {

// Simple geometry pass for demonstration
class GeometryPass : public RenderPass {
public:
    GeometryPass(BgfxRenderer* renderer) : m_renderer(renderer) {}
    
    void Execute() override {
        // Execute geometry rendering
        LOG_DEBUG("Executing Geometry Pass");
    }
    
    const char* GetName() const override {
        return "GeometryPass";
    }

private:
    BgfxRenderer* m_renderer;
};

// Clear pass for demonstration
class ClearPass : public RenderPass {
public:
    ClearPass(BgfxRenderer* renderer) : m_renderer(renderer) {}
    
    void Execute() override {
        // Clear the screen
        m_renderer->Clear(0.0f, 0.0f, 0.2f, 1.0f); // Dark blue background
        LOG_DEBUG("Executing Clear Pass");
    }
    
    const char* GetName() const override {
        return "ClearPass";
    }

private:
    BgfxRenderer* m_renderer;
};

RenderPipeline::RenderPipeline()
    : m_initialized(false)
    , m_renderer(nullptr)
{
}

RenderPipeline::~RenderPipeline()
{
    if (m_initialized) {
        Shutdown();
    }
}

bool RenderPipeline::Initialize(BgfxRenderer* renderer)
{
    if (m_initialized) {
        LOG_WARNING("RenderPipeline already initialized");
        return false;
    }

    LOG_INFO("Initializing RenderPipeline...");

    if (!renderer) {
        LOG_ERROR("RenderPipeline: Invalid renderer pointer");
        return false;
    }

    m_renderer = renderer;

    // Add default passes
    AddPass(std::make_unique<ClearPass>(renderer));
    AddPass(std::make_unique<GeometryPass>(renderer));

    LOG_INFO("RenderPipeline initialized successfully with " + std::to_string(m_passes.size()) + " passes");
    m_initialized = true;
    return true;
}

void RenderPipeline::Shutdown()
{
    if (!m_initialized) {
        return;
    }

    LOG_INFO("Shutting down RenderPipeline...");

    m_passes.clear();
    m_renderer = nullptr;

    m_initialized = false;
    LOG_INFO("RenderPipeline shutdown complete");
}

void RenderPipeline::Execute()
{
    if (!m_initialized) {
        LOG_ERROR("RenderPipeline: Cannot execute - not initialized");
        return;
    }

    LOG_DEBUG("Executing RenderPipeline with " + std::to_string(m_passes.size()) + " passes");

    for (const auto& pass : m_passes) {
        if (pass) {
            try {
                pass->Execute();
            } catch (const std::exception& e) {
                LOG_ERROR("RenderPipeline: Error executing pass " + std::string(pass->GetName()) + ": " + e.what());
            }
        }
    }
}

void RenderPipeline::AddPass(std::unique_ptr<RenderPass> pass)
{
    if (pass) {
        m_passes.push_back(std::move(pass));
        LOG_DEBUG("Added render pass: " + std::string(pass->GetName()));
    }
}

void RenderPipeline::RemovePass(const char* name)
{
    auto it = std::remove_if(m_passes.begin(), m_passes.end(),
        [name](const std::unique_ptr<RenderPass>& pass) {
            return std::string(pass->GetName()) == std::string(name);
        });
    
    if (it != m_passes.end()) {
        m_passes.erase(it, m_passes.end());
        LOG_DEBUG("Removed render pass: " + std::string(name));
    }
}

bool RenderPipeline::IsInitialized() const
{
    return m_initialized;
}

size_t RenderPipeline::GetPassCount() const
{
    return m_passes.size();
}

} // namespace Rendering
} // namespace Poko