/**
 * @file metadata.cpp
 * @brief Property metadata implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/properties/property.h"

namespace poko {
namespace core {
namespace properties {

const char* propertyTypeToString(PropertyType type) noexcept {
    switch (type) {
        case PropertyType::Bool: return "bool";
        case PropertyType::Int32: return "int32";
        case PropertyType::Int64: return "int64";
        case PropertyType::Float: return "float";
        case PropertyType::Double: return "double";
        case PropertyType::String: return "string";
        case PropertyType::Vector2: return "vector2";
        case PropertyType::Vector3: return "vector3";
        case PropertyType::Vector4: return "vector4";
        case PropertyType::Color: return "color";
        default: return "invalid";
    }
}

PropertyType stringToPropertyType(const std::string& str) noexcept {
    if (str == "bool") return PropertyType::Bool;
    if (str == "int32") return PropertyType::Int32;
    if (str == "int64") return PropertyType::Int64;
    if (str == "float") return PropertyType::Float;
    if (str == "double") return PropertyType::Double;
    if (str == "string") return PropertyType::String;
    if (str == "vector2") return PropertyType::Vector2;
    if (str == "vector3") return PropertyType::Vector3;
    if (str == "vector4") return PropertyType::Vector4;
    if (str == "color") return PropertyType::Color;
    return PropertyType::Invalid;
}

PropertyValue getDefaultValueForType(PropertyType type) {
    switch (type) {
        case PropertyType::Bool: return false;
        case PropertyType::Int32: return int32_t(0);
        case PropertyType::Int64: return int64_t(0);
        case PropertyType::Float: return 0.0f;
        case PropertyType::Double: return 0.0;
        case PropertyType::String: return std::string();
        case PropertyType::Vector2: return std::vector<float>{0.0f, 0.0f};
        case PropertyType::Vector3: return std::vector<float>{0.0f, 0.0f, 0.0f};
        case PropertyType::Vector4: return std::vector<float>{0.0f, 0.0f, 0.0f, 0.0f};
        case PropertyType::Color: return std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f};
        default: return int32_t(0);
    }
}

} // namespace properties
} // namespace core
} // namespace poko
