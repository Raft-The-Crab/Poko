/**
 * @file pnv.cpp
 * @brief Implementation of the PNV (Poko Numeric Version) system
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "pnv/pnv.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <mutex>
#include <iostream>

namespace poko {
namespace pnv {

// PNVManager implementation
PNVManager::PNVManager() {
    // Initialize with default values
    state_.engine_max = 0;
    state_.mute_max = 0;
    state_.studio_max = 0;
    state_.pokox_max = 0;
    state_.authority_max = 0;
    state_.pkx_format_max = 0;
    state_.game_max = 0;
    state_.package_max = 0;
    state_.plugin_max = 0;
    state_.tooling_max = 0;
}

PNVManager& PNVManager::instance() {
    static PNVManager instance;
    return instance;
}

PNV PNVManager::allocate(PNVType type) {
    std::lock_guard<std::mutex> lock(mutex_);

    PNV new_pnv = 0;
    switch (type) {
        case PNVType::ENGINE:
            new_pnv = ++state_.engine_max;
            break;
        case PNVType::MUTE:
            new_pnv = ++state_.mute_max;
            break;
        case PNVType::STUDIO:
            new_pnv = ++state_.studio_max;
            break;
        case PNVType::POKOX:
            new_pnv = ++state_.pokox_max;
            break;
        case PNVType::AUTHORITY:
            new_pnv = ++state_.authority_max;
            break;
        case PNVType::PKX_FORMAT:
            new_pnv = ++state_.pkx_format_max;
            break;
        case PNVType::GAME:
            new_pnv = ++state_.game_max;
            break;
        case PNVType::PACKAGE:
            new_pnv = ++state_.package_max;
            break;
        case PNVType::PLUGIN:
            new_pnv = ++state_.plugin_max;
            break;
        case PNVType::TOOLING:
            new_pnv = ++state_.tooling_max;
            break;
    }

    allocated_pnvs_.push_back(new_pnv);
    return new_pnv;
}

bool PNVManager::allocate_specific(PNVType type, PNV pnv) {
    if (is_reserved(pnv)) {
        return false;
    }

    // Check if PNV is already allocated
    if (std::find(allocated_pnvs_.begin(), allocated_pnvs_.end(), pnv) != allocated_pnvs_.end()) {
        return false;
    }

    // Update the max for the type if necessary
    switch (type) {
        case PNVType::ENGINE:
            if (pnv > state_.engine_max) state_.engine_max = pnv;
            break;
        case PNVType::MUTE:
            if (pnv > state_.mute_max) state_.mute_max = pnv;
            break;
        case PNVType::STUDIO:
            if (pnv > state_.studio_max) state_.studio_max = pnv;
            break;
        case PNVType::POKOX:
            if (pnv > state_.pokox_max) state_.pokox_max = pnv;
            break;
        case PNVType::AUTHORITY:
            if (pnv > state_.authority_max) state_.authority_max = pnv;
            break;
        case PNVType::PKX_FORMAT:
            if (pnv > state_.pkx_format_max) state_.pkx_format_max = pnv;
            break;
        case PNVType::GAME:
            if (pnv > state_.game_max) state_.game_max = pnv;
            break;
        case PNVType::PACKAGE:
            if (pnv > state_.package_max) state_.package_max = pnv;
            break;
        case PNVType::PLUGIN:
            if (pnv > state_.plugin_max) state_.plugin_max = pnv;
            break;
        case PNVType::TOOLING:
            if (pnv > state_.tooling_max) state_.tooling_max = pnv;
            break;
    }

    allocated_pnvs_.push_back(pnv);
    return true;
}

bool PNVManager::validate(PNVType type, PNV pnv) const {
    if (is_reserved(pnv)) {
        return false;
    }

    PNV max = get_max(type);
    return pnv <= max && pnv > 0;
}

PNV PNVManager::get_max(PNVType type) const {
    switch (type) {
        case PNVType::ENGINE: return state_.engine_max;
        case PNVType::MUTE: return state_.mute_max;
        case PNVType::STUDIO: return state_.studio_max;
        case PNVType::POKOX: return state_.pokox_max;
        case PNVType::AUTHORITY: return state_.authority_max;
        case PNVType::PKX_FORMAT: return state_.pkx_format_max;
        case PNVType::GAME: return state_.game_max;
        case PNVType::PACKAGE: return state_.package_max;
        case PNVType::PLUGIN: return state_.plugin_max;
        case PNVType::TOOLING: return state_.tooling_max;
        default: return 0;
    }
}

std::string PNVManager::to_string(PNV pnv) {
    return std::to_string(pnv);
}

std::optional<PNV> PNVManager::from_string(const std::string& str) {
    try {
        PNV pnv = std::stoull(str);
        if (pnv == 0) {
            return std::nullopt;
        }
        return pnv;
    } catch (...) {
        return std::nullopt;
    }
}

std::string PNVManager::type_to_string(PNVType type) {
    switch (type) {
        case PNVType::ENGINE: return "ENGINE";
        case PNVType::MUTE: return "MUTE";
        case PNVType::STUDIO: return "STUDIO";
        case PNVType::POKOX: return "POKOX";
        case PNVType::AUTHORITY: return "AUTHORITY";
        case PNVType::PKX_FORMAT: return "PKX_FORMAT";
        case PNVType::GAME: return "GAME";
        case PNVType::PACKAGE: return "PACKAGE";
        case PNVType::PLUGIN: return "PLUGIN";
        case PNVType::TOOLING: return "TOOLING";
        default: return "UNKNOWN";
    }
}

std::optional<PNVType> PNVManager::type_from_string(const std::string& str) {
    if (str == "ENGINE") return PNVType::ENGINE;
    if (str == "MUTE") return PNVType::MUTE;
    if (str == "STUDIO") return PNVType::STUDIO;
    if (str == "POKOX") return PNVType::POKOX;
    if (str == "AUTHORITY") return PNVType::AUTHORITY;
    if (str == "PKX_FORMAT") return PNVType::PKX_FORMAT;
    if (str == "GAME") return PNVType::GAME;
    if (str == "PACKAGE") return PNVType::PACKAGE;
    if (str == "PLUGIN") return PNVType::PLUGIN;
    if (str == "TOOLING") return PNVType::TOOLING;
    return std::nullopt;
}

bool PNVManager::is_reserved(PNV pnv) {
    // Reserve PNV 0 and some ranges for special purposes
    return pnv == 0 || (pnv >= 1000 && pnv <= 1999);
}

bool PNVManager::load_state(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    try {
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string type_str;
            PNV value;

            if (iss >> type_str >> value) {
                auto type = type_from_string(type_str);
                if (type) {
                    allocate_specific(*type, value);
                }
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool PNVManager::save_state(const std::string& path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    try {
        file << type_to_string(PNVType::ENGINE) << " " << state_.engine_max << "\n";
        file << type_to_string(PNVType::MUTE) << " " << state_.mute_max << "\n";
        file << type_to_string(PNVType::STUDIO) << " " << state_.studio_max << "\n";
        file << type_to_string(PNVType::POKOX) << " " << state_.pokox_max << "\n";
        file << type_to_string(PNVType::AUTHORITY) << " " << state_.authority_max << "\n";
        file << type_to_string(PNVType::PKX_FORMAT) << " " << state_.pkx_format_max << "\n";
        file << type_to_string(PNVType::GAME) << " " << state_.game_max << "\n";
        file << type_to_string(PNVType::PACKAGE) << " " << state_.package_max << "\n";
        file << type_to_string(PNVType::PLUGIN) << " " << state_.plugin_max << "\n";
        file << type_to_string(PNVType::TOOLING) << " " << state_.tooling_max << "\n";
        return true;
    } catch (...) {
        return false;
    }
}

// PNVRange implementation
PNVRange::PNVRange(PNV min, PNV max) : min_(min), max_(max) {
    if (min_ > max_) {
        std::swap(min_, max_);
    }
}

bool PNVRange::contains(PNV pnv) const {
    return pnv >= min_ && pnv <= max_;
}

std::string PNVRange::to_string() const {
    return PNVManager::to_string(min_) + "-" + PNVManager::to_string(max_);
}

std::optional<PNVRange> PNVRange::from_string(const std::string& str) {
    size_t dash_pos = str.find('-');
    if (dash_pos == std::string::npos) {
        return std::nullopt;
    }

    auto min_str = str.substr(0, dash_pos);
    auto max_str = str.substr(dash_pos + 1);

    auto min = PNVManager::from_string(min_str);
    auto max = PNVManager::from_string(max_str);

    if (min && max) {
        return PNVRange(*min, *max);
    }
    return std::nullopt;
}

// PNVCompatibility implementation
bool PNVCompatibility::is_compatible(PNV component_pnv, PNV required_pnv,
                                     const std::string& requirement_type) {
    if (requirement_type == "exact") {
        return component_pnv == required_pnv;
    } else if (requirement_type == "minimum") {
        return component_pnv >= required_pnv;
    } else if (requirement_type == "maximum") {
        return component_pnv <= required_pnv;
    }
    // Default to exact match
    return component_pnv == required_pnv;
}

bool PNVCompatibility::is_compatible(PNV component_pnv, const PNVRange& range) {
    return range.contains(component_pnv);
}

PNV PNVCompatibility::get_minimum_supported(PNVType type) {
    (void)type; // Suppress unused parameter warning
    // TODO: Implement minimum supported tracking
    return 1;
}

void PNVCompatibility::set_minimum_supported(PNVType type, PNV pnv) {
    (void)type; // Suppress unused parameter warning
    (void)pnv; // Suppress unused parameter warning
    // TODO: Implement minimum supported tracking
}

} // namespace pnv
} // namespace poko
