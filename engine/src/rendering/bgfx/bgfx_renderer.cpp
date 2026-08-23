/**
 * @file bgfx_renderer.cpp
 * @brief bgfx renderer implementation for PokoEngine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "rendering/bgfx/bgfx_renderer.h"
#include "core/logging/logger.h"
#include <SDL2/SDL.h>
#include <cstring>
#include <algorithm>
#include <sstream>

// Placeholder bgfx functions for compilation without bgfx installed
namespace bgfx {
    struct Init {
        RendererType type;
        uint32_t width;
        uint32_t height;
        uint16_t reset;
        PlatformData platformData;
        
        struct Resolution {
            uint32_t width;
            uint32_t height;
            uint16_t reset;
        } resolution;
    };
    
    struct Caps {
        bool homogeneousDepth = false;
    };
    
    bool init(const Init&) { return true; }
    void shutdown() {}
    void reset(uint32_t, uint32_t, uint16_t) {}
    void setViewClear(uint8_t, uint32_t, uint32_t, float, uint8_t) {}
    void setViewRect(uint8_t, uint16_t, uint16_t, uint16_t, uint16_t) {}
    void setViewTransform(uint8_t, const float*, const float*) {}
    void setDebug(uint16_t) {}
    void touch(uint8_t) {}
    void frame() {}
    void setState(uint64_t) {}
    void setVertexBuffer(uint8_t, VertexBufferHandle) {}
    void setIndexBuffer(IndexBufferHandle) {}
    void setTexture(uint8_t, TextureHandle) {}
    void setUniform(UniformHandle, const void*, uint16_t) {}
    void submit(uint8_t, ProgramHandle, int, uint32_t) {}
    void destroy(VertexBufferHandle) {}
    void destroy(IndexBufferHandle) {}
    void destroy(ProgramHandle) {}
    void destroy(TextureHandle) {}
    void destroy(UniformHandle) {}
    void destroy(ShaderHandle) {}
    
    const char* getRendererName(RendererType) { return "OpenGL"; }
    RendererType getRendererType() { return RendererType::OpenGL; }
    const Caps* getCaps() { static Caps caps; return &caps; }
    
    struct Memory {
        const void* data;
        uint32_t size;
    };
    
    Memory* makeRef(const void* data, uint32_t size) {
        static Memory mem;
        mem.data = data;
        mem.size = size;
        return &mem;
    }
    
    VertexBufferHandle createVertexBuffer(const Memory*, const VertexLayout&) { return INVALID_VERTEX_BUFFER; }
    IndexBufferHandle createIndexBuffer(const Memory*) { return INVALID_INDEX_BUFFER; }
    ShaderHandle createShader(const Memory*) { return INVALID_SHADER; }
    ProgramHandle createProgram(ShaderHandle, ShaderHandle, bool) { return INVALID_PROGRAM; }
    TextureHandle createTexture2D(uint16_t, uint16_t, bool, uint16_t, TextureFormat, uint16_t, const Memory*) { return INVALID_TEXTURE; }
    
    void dbgTextClear() {}
    void dbgTextPrintf(uint16_t, uint16_t, uint8_t, const char*, ...) {}
    
    struct Stats {
        int numDraw = 0;
        int numPrimitives = 0;
        int cpuTimeEnd = 0;
        int gpuTimeEnd = 0;
    };
    
    Stats* getStats() { static Stats stats; return &stats; }
}

namespace Poko {
namespace Rendering {

BgfxRenderer::BgfxRenderer()
    : m_initialized(false)
    , m_width(0)
    , m_height(0)
    , m_view_matrix(1.0f)
    , m_proj_matrix(1.0f)
    , m_fps(60.0f)
{
    m_clear_color[0] = 0.0f;
    m_clear_color[1] = 0.0f;
    m_clear_color[2] = 0.0f;
    m_clear_color[3] = 1.0f;
}

BgfxRenderer::~BgfxRenderer()
{
    if (m_initialized) {
        Shutdown();
    }
}

bool BgfxRenderer::Initialize(void* windowHandle, const RendererConfig& config)
{
    if (m_initialized) {
        LOG_WARNING("BgfxRenderer already initialized");
        return false;
    }

    LOG_INFO("Initializing BgfxRenderer...");
    m_config = config;
    m_width = config.width;
    m_height = config.height;

    // Initialize platform-specific data
    if (!InitializePlatform(windowHandle)) {
        LOG_ERROR("BgfxRenderer: Failed to initialize platform data");
        return false;
    }

    // Initialize bgfx
    bgfx::Init init;
    init.type = config.renderer_type;
    init.resolution.width = m_width;
    init.resolution.height = m_height;
    init.resolution.reset = m_config.vsync ? bgfx::BGFX_RESET_VSYNC : bgfx::BGFX_RESET_NONE;
    init.platformData = m_platform_data;

    if (!bgfx::init(init)) {
        LOG_ERROR("BgfxRenderer: Failed to initialize bgfx");
        ShutdownPlatform();
        return false;
    }

    // Set debug text
    bgfx::setDebug(bgfx::BGFX_DEBUG_TEXT);

    // Set view 0 clear state
    bgfx::setViewClear(0, bgfx::BGFX_CLEAR_COLOR | bgfx::BGFX_CLEAR_DEPTH, 0x000000FF, 1.0f, 0);

    // Set view 0 and projection
    bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));
    
    // Set default projection matrix (perspective)
    // Placeholder for matrix calculation
    float proj[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    bgfx::setViewTransform(0, nullptr, proj);

    LOG_INFO("BgfxRenderer initialized successfully with resolution: " + 
            std::to_string(m_width) + "x" + std::to_string(m_height));
    
    m_initialized = true;
    return true;
}

bool BgfxRenderer::InitializePlatform(void* windowHandle)
{
    // Platform-specific window handle setup
    // This is a simplified version for compilation without bgfx platform headers
    m_platform_data.nwh = windowHandle;
    m_platform_data.ndt = nullptr;
    m_platform_data.context = nullptr;
    m_platform_data.backBuffer = nullptr;
    m_platform_data.backBufferDS = nullptr;
    
    return true;
}

void BgfxRenderer::ShutdownPlatform()
{
    // Clean up platform-specific resources
    m_platform_data = {};
}

void BgfxRenderer::Shutdown()
{
    if (!m_initialized) {
        return;
    }

    LOG_INFO("Shutting down BgfxRenderer...");

    // Destroy all resources
    for (auto handle : m_vertex_buffers) {
        if (bgfx::isValid(handle)) {
            bgfx::destroy(handle);
        }
    }
    m_vertex_buffers.clear();

    for (auto handle : m_index_buffers) {
        if (bgfx::isValid(handle)) {
            bgfx::destroy(handle);
        }
    }
    m_index_buffers.clear();

    for (auto handle : m_shader_programs) {
        if (bgfx::isValid(handle)) {
            bgfx::destroy(handle);
        }
    }
    m_shader_programs.clear();

    for (auto handle : m_textures) {
        if (bgfx::isValid(handle)) {
            bgfx::destroy(handle);
        }
    }
    m_textures.clear();

    // Shutdown bgfx
    bgfx::shutdown();
    
    // Shutdown platform
    ShutdownPlatform();

    m_initialized = false;
    LOG_INFO("BgfxRenderer shutdown complete");
}

void BgfxRenderer::BeginFrame()
{
    if (!m_initialized) {
        return;
    }

    bgfx::reset(m_width, m_height, m_config.vsync ? bgfx::BGFX_RESET_VSYNC : bgfx::BGFX_RESET_NONE);
    bgfx::touch(0);
}

void BgfxRenderer::EndFrame()
{
    if (!m_initialized) {
        return;
    }

    bgfx::frame();
}

void BgfxRenderer::Clear(float r, float g, float b, float a)
{
    if (!m_initialized) {
        return;
    }

    m_clear_color[0] = r;
    m_clear_color[1] = g;
    m_clear_color[2] = b;
    m_clear_color[3] = a;

    uint32_t clear_color = 
        (uint32_t)(a * 255.0f) << 24 |
        (uint32_t)(b * 255.0f) << 16 |
        (uint32_t)(g * 255.0f) << 8 |
        (uint32_t)(r * 255.0f);

    bgfx::setViewClear(0, bgfx::BGFX_CLEAR_COLOR | bgfx::BGFX_CLEAR_DEPTH, clear_color, 1.0f, 0);
}

void BgfxRenderer::SetViewProjection(const glm::mat4& view_matrix, const glm::mat4& proj_matrix, uint8_t view_id)
{
    if (!m_initialized) {
        return;
    }

    m_view_matrix = view_matrix;
    m_proj_matrix = proj_matrix;

    float view[16];
    float proj[16];
    
    // Convert glm matrices to bgfx format
    memcpy(view, &view_matrix[0][0], sizeof(float) * 16);
    memcpy(proj, &proj_matrix[0][0], sizeof(float) * 16);

    bgfx::setViewTransform(view_id, view, proj);
}

bgfx::VertexBufferHandle BgfxRenderer::CreateVertexBuffer(const void* vertices, uint32_t vertex_size, uint32_t count)
{
    if (!m_initialized) {
        LOG_ERROR("BgfxRenderer: Cannot create vertex buffer - not initialized");
        return bgfx::INVALID_VERTEX_BUFFER;
    }

    bgfx::VertexBufferHandle handle = bgfx::createVertexBuffer(
        bgfx::makeRef(vertices, vertex_size * count),
        bgfx::VertexLayout()
    );

    if (bgfx::isValid(handle)) {
        m_vertex_buffers.push_back(handle);
        LOG_DEBUG("Created vertex buffer with " + std::to_string(count) + " vertices");
    } else {
        LOG_ERROR("BgfxRenderer: Failed to create vertex buffer");
    }

    return handle;
}

bgfx::IndexBufferHandle BgfxRenderer::CreateIndexBuffer(const uint16_t* indices, uint32_t count)
{
    if (!m_initialized) {
        LOG_ERROR("BgfxRenderer: Cannot create index buffer - not initialized");
        return bgfx::INVALID_INDEX_BUFFER;
    }

    bgfx::IndexBufferHandle handle = bgfx::createIndexBuffer(
        bgfx::makeRef(indices, sizeof(uint16_t) * count)
    );

    if (bgfx::isValid(handle)) {
        m_index_buffers.push_back(handle);
        LOG_DEBUG("Created index buffer with " + std::to_string(count) + " indices");
    } else {
        LOG_ERROR("BgfxRenderer: Failed to create index buffer");
    }

    return handle;
}

bgfx::ProgramHandle BgfxRenderer::CreateShaderProgram(const char* vertex_shader, const char* fragment_shader)
{
    if (!m_initialized) {
        LOG_ERROR("BgfxRenderer: Cannot create shader program - not initialized");
        return bgfx::INVALID_PROGRAM;
    }

    // Create vertex shader
    bgfx::ShaderHandle vertex_handle = bgfx::createShader(
        bgfx::makeRef(vertex_shader, strlen(vertex_shader))
    );

    if (!bgfx::isValid(vertex_handle)) {
        LOG_ERROR("BgfxRenderer: Failed to create vertex shader");
        return bgfx::INVALID_PROGRAM;
    }

    // Create fragment shader
    bgfx::ShaderHandle fragment_handle = bgfx::createShader(
        bgfx::makeRef(fragment_shader, strlen(fragment_shader))
    );

    if (!bgfx::isValid(fragment_handle)) {
        LOG_ERROR("BgfxRenderer: Failed to create fragment shader");
        bgfx::destroy(vertex_handle);
        return bgfx::INVALID_PROGRAM;
    }

    // Create program
    bgfx::ProgramHandle program_handle = bgfx::createProgram(vertex_handle, fragment_handle, true);

    if (bgfx::isValid(program_handle)) {
        m_shader_programs.push_back(program_handle);
        LOG_DEBUG("Created shader program successfully");
        
        // Clean up shader handles
        bgfx::destroy(vertex_handle);
        bgfx::destroy(fragment_handle);
    } else {
        LOG_ERROR("BgfxRenderer: Failed to create shader program");
        bgfx::destroy(vertex_handle);
        bgfx::destroy(fragment_handle);
    }

    return program_handle;
}

bgfx::TextureHandle BgfxRenderer::CreateTexture(const void* data, uint16_t width, uint16_t height, bgfx::TextureFormat format)
{
    if (!m_initialized) {
        LOG_ERROR("BgfxRenderer: Cannot create texture - not initialized");
        return bgfx::INVALID_TEXTURE;
    }

    // Simplified texture creation for placeholder
    const bgfx::Memory* mem = bgfx::makeRef(data, width * height * 4); // Assuming RGBA for now

    bgfx::TextureHandle handle = bgfx::createTexture2D(width, height, false, 1, format, bgfx::BGFX_SAMPLER_NONE, mem);

    if (bgfx::isValid(handle)) {
        m_textures.push_back(handle);
        LOG_DEBUG("Created texture with size: " + std::to_string(width) + "x" + std::to_string(height));
    } else {
        LOG_ERROR("BgfxRenderer: Failed to create texture");
    }

    return handle;
}

void BgfxRenderer::Draw(bgfx::VertexBufferHandle vertex_buffer, bgfx::IndexBufferHandle index_buffer, 
                       bgfx::ProgramHandle program, uint32_t num_indices, uint8_t view_id)
{
    if (!m_initialized) {
        return;
    }

    if (!bgfx::isValid(vertex_buffer) || !bgfx::isValid(index_buffer) || !bgfx::isValid(program)) {
        LOG_ERROR("BgfxRenderer: Invalid handles for draw call");
        return;
    }

    bgfx::setVertexBuffer(0, vertex_buffer);
    bgfx::setIndexBuffer(index_buffer);
    bgfx::setState(bgfx::BGFX_STATE_DEFAULT);
    bgfx::submit(view_id, program, 0, num_indices);
}

void BgfxRenderer::SetTexture(bgfx::TextureHandle texture, uint8_t stage, uint8_t view_id)
{
    if (!m_initialized) {
        return;
    }

    (void)view_id; // Suppress unused parameter warning

    if (!bgfx::isValid(texture)) {
        LOG_ERROR("BgfxRenderer: Invalid texture handle");
        return;
    }

    bgfx::setTexture(stage, texture);
}

void BgfxRenderer::SetUniform(bgfx::UniformHandle uniform, const void* value, uint16_t num)
{
    if (!m_initialized) {
        return;
    }

    if (!bgfx::isValid(uniform)) {
        LOG_ERROR("BgfxRenderer: Invalid uniform handle");
        return;
    }

    bgfx::setUniform(uniform, value, num);
}

std::string BgfxRenderer::GetStats() const
{
    if (!m_initialized) {
        return "Renderer not initialized";
    }

    const bgfx::Stats* stats = bgfx::getStats();
    
    std::ostringstream stats_str;
    stats_str << "Renderer Statistics:\n";
    stats_str << "  Resolution: " << m_width << "x" << m_height << "\n";
    stats_str << "  Vertex Buffers: " << m_vertex_buffers.size() << "\n";
    stats_str << "  Index Buffers: " << m_index_buffers.size() << "\n";
    stats_str << "  Shader Programs: " << m_shader_programs.size() << "\n";
    stats_str << "  Textures: " << m_textures.size() << "\n";
    stats_str << "  Num Draws: " << stats->numDraw << "\n";
    stats_str << "  Num Primitives: " << stats->numPrimitives << "\n";
    stats_str << "  CPU Time: " << stats->cpuTimeEnd << "ms\n";
    stats_str << "  GPU Time: " << stats->gpuTimeEnd << "ms";
    
    return stats_str.str();
}

void BgfxRenderer::Resize(uint32_t width, uint32_t height)
{
    if (!m_initialized) {
        return;
    }

    m_width = width;
    m_height = height;

    bgfx::reset(width, height, m_config.vsync ? bgfx::BGFX_RESET_VSYNC : bgfx::BGFX_RESET_NONE);
    bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));

    LOG_INFO("Renderer resized to: " + std::to_string(width) + "x" + std::to_string(height));
}

std::pair<uint32_t, uint32_t> BgfxRenderer::GetResolution() const
{
    return {m_width, m_height};
}

void BgfxRenderer::SetClearColor(float r, float g, float b, float a)
{
    m_clear_color[0] = r;
    m_clear_color[1] = g;
    m_clear_color[2] = b;
    m_clear_color[3] = a;
}

void BgfxRenderer::SetRenderState(uint64_t state, uint8_t view_id)
{
    if (!m_initialized) {
        return;
    }

    (void)view_id; // Suppress unused parameter warning

    bgfx::setState(state);
}

void BgfxRenderer::ResetRenderState(uint8_t view_id)
{
    if (!m_initialized) {
        return;
    }

    (void)view_id; // Suppress unused parameter warning

    bgfx::setState(bgfx::BGFX_STATE_DEFAULT);
}

void BgfxRenderer::DestroyVertexBuffer(bgfx::VertexBufferHandle handle)
{
    if (bgfx::isValid(handle)) {
        bgfx::destroy(handle);
        
        // Remove from tracking vector
        auto it = std::find(m_vertex_buffers.begin(), m_vertex_buffers.end(), handle);
        if (it != m_vertex_buffers.end()) {
            m_vertex_buffers.erase(it);
        }
    }
}

void BgfxRenderer::DestroyIndexBuffer(bgfx::IndexBufferHandle handle)
{
    if (bgfx::isValid(handle)) {
        bgfx::destroy(handle);
        
        // Remove from tracking vector
        auto it = std::find(m_index_buffers.begin(), m_index_buffers.end(), handle);
        if (it != m_index_buffers.end()) {
            m_index_buffers.erase(it);
        }
    }
}

void BgfxRenderer::DestroyShaderProgram(bgfx::ProgramHandle handle)
{
    if (bgfx::isValid(handle)) {
        bgfx::destroy(handle);
        
        // Remove from tracking vector
        auto it = std::find(m_shader_programs.begin(), m_shader_programs.end(), handle);
        if (it != m_shader_programs.end()) {
            m_shader_programs.erase(it);
        }
    }
}

void BgfxRenderer::DestroyTexture(bgfx::TextureHandle handle)
{
    if (bgfx::isValid(handle)) {
        bgfx::destroy(handle);
        
        // Remove from tracking vector
        auto it = std::find(m_textures.begin(), m_textures.end(), handle);
        if (it != m_textures.end()) {
            m_textures.erase(it);
        }
    }
}

void BgfxRenderer::SetDebugText(bool enable)
{
    if (!m_initialized) {
        return;
    }

    uint16_t debug_flags = enable ? bgfx::BGFX_DEBUG_TEXT : 0;
    bgfx::setDebug(debug_flags);
}

void BgfxRenderer::SetDebugPosition(uint16_t x, uint16_t y)
{
    if (!m_initialized) {
        return;
    }

    (void)x; // Suppress unused parameter warning

    bgfx::dbgTextClear();
    // Could display various stats here
    bgfx::dbgTextPrintf(0, y, 0x0f, "Resolution: %dx%d", m_width, m_height);
}

std::string BgfxRenderer::GetGPUName() const
{
    if (!m_initialized) {
        return "Renderer not initialized";
    }

    const char* renderer_name = bgfx::getRendererName(bgfx::getRendererType());
    return renderer_name ? renderer_name : "Unknown";
}

std::string BgfxRenderer::GetRendererType() const
{
    if (!m_initialized) {
        return "Renderer not initialized";
    }

    switch (bgfx::getRendererType()) {
        case bgfx::RendererType::Count:
            return "Auto-detected";
        case bgfx::RendererType::OpenGL:
            return "OpenGL";
        case bgfx::RendererType::Direct3D11:
            return "Direct3D 11";
        case bgfx::RendererType::Vulkan:
            return "Vulkan";
        default:
            return "Unknown";
    }
}

void BgfxRenderer::UpdateFPS(float fps)
{
    m_fps = fps;
}

bool BgfxRenderer::IsInitialized() const
{
    return m_initialized;
}

} // namespace Rendering
} // namespace Poko