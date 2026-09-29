/**
 * @file property.h
 * @brief Runtime property system header
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 * 
 * This module provides a type-safe property system with metadata,
 * change notifications, and serialization support for runtime objects.
 */

#ifndef POKO_CORE_PROPERTIES_PROPERTY_H
#define POKO_CORE_PROPERTIES_PROPERTY_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <variant>
#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <mutex>

namespace poko {
namespace core {
namespace properties {

// ============================================================================
// Property Types
// ============================================================================

/**
 * @brief Property value types supported by the property system
 */
enum class PropertyType : uint32_t {
    Invalid = 0,
    Bool,
    Int32,
    Int64,
    Float,
    Double,
    String,
    Vector2,
    Vector3,
    Vector4,
    Color,
    Count
};

/**
 * @brief Property flags controlling behavior
 */
enum class PropertyFlags : uint32_t {
    None = 0,
    ReadOnly = 1 << 0,           ///< Property cannot be modified after initialization
    Transient = 1 << 1,           ///< Property is not serialized
    Replicated = 1 << 2,          ///< Property is replicated over network
    EditorOnly = 1 << 3,         ///< Property only exists in editor builds
    RuntimeOnly = 1 << 4,        ///< Property only exists in runtime builds
    Clamp = 1 << 5,               ///< Value should be clamped to min/max
    Hidden = 1 << 6               ///< Property hidden from editor UI
};

// Property flags operators
inline PropertyFlags operator|(PropertyFlags a, PropertyFlags b) {
    return static_cast<PropertyFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline PropertyFlags operator&(PropertyFlags a, PropertyFlags b) {
    return static_cast<PropertyFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline bool hasFlag(PropertyFlags flags, PropertyFlags flag) {
    return (flags & flag) == flag;
}

// ============================================================================
// Property Value Variant
// ============================================================================

/**
 * @brief Variant type for property values
 */
using PropertyValue = std::variant<
    bool,
    int32_t,
    int64_t,
    float,
    double,
    std::string,
    std::vector<float>  // For vectors and colors
>;

// ============================================================================
// Constants
// ============================================================================

/// Maximum property name length (for safety)
constexpr size_t MAX_PROPERTY_NAME_LENGTH = 128;

/// Maximum property category length (for safety)
constexpr size_t MAX_PROPERTY_CATEGORY_LENGTH = 64;

/// Maximum property description length (for safety)
constexpr size_t MAX_PROPERTY_DESCRIPTION_LENGTH = 512;

/// Maximum properties per registry (for safety)
constexpr size_t MAX_PROPERTIES_PER_REGISTRY = 256;

/// Maximum vector size for property values (for safety)
constexpr size_t MAX_PROPERTY_VECTOR_SIZE = 4;

// ============================================================================
// Property Metadata
// ============================================================================

/**
 * @brief Metadata describing a property
 */
struct PropertyMetadata {
    std::string name;           ///< Property name
    PropertyType type;          ///< Property type
    PropertyFlags flags;        ///< Property flags
    PropertyValue defaultValue; ///< Default value
    PropertyValue minValue;     ///< Minimum value (for numeric types)
    PropertyValue maxValue;     ///< Maximum value (for numeric types)
    std::string category;       ///< Category for editor grouping
    std::string description;    ///< Human-readable description
    
    PropertyMetadata()
        : type(PropertyType::Invalid)
        , flags(PropertyFlags::None)
        , defaultValue(int32_t(0))
        , minValue(int32_t(0))
        , maxValue(int32_t(0))
    {}
};

// ============================================================================
// Property Change Callback
// ============================================================================

/**
 * @brief Callback type for property change notifications
 */
using PropertyChangeCallback = std::function<void(const std::string& name, const PropertyValue& oldValue, const PropertyValue& newValue)>;

// ============================================================================
// Property Class
// ============================================================================

/**
 * @brief Individual property with type-safe value and metadata
 */
class Property {
public:
    Property() = default;
    
    /**
     * @brief Construct property with metadata
     */
    explicit Property(const PropertyMetadata& metadata);
    
    /**
     * @brief Construct property with metadata and initial value
     */
    Property(const PropertyMetadata& metadata, const PropertyValue& value);
    
    // Copy/move
    Property(const Property& other);
    Property(Property&& other) noexcept;
    Property& operator=(const Property& other);
    Property& operator=(Property&& other) noexcept;
    
    /**
     * @brief Get property value
     */
    [[nodiscard]] const PropertyValue& getValue() const noexcept;
    
    /**
     * @brief Set property value
     * @return true if value was changed, false if read-only or same value
     */
    bool setValue(const PropertyValue& value);
    
    /**
     * @brief Get property metadata
     */
    [[nodiscard]] const PropertyMetadata& getMetadata() const noexcept;
    
    /**
     * @brief Get property name
     */
    [[nodiscard]] const std::string& getName() const noexcept;
    
    /**
     * @brief Get property type
     */
    [[nodiscard]] PropertyType getType() const noexcept;
    
    /**
     * @brief Check if property is read-only
     */
    [[nodiscard]] bool isReadOnly() const noexcept;
    
    /**
     * @brief Check if property is transient (not serialized)
     */
    [[nodiscard]] bool isTransient() const noexcept;
    
    /**
     * @brief Reset to default value
     */
    void resetToDefault();
    
    /**
     * @brief Set change callback
     */
    void setChangeCallback(PropertyChangeCallback callback);
    
private:
    PropertyMetadata m_metadata;
    PropertyValue m_value;
    PropertyChangeCallback m_callback;
    mutable std::mutex m_mutex;
};

// ============================================================================
// Property Registry
// ============================================================================

/**
 * @brief Registry for managing multiple properties
 */
class PropertyRegistry {
public:
    PropertyRegistry() = default;
    ~PropertyRegistry() = default;
    
    // Delete copy/move (registry should be unique)
    PropertyRegistry(const PropertyRegistry&) = delete;
    PropertyRegistry& operator=(const PropertyRegistry&) = delete;
    PropertyRegistry(PropertyRegistry&&) = delete;
    PropertyRegistry& operator=(PropertyRegistry&&) = delete;
    
    /**
     * @brief Register a property
     * @return true if registered, false if name already exists
     */
    bool registerProperty(const std::string& name, const PropertyMetadata& metadata);
    
    /**
     * @brief Register a property with initial value
     * @return true if registered, false if name already exists
     */
    bool registerProperty(const std::string& name, const PropertyMetadata& metadata, const PropertyValue& value);
    
    /**
     * @brief Unregister a property
     * @return true if unregistered, false if not found
     */
    bool unregisterProperty(const std::string& name);
    
    /**
     * @brief Get property by name
     * @return Property pointer or nullptr if not found
     */
    [[nodiscard]] Property* getProperty(const std::string& name);
    [[nodiscard]] const Property* getProperty(const std::string& name) const;
    
    /**
     * @brief Get property value
     * @return true if found and value retrieved
     */
    bool getValue(const std::string& name, PropertyValue& outValue) const;
    
    /**
     * @brief Set property value
     * @return true if found and set successfully
     */
    bool setValue(const std::string& name, const PropertyValue& value);
    
    /**
     * @brief Get all property names
     */
    [[nodiscard]] std::vector<std::string> getPropertyNames() const;
    
    /**
     * @brief Get properties by category
     */
    [[nodiscard]] std::vector<std::string> getPropertiesByCategory(const std::string& category) const;
    
    /**
     * @brief Get property count
     */
    [[nodiscard]] size_t getPropertyCount() const noexcept;
    
    /**
     * @brief Check if property exists
     */
    [[nodiscard]] bool hasProperty(const std::string& name) const;
    
    /**
     * @brief Reset all properties to default values
     */
    void resetAllToDefault();
    
    /**
     * @brief Clear all properties
     */
    void clear();
    
private:
    std::unordered_map<std::string, std::unique_ptr<Property>> m_properties;
    mutable std::mutex m_mutex;
};

// ============================================================================
// Global Registry Interface
// ============================================================================

/**
 * @brief Get the global property registry
 * Creates the registry on first call
 */
PropertyRegistry& getGlobalPropertyRegistry();

/**
 * @brief Destroy the global property registry
 * Called during engine shutdown
 */
void destroyGlobalPropertyRegistry();

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * @brief Convert property type to string
 */
[[nodiscard]] const char* propertyTypeToString(PropertyType type) noexcept;

/**
 * @brief Convert string to property type
 */
[[nodiscard]] PropertyType stringToPropertyType(const std::string& str) noexcept;

/**
 * @brief Get default value for property type
 */
[[nodiscard]] PropertyValue getDefaultValueForType(PropertyType type);

} // namespace properties
} // namespace core
} // namespace poko

#endif // POKO_CORE_PROPERTIES_PROPERTY_H
