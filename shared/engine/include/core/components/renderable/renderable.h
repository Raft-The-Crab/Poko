/**
 * @file renderable.h
 * @brief Renderable component header
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_RENDERABLE_RENDERABLE_H
#define POKO_CORE_COMPONENTS_RENDERABLE_RENDERABLE_H

#include <cstdint>
#include <string>
#include "../component.h"

namespace poko {
namespace core {
namespace components {
namespace renderable {

// ============================================================================
// Render Layer
// ============================================================================

/**
 * @brief Render layer for visibility control
 */
enum class RenderLayer : uint32_t {
    Default = 0,    ///< Default render layer
    UI = 1,         ///< UI layer (always on top)
    Background = 2, ///< Background layer (always behind)
    Water = 3,      ///< Water rendering layer
    Transparent = 4, ///< Transparent objects
    Effects = 5,    ///< Particle effects
    Overlay = 6,    ///< Post-processing overlay
    Custom = 7      ///< Custom user-defined layers
};

// ============================================================================
// Render Queue
// ============================================================================

/**
 * @brief Render queue for ordering
 */
enum class RenderQueue : uint32_t {
    Background = 0,    ///< Background geometry
    Opaque = 1000,     ///< Opaque geometry (sorted front-to-back)
    Transparent = 2000, ///< Transparent geometry (sorted back-to-front)
    Overlay = 3000,    ///< UI and overlay elements
};

// ============================================================================
// Bounding Box
// ============================================================================

/**
 * @brief Axis-aligned bounding box
 */
struct BoundingBox {
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
    
    /**
     * @brief Default constructor - empty box
     */
    constexpr BoundingBox() noexcept 
        : minX(0.0f), minY(0.0f), minZ(0.0f)
        , maxX(0.0f), maxY(0.0f), maxZ(0.0f) {}
    
    /**
     * @brief Construct from min and max
     */
    constexpr BoundingBox(float minX_, float minY_, float minZ_,
                        float maxX_, float maxY_, float maxZ_) noexcept
        : minX(minX_), minY(minY_), minZ(minZ_)
        , maxX(maxX_), maxY(maxY_), maxZ(maxZ_) {}
    
    /**
     * @brief Get center point
     */
    [[nodiscard]] constexpr float centerX() const noexcept { return (minX + maxX) * 0.5f; }
    [[nodiscard]] constexpr float centerY() const noexcept { return (minY + maxY) * 0.5f; }
    [[nodiscard]] constexpr float centerZ() const noexcept { return (minZ + maxZ) * 0.5f; }
    
    /**
     * @brief Get extents
     */
    [[nodiscard]] constexpr float extentX() const noexcept { return maxX - minX; }
    [[nodiscard]] constexpr float extentY() const noexcept { return maxY - minY; }
    [[nodiscard]] constexpr float extentZ() const noexcept { return maxZ - minZ; }
};

// ============================================================================
// Renderable Component
// ============================================================================

/**
 * @brief Renderable component type ID
 */
constexpr ComponentID RENDERABLE_COMPONENT_ID = 1;

/**
 * @brief Renderable component type name
 */
constexpr const char* RENDERABLE_COMPONENT_NAME = "Renderable";

/**
 * @brief Renderable component for rendering properties
 * 
 * This component handles:
 * - Material reference
 * - Mesh reference
 * - Visibility state
 * - Layer/mask for rendering control
 * - Bounds for culling
 * - Render queue priority
 * - Shadow casting control
 * 
 * @section thread_safety Thread Safety
 * Renderable component is not thread-safe by default.
 * Access must be synchronized externally if used from multiple threads.
 */
class Renderable : public Component {
public:
    // Component constants
    static constexpr ComponentID COMPONENT_ID = RENDERABLE_COMPONENT_ID;
    static constexpr const char* COMPONENT_NAME = RENDERABLE_COMPONENT_NAME;
    
    /**
     * @brief Constructor
     */
    Renderable() noexcept;
    
    /**
     * @brief Destructor
     */
    ~Renderable() override = default;
    
    // ============================================================================
    // Component Interface
    // ============================================================================
    
    /**
     * @brief Get component type ID
     */
    [[nodiscard]] ComponentID getTypeID() const noexcept override { return COMPONENT_ID; }
    
    /**
     * @brief Get component type name
     */
    [[nodiscard]] const char* getTypeName() const noexcept override { return COMPONENT_NAME; }
    
    /**
     * @brief Called when component is created
     */
    void onCreate() override;
    
    /**
     * @brief Called when component is activated
     */
    void onActivate() override;
    
    /**
     * @brief Called when component is deactivated
     */
    void onDeactivate() override;
    
    /**
     * @brief Called when component is destroyed
     */
    void onDestroy() override;
    
