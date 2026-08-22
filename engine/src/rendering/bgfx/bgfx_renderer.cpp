/**
 * @file bgfx_renderer.cpp
 * @brief bgfx renderer implementation for PokoEngine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "rendering/bgfx/bgfx_renderer.h"
#include <iostream>

namespace Poko {
namespace Rendering {

BgfxRenderer::BgfxRenderer()
    : m_initialized(false)
    , m_width(0)
    , m_height(0)
{
}

BgfxRenderer::~BgfxRenderer()
{
    if (m_initialized) {
        Shutdown();
    }
}

bool BgfxRenderer::Initialize(void* windowHandle, uint32_t width, uint32_t height)
{
    if (m_initialized) {
        std::cerr << "BgfxRenderer already initialized" << std::endl;
        return false;
    }

    std::cout << "Initializing BgfxRenderer..." << std::endl;
    std::cout << "Window handle: " << windowHandle << std::endl;
    std::cout << "Resolution: " << width << "x" << height << std::endl;

    // bgfx initialization will be added here
    // For now, just store the parameters
    m_width = width;
    m_height = height;
    m_initialized = true;

    std::cout << "BgfxRenderer initialized successfully" << std::endl;
    return true;
}

void BgfxRenderer::Shutdown()
{
    if (!m_initialized) {
        return;
    }

    std::cout << "Shutting down BgfxRenderer..." << std::endl;

    // bgfx shutdown will be added here

    m_initialized = false;
    std::cout << "BgfxRenderer shutdown complete" << std::endl;
}

void BgfxRenderer::BeginFrame()
{
    if (!m_initialized) {
        return;
    }

    // bgfx frame begin will be added here
}

void BgfxRenderer::EndFrame()
{
    if (!m_initialized) {
        return;
    }

    // bgfx frame end and present will be added here
}

void BgfxRenderer::Clear(float r, float g, float b, float a)
{
    if (!m_initialized) {
        return;
    }

    // bgfx clear will be added here
    (void)r; (void)g; (void)b; (void)a; // Suppress unused parameter warnings
}

bool BgfxRenderer::IsInitialized() const
{
    return m_initialized;
}

} // namespace Rendering
} // namespace Poko