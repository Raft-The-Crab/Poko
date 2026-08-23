/**
 * @file bgfx_renderer.h
 * @brief bgfx renderer header for PokoEngine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>
#include <string>

// Placeholder bgfx types for compilation without bgfx installed
namespace bgfx {
    enum class RendererType { Count = 0, OpenGL = 1, Direct3D11 = 2, Vulkan = 3 };
    enum class TextureFormat { RGBA8 = 0 };
    
    struct VertexBufferHandle { uint16_t idx = 0xFFFF; };
    struct IndexBufferHandle { uint16_t idx = 0xFFFF; };
    struct ProgramHandle { uint16_t idx = 0xFFFF; };
    struct TextureHandle { uint16_t idx = 0xFFFF; };
    struct UniformHandle { uint16_t idx = 0xFFFF; };
    struct ShaderHandle { uint16_t idx = 0xFFFF; };
    struct VertexLayout {};
    
    // Add comparison operators for STL compatibility
    inline bool operator==(const VertexBufferHandle& a, const VertexBufferHandle& b) { return a.idx == b.idx; }
    inline bool operator==(const IndexBufferHandle& a, const IndexBufferHandle& b) { return a.idx == b.idx; }
    inline bool operator==(const ProgramHandle& a, const ProgramHandle& b) { return a.idx == b.idx; }
    inline bool operator==(const TextureHandle& a, const TextureHandle& b) { return a.idx == b.idx; }
    inline bool operator==(const UniformHandle& a, const UniformHandle& b) { return a.idx == b.idx; }
    inline bool operator==(const ShaderHandle& a, const ShaderHandle& b) { return a.idx == b.idx; }
    
    struct PlatformData {
        void* nwh = nullptr;
        void* ndt = nullptr;
        void* context = nullptr;
        void* backBuffer = nullptr;
        void* backBufferDS = nullptr;
    };
    
    inline bool isValid(const VertexBufferHandle&) { return false; }
    inline bool isValid(const IndexBufferHandle&) { return false; }
    inline bool isValid(const ProgramHandle&) { return false; }
    inline bool isValid(const TextureHandle&) { return false; }
    inline bool isValid(const UniformHandle&) { return false; }
    inline bool isValid(const ShaderHandle&) { return false; }
    
    // Use separate names for invalid handles to avoid conflicts
    constexpr VertexBufferHandle INVALID_VERTEX_BUFFER = {0xFFFF};
    constexpr IndexBufferHandle INVALID_INDEX_BUFFER = {0xFFFF};
    constexpr ProgramHandle INVALID_PROGRAM = {0xFFFF};
    constexpr TextureHandle INVALID_TEXTURE = {0xFFFF};
    constexpr UniformHandle INVALID_UNIFORM = {0xFFFF};
    constexpr ShaderHandle INVALID_SHADER = {0xFFFF};
    
    constexpr uint16_t BGFX_RESET_VSYNC = 0x1;
    constexpr uint16_t BGFX_RESET_NONE = 0x0;
    constexpr uint32_t BGFX_CLEAR_COLOR = 0x1;
    constexpr uint32_t BGFX_CLEAR_DEPTH = 0x2;
    constexpr uint32_t BGFX_STATE_DEFAULT = 0x0;
    constexpr uint16_t BGFX_DEBUG_TEXT = 0x10;
    constexpr uint16_t BGFX_SAMPLER_NONE = 0x0;
}

namespace Poko {
namespace Rendering {

/**
 * @brief bgfx renderer class for 3D rendering
 */
class BgfxRenderer {
public:
    struct RendererConfig {
        uint32_t width = 1280;
        uint32_t height = 720;
        bool vsync = true;
        uint32_t max_fps = 60;
        bgfx::RendererType renderer_type = bgfx::RendererType::Count; // Auto-detect
    };

    BgfxRenderer();
    ~BgfxRenderer();

    /**
     * @brief Initialize the renderer
     * @param windowHandle Native window handle
     * @param config Renderer configuration
     * @return true if initialization succeeded
     */
    bool Initialize(void* windowHandle, const RendererConfig& config);

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

    /**
     * @brief Set view and projection matrices
     * @param view_matrix View matrix
     * @param proj_matrix Projection matrix
     * @param view_id View ID (default 0)
     */
    void SetViewProjection(const glm::mat4& view_matrix, const glm::mat4& proj_matrix, uint8_t view_id = 0);

    /**
     * @brief Create vertex buffer
     * @param vertices Vertex data
     * @param vertex_size Size of each vertex
     * @param count Number of vertices
     * @return Vertex buffer handle
     */
    bgfx::VertexBufferHandle CreateVertexBuffer(const void* vertices, uint32_t vertex_size, uint32_t count);

    /**
     * @brief Create index buffer
     * @param indices Index data
     * @param count Number of indices
     * @return Index buffer handle
     */
    bgfx::IndexBufferHandle CreateIndexBuffer(const uint16_t* indices, uint32_t count);