    // ============================================================================
    // Visibility
    // ============================================================================
    
    /**
     * @brief Set visibility
     * @param visible True if visible
     */
    void setVisible(bool visible) noexcept { m_visible = visible; }
    
    /**
     * @brief Get visibility
     * @return True if visible
     */
    [[nodiscard]] bool isVisible() const noexcept { return m_visible; }
    
    // ============================================================================
    // Material
    // ============================================================================
    
    /**
     * @brief Set material ID
     * @param materialId Material resource ID
     */
    void setMaterialId(uint64_t materialId) noexcept { m_materialId = materialId; }
    
    /**
     * @brief Get material ID
     * @return Material resource ID
     */
    [[nodiscard]] uint64_t getMaterialId() const noexcept { return m_materialId; }
    
    // ============================================================================
    // Mesh
    // ============================================================================
    
    /**
     * @brief Set mesh ID
     * @param meshId Mesh resource ID
     */
    void setMeshId(uint64_t meshId) noexcept { m_meshId = meshId; }
    
    /**
     * @brief Get mesh ID
     * @return Mesh resource ID
     */
    [[nodiscard]] uint64_t getMeshId() const noexcept { return m_meshId; }
    
    // ============================================================================
    // Render Layer
    // ============================================================================
    
    /**
     * @brief Set render layer
     * @param layer Render layer
     */
    void setRenderLayer(RenderLayer layer) noexcept { m_renderLayer = layer; }
    
    /**
     * @brief Get render layer
     * @return Render layer
     */
    [[nodiscard]] RenderLayer getRenderLayer() const noexcept { return m_renderLayer; }
    
    // ============================================================================
    // Render Queue
    // ============================================================================
    
    /**
     * @brief Set render queue
     * @param queue Render queue
     */
    void setRenderQueue(RenderQueue queue) noexcept { m_renderQueue = queue; }
    
    /**
     * @brief Get render queue
     * @return Render queue
     */
    [[nodiscard]] RenderQueue getRenderQueue() const noexcept { return m_renderQueue; }
    
    // ============================================================================
    // Bounds
    // ============================================================================
    
    /**
     * @brief Set bounding box
     * @param bounds Bounding box
     */
    void setBounds(const BoundingBox& bounds) noexcept { m_bounds = bounds; }
    
    /**
     * @brief Get bounding box
     * @return Bounding box
     */
    [[nodiscard]] const BoundingBox& getBounds() const noexcept { return m_bounds; }
    
    // ============================================================================
    // Shadow Casting
    // ============================================================================
    
    /**
     * @brief Set shadow casting
     * @param castShadows True if casts shadows
     */
    void setCastShadows(bool castShadows) noexcept { m_castShadows = castShadows; }
    
    /**
     * @brief Get shadow casting
     * @return True if casts shadows
     */
    [[nodiscard]] bool castsShadows() const noexcept { return m_castShadows; }
    
    // ============================================================================
    // Shadow Receiving
    // ============================================================================
    
    /**
     * @brief Set shadow receiving
     * @param receiveShadows True if receives shadows
     */
    void setReceiveShadows(bool receiveShadows) noexcept { m_receiveShadows = receiveShadows; }
    
    /**
     * @brief Get shadow receiving
     * @return True if receives shadows
     */
    [[nodiscard]] bool receivesShadows() const noexcept { return m_receiveShadows; }
    
    // ============================================================================
    // Custom Data
    // ============================================================================
    
    /**
     * @brief Set custom user data
     * @param userData User data pointer
     */
    void setUserData(void* userData) noexcept { m_userData = userData; }
    
    /**
     * @brief Get custom user data
     * @return User data pointer
     */
    [[nodiscard]] void* getUserData() const noexcept { return m_userData; }
    
private:
    // Visibility
    bool m_visible;
    
    // Resources
    uint64_t m_materialId;
    uint64_t m_meshId;
    
    // Render properties
    RenderLayer m_renderLayer;
    RenderQueue m_renderQueue;
    
    // Bounds
    BoundingBox m_bounds;
    
    // Shadows
    bool m_castShadows;
    bool m_receiveShadows;
    
    // Custom data
    void* m_userData;
};

// ============================================================================
// Constants
// ============================================================================

/// Invalid resource ID
constexpr uint64_t INVALID_RESOURCE_ID = 0;

/// Maximum material name length
constexpr size_t MAX_MATERIAL_NAME_LENGTH = 256;

/// Maximum mesh name length
constexpr size_t MAX_MESH_NAME_LENGTH = 256;

} // namespace renderable

/**
 * @brief Register Renderable component with global registry
 */
void registerRenderableComponent();

} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_RENDERABLE_RENDERABLE_H
