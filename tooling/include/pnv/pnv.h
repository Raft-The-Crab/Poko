/**
 * @file pnv.h
 * @brief PNV (Poko Numeric Version) system for version management
 * @details Provides numeric version identity for Poko components, games, packages, etc.
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#pragma once

#include <string>
#include <cstdint>
#include <optional>
#include <vector>
#include <mutex>

namespace poko {
namespace pnv {

/**
 * @typedef PNV
 * @brief Numeric version identifier (64-bit unsigned integer)
 * @details PNV is monotonic, unique, and sortable. Not a hash or semantic version.
 */
using PNV = uint64_t;

/**
 * @enum PNVType
 * @brief Types of entities that can have PNV identifiers
 */
enum class PNVType {
    ENGINE,           ///< Poko Engine version
    MUTE,             ///< Mute language version
    STUDIO,           ///< Poko Studio version
    POKOX,            ///< PokoX version
    AUTHORITY,        ///< Game Authority version
    PKX_FORMAT,       ///< PKX package format version
    GAME,             ///< Published game version
    PACKAGE,          ///< Package/library version
    PLUGIN,           ///< Plugin version
    TOOLING           ///< Build tooling version
};

/**
 * @class PNVManager
 * @brief Manages PNV allocation and validation
 * @details Ensures PNV values are unique, monotonic, and properly allocated
 */
class PNVManager {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the PNV manager
     */
    static PNVManager& instance();

    /**
     * @brief Allocate a new PNV for a specific type
     * @param type The type of entity being versioned
     * @return Newly allocated PNV
     */
    PNV allocate(PNVType type);

    /**
     * @brief Allocate a specific PNV (for migration or known values)
     * @param type The type of entity
     * @param pnv The specific PNV to allocate
     * @return true if allocation succeeded, false if PNV already in use
     */
    bool allocate_specific(PNVType type, PNV pnv);

    /**
     * @brief Validate a PNV for a specific type
     * @param type The type of entity
     * @param pnv The PNV to validate
     * @return true if PNV is valid for the type
     */
    bool validate(PNVType type, PNV pnv) const;

    /**
     * @brief Get the current maximum PNV for a type
     * @param type The type of entity
     * @return Current maximum PNV for the type
     */
    PNV get_max(PNVType type) const;

    /**
     * @brief Convert PNV to string representation
     * @param pnv The PNV to convert
     * @return String representation (e.g., "12345")
     */
    static std::string to_string(PNV pnv);

    /**
     * @brief Parse string to PNV
     * @param str String representation
     * @return Parsed PNV, or nullopt if invalid
     */
    static std::optional<PNV> from_string(const std::string& str);

    /**
     * @brief Get PNV type string
     * @param type The PNV type
     * @return String representation of the type
     */
    static std::string type_to_string(PNVType type);

    /**
     * @brief Parse PNV type from string
     * @param str String representation
     * @return Parsed type, or nullopt if invalid
     */
    static std::optional<PNVType> type_from_string(const std::string& str);

    /**
     * @brief Check if PNV is reserved for special purposes
     * @param pnv The PNV to check
     * @return true if PNV is reserved
     */
    static bool is_reserved(PNV pnv);

    /**
     * @brief Load PNV allocation state from storage
     * @param path Path to the storage file
     * @return true if load succeeded
     */
    bool load_state(const std::string& path);

    /**
     * @brief Save PNV allocation state to storage
     * @param path Path to the storage file
     * @return true if save succeeded
     */
    bool save_state(const std::string& path);

private:
    /**
     * @brief Private constructor for singleton
     */
    PNVManager();

    /**
     * @brief Private destructor
     */
    ~PNVManager() = default;

    /**
     * @brief Delete copy constructor
     */
    PNVManager(const PNVManager&) = delete;

    /**
     * @brief Delete assignment operator
     */
    PNVManager& operator=(const PNVManager&) = delete;

    mutable std::mutex mutex_;           ///< Mutex for thread-safe operations

    struct AllocationState {
        PNV engine_max = 0;
        PNV mute_max = 0;
        PNV studio_max = 0;
        PNV pokox_max = 0;
        PNV authority_max = 0;
        PNV pkx_format_max = 0;
        PNV game_max = 0;
        PNV package_max = 0;
        PNV plugin_max = 0;
        PNV tooling_max = 0;
    };

    AllocationState state_;
    std::vector<PNV> allocated_pnvs_;
};

/**
 * @class PNVRange
 * @brief Represents a range of PNV values for compatibility checking
 */
class PNVRange {
public:
    /**
     * @brief Construct a PNV range
     * @param min Minimum PNV (inclusive)
     * @param max Maximum PNV (inclusive)
     */
    PNVRange(PNV min, PNV max);

    /**
     * @brief Check if a PNV is within the range
     * @param pnv The PNV to check
     * @return true if PNV is within range
     */
    bool contains(PNV pnv) const;

    /**
     * @brief Get the minimum PNV
     * @return Minimum PNV
     */
    PNV min() const { return min_; }

    /**
     * @brief Get the maximum PNV
     * @return Maximum PNV
     */
    PNV max() const { return max_; }

    /**
     * @brief Convert range to string representation
     * @return String representation (e.g., "100-200")
     */
    std::string to_string() const;

    /**
     * @brief Parse range from string
     * @param str String representation
     * @return Parsed range, or nullopt if invalid
     */
    static std::optional<PNVRange> from_string(const std::string& str);

private:
    PNV min_;
    PNV max_;
};

/**
 * @class PNVCompatibility
 * @brief Handles PNV compatibility checking between components
 */
class PNVCompatibility {
public:
    /**
     * @brief Check if a component PNV is compatible with a requirement
     * @param component_pnv The component's PNV
     * @param required_pnv The required PNV
     * @param requirement_type Type of requirement (exact, minimum, range)
     * @return true if compatible
     */
    static bool is_compatible(PNV component_pnv, PNV required_pnv,
                             const std::string& requirement_type = "exact");

    /**
     * @brief Check if a component PNV is compatible with a range
     * @param component_pnv The component's PNV
     * @param range The required PNV range
     * @return true if compatible
     */
    static bool is_compatible(PNV component_pnv, const PNVRange& range);

    /**
     * @brief Get the minimum supported PNV for a type
     * @param type The PNV type
     * @return Minimum supported PNV
     */
    static PNV get_minimum_supported(PNVType type);

    /**
     * @brief Set the minimum supported PNV for a type
     * @param type The PNV type
     * @param pnv The minimum supported PNV
     */
    static void set_minimum_supported(PNVType type, PNV pnv);
};

// Convenience macros for PNV operations
#define POKO_PNV_ALLOCATE(type) ::poko::pnv::PNVManager::instance().allocate(type)
#define POKO_PNV_VALIDATE(type, pnv) ::poko::pnv::PNVManager::instance().validate(type, pnv)
#define POKO_PNV_TO_STRING(pnv) ::poko::pnv::PNVManager::to_string(pnv)
#define POKO_PNV_FROM_STRING(str) ::poko::pnv::PNVManager::from_string(str)

} // namespace pnv
} // namespace poko