    /**
     * @brief Create shader program
     * @param vertex_shader Vertex shader code
     * @param fragment_shader Fragment shader code
     * @return Shader program handle
     */
    bgfx::ProgramHandle CreateShaderProgram(const char* vertex_shader, const char* fragment_shader);

    /**
     * @brief Create texture
     * @param data Texture data
     * @param width Texture width
     * @param height Texture height
     * @param format Texture format
     * @return Texture handle
     */
    bgfx::TextureHandle CreateTexture(const void* data, uint16_t width, uint16_t height, bgfx::TextureFormat format);

    /**
     * @brief Draw geometry
     * @param vertex_buffer Vertex buffer handle
     * @param index_buffer Index buffer handle
     * @param program Shader program handle
     * @param num_indices Number of indices to draw
     * @param view_id View ID (default 0)
     */
    void Draw(bgfx::VertexBufferHandle vertex_buffer, bgfx::IndexBufferHandle index_buffer, 
             bgfx::ProgramHandle program, uint32_t num_indices, uint8_t view_id = 0);

    /**
     * @brief Set texture for a slot
     * @param texture Texture handle
     * @param stage Texture stage (default 0)
     * @param view_id View ID (default 0)
     */
    void SetTexture(bgfx::TextureHandle texture, uint8_t stage = 0, uint8_t view_id = 0);

    /**
     * @brief Set uniform value
     * @param uniform Uniform handle
     * @param value Uniform value
     * @param num Number of values
     */
    void SetUniform(bgfx::UniformHandle uniform, const void* value, uint16_t num = 1);

    /**
     * @brief Get renderer statistics
     * @return Statistics string
     */
    std::string GetStats() const;

    /**
     * @brief Resize renderer
     * @param width New width
     * @param height New height
     */
    void Resize(uint32_t width, uint32_t height);

    /**
     * @brief Get current resolution
     * @return Pair of (width, height)
     */
    std::pair<uint32_t, uint32_t> GetResolution() const;

    /**
     * @brief Set clear color
     * @param r Red component (0-1)
     * @param g Green component (0-1)
     * @param b Blue component (0-1)
     * @param a Alpha component (0-1)
     */
    void SetClearColor(float r, float g, float b, float a);

    /**
     * @brief Set render state
     * @param state Render state flags
     * @param view_id View ID (default 0)
     */
    void SetRenderState(uint64_t state, uint8_t view_id = 0);

    /**
     * @brief Reset render state to default
     * @param view_id View ID (default 0)
     */
    void ResetRenderState(uint8_t view_id = 0);

    /**
     * @brief Destroy vertex buffer
     * @param handle Vertex buffer handle
     */
    void DestroyVertexBuffer(bgfx::VertexBufferHandle handle);

    /**
     * @brief Destroy index buffer
     * @param handle Index buffer handle
     */
    void DestroyIndexBuffer(bgfx::IndexBufferHandle handle);

    /**
     * @brief Destroy shader program
     * @param handle Shader program handle
     */
    void DestroyShaderProgram(bgfx::ProgramHandle handle);

    /**
     * @brief Destroy texture
     * @param handle Texture handle
     */
    void DestroyTexture(bgfx::TextureHandle handle);

    /**
     * @brief Enable/disable debug text
     * @param enable Whether to enable debug text
     */
    void SetDebugText(bool enable);

    /**
     * @brief Set debug position
     @param x X position
     * @param y Y position
     */
    void SetDebugPosition(uint16_t x, uint16_t y);

    /**
     * @brief Get GPU name
     * @return GPU name string
     */
    std::string GetGPUName() const;

    /**
     * @brief Get renderer type name
     * @return Renderer type string
     */
    std::string GetRendererType() const;

    /**
     * @brief Update FPS from engine (for debug display)
     * @param fps Current FPS
     */
    void UpdateFPS(float fps);

private:
    bool m_initialized;
    RendererConfig m_config;
    uint32_t m_width;
    uint32_t m_height;
    
    // View and projection matrices
    glm::mat4 m_view_matrix;
    glm::mat4 m_proj_matrix;
    
    // Clear color
    float m_clear_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    
    // FPS for debug display
    float m_fps = 60.0f;
    
    // Platform-specific window data
    bgfx::PlatformData m_platform_data;
    
    // Resource tracking
    std::vector<bgfx::VertexBufferHandle> m_vertex_buffers;
    std::vector<bgfx::IndexBufferHandle> m_index_buffers;
    std::vector<bgfx::ProgramHandle> m_shader_programs;
    std::vector<bgfx::TextureHandle> m_textures;
    
    // Initialization helper
    bool InitializePlatform(void* windowHandle);
    void ShutdownPlatform();
};

} // namespace Rendering
} // namespace Poko