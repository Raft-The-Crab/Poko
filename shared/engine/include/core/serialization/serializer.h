/**
 * @file serializer.h
 * @brief Core serialization system header
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_SERIALIZATION_SERIALIZER_H
#define POKO_CORE_SERIALIZATION_SERIALIZER_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace poko {
namespace core {
namespace serialization {

// ============================================================================
// Type Definitions
// ============================================================================

/**
 * @brief Serialize mode (read or write)
 */
enum class SerializeMode : uint8_t {
    Read = 0,    ///< Reading from stream
    Write = 1     ///< Writing to stream
};

// ============================================================================
// Serializer Interface
// ============================================================================

/**
 * @brief Base serializer interface
 * 
 * Provides a unified API for reading and writing data to streams.
 * Supports both text and binary serialization formats.
 */
class ISerializer {
public:
    virtual ~ISerializer() = default;
    
    /**
     * @brief Get current serialization mode
     * @return Current mode (Read or Write)
     */
    [[nodiscard]] virtual SerializeMode getMode() const noexcept = 0;
    
    /**
     * @brief Check if in read mode
     * @return True if reading
     */
    [[nodiscard]] virtual bool isReading() const noexcept = 0;
    
    /**
     * @brief Check if in write mode
     * @return True if writing
     */
    [[nodiscard]] virtual bool isWriting() const noexcept = 0;
    
    // Primitive types
    virtual bool serialize(bool& value) = 0;
    virtual bool serialize(int8_t& value) = 0;
    virtual bool serialize(int16_t& value) = 0;
    virtual bool serialize(int32_t& value) = 0;
    virtual bool serialize(int64_t& value) = 0;
    virtual bool serialize(uint8_t& value) = 0;
    virtual bool serialize(uint16_t& value) = 0;
    virtual bool serialize(uint32_t& value) = 0;
    virtual bool serialize(uint64_t& value) = 0;
    virtual bool serialize(float& value) = 0;
    virtual bool serialize(double& value) = 0;
    
    // String types
    virtual bool serialize(std::string& value) = 0;
    
    // Container types
    template<typename T>
    bool serialize(std::vector<T>& value) {
        if (isWriting()) {
            uint32_t size = static_cast<uint32_t>(value.size());
            if (!serialize(size)) return false;
            for (auto& item : value) {
                if (!serialize(item)) return false;
            }
        } else {
            uint32_t size = 0;
            if (!serialize(size)) return false;
            value.resize(size);
            for (auto& item : value) {
                if (!serialize(item)) return false;
            }
        }
        return true;
    }
};

// ============================================================================
// Binary Memory Serializer
// ============================================================================

/**
 * @brief Binary serializer using memory buffer
 * 
 * Serializes data to/from a memory buffer in binary format.
 * Fast and efficient for in-memory serialization.
 */
class BinaryMemorySerializer : public ISerializer {
public:
    /**
     * @brief Constructor for write mode
     * @param initialCapacity Initial buffer capacity
     */
    explicit BinaryMemorySerializer(size_t initialCapacity = 1024);
    
    /**
     * @brief Constructor for read mode
     * @param data Pointer to data to read
     * @param size Size of data
     */
    BinaryMemorySerializer(const void* data, size_t size);
    
    /**
     * @brief Destructor
     */
    ~BinaryMemorySerializer() override;
    
    /**
     * @brief Copy constructor (deleted)
     */
    BinaryMemorySerializer(const BinaryMemorySerializer&) = delete;
    
    /**
     * @brief Copy assignment (deleted)
     */
    BinaryMemorySerializer& operator=(const BinaryMemorySerializer&) = delete;
    
    /**
     * @brief Move constructor
     */
    BinaryMemorySerializer(BinaryMemorySerializer&&) noexcept = default;
    
    /**
     * @brief Move assignment
     */
    BinaryMemorySerializer& operator=(BinaryMemorySerializer&&) noexcept = default;
    
    // ISerializer implementation
    [[nodiscard]] SerializeMode getMode() const noexcept override;
    [[nodiscard]] bool isReading() const noexcept override;
    [[nodiscard]] bool isWriting() const noexcept override;
    
    bool serialize(bool& value) override;
    bool serialize(int8_t& value) override;
    bool serialize(int16_t& value) override;
    bool serialize(int32_t& value) override;
    bool serialize(int64_t& value) override;
    bool serialize(uint8_t& value) override;
    bool serialize(uint16_t& value) override;
    bool serialize(uint32_t& value) override;
    bool serialize(uint64_t& value) override;
    bool serialize(float& value) override;
    bool serialize(double& value) override;
    bool serialize(std::string& value) override;
    
    /**
     * @brief Get the serialized data (write mode only)
     * @return Pointer to data buffer
     */
    [[nodiscard]] const uint8_t* getData() const noexcept;
    
    /**
     * @brief Get the data size
     * @return Size of data in bytes
     */
    [[nodiscard]] size_t getSize() const noexcept;
    
    /**
     * @brief Reset the serializer to beginning
     */
    void reset() noexcept;
    
private:
    SerializeMode m_mode;
    std::vector<uint8_t> m_buffer;
    size_t m_position;
    
    bool readBytes(void* data, size_t size);
    bool writeBytes(const void* data, size_t size);
};

// ============================================================================
// Global Serializer Access
// ============================================================================

/**
 * @brief Create a binary memory serializer for writing
 * @param initialCapacity Initial buffer capacity
 * @return Unique pointer to serializer
 */
[[nodiscard]] std::unique_ptr<BinaryMemorySerializer> createBinaryWriter(size_t initialCapacity = 1024);

/**
 * @brief Create a binary memory serializer for reading
 * @param data Pointer to data to read
 * @param size Size of data
 * @return Unique pointer to serializer
 */
[[nodiscard]] std::unique_ptr<BinaryMemorySerializer> createBinaryReader(const void* data, size_t size);

} // namespace serialization
} // namespace core
} // namespace poko

#endif // POKO_CORE_SERIALIZATION_SERIALIZER_H
