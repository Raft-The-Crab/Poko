/**
 * @file serializer.h
 * @brief Core serialization primitives for Poko Engine
 * @details Provides binary serialization utilities for data persistence and network transmission
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <memory>
#include <type_traits>

namespace poko {
namespace core {

/**
 * @class BinaryWriter
 * @brief Binary data writer for serialization
 * @details Provides efficient binary serialization with endianness handling
 */
class BinaryWriter {
public:
    /**
     * @brief Construct a binary writer
     * @param initial_capacity Initial buffer capacity
     */
    explicit BinaryWriter(size_t initial_capacity = 1024);

    /**
     * @brief Get the current buffer
     * @return Reference to the buffer
     */
    const std::vector<uint8_t>& buffer() const { return buffer_; }

    /**
     * @brief Get the current buffer (mutable)
     * @return Reference to the buffer
     */
    std::vector<uint8_t>& buffer() { return buffer_; }

    /**
     * @brief Get the current write position
     * @return Current position in bytes
     */
    size_t position() const { return position_; }

    /**
     * @brief Set the write position
     * @param pos New position
     */
    void set_position(size_t pos);

    /**
     * @brief Clear the buffer
     */
    void clear();

    /**
     * @brief Reserve capacity in the buffer
     * @param capacity Minimum capacity to reserve
     */
    void reserve(size_t capacity);

    // Write primitive types
    void write_uint8(uint8_t value);
    void write_uint16(uint16_t value);
    void write_uint32(uint32_t value);
    void write_uint64(uint64_t value);
    void write_int8(int8_t value);
    void write_int16(int16_t value);
    void write_int32(int32_t value);
    void write_int64(int64_t value);
    void write_float(float value);
    void write_double(double value);
    void write_bool(bool value);

    /**
     * @brief Write raw bytes
     * @param data Pointer to data
     * @param size Number of bytes to write
     */
    void write_bytes(const void* data, size_t size);

    /**
     * @brief Write a string
     * @param str String to write
     */
    void write_string(const std::string& str);

    /**
     * @brief Write a variable-length integer
     * @param value Integer to write
     */
    void write_varint(uint64_t value);

private:
    std::vector<uint8_t> buffer_;
    size_t position_;

    void ensure_capacity(size_t additional);
};

/**
 * @class BinaryReader
 * @brief Binary data reader for deserialization
 * @details Provides efficient binary deserialization with endianness handling
 */
class BinaryReader {
public:
    /**
     * @brief Construct a binary reader from buffer
     * @param data Pointer to data
     * @param size Size of data
     */
    BinaryReader(const void* data, size_t size);

    /**
     * @brief Construct a binary reader from vector
     * @param data Data vector
     */
    explicit BinaryReader(const std::vector<uint8_t>& data);

    /**
     * @brief Get the current read position
     * @return Current position in bytes
     */
    size_t position() const { return position_; }

    /**
     * @brief Get the total size
     * @return Total size in bytes
     */
    size_t size() const { return size_; }

    /**
     * @brief Check if we've reached the end
     * @return true if at end of data
     */
    bool eof() const { return position_ >= size_; }

    /**
     * @brief Set the read position
     * @param pos New position
     */
    void set_position(size_t pos);

    /**
     * @brief Read primitive types
     * @return The read value
     */
    uint8_t read_uint8();
    uint16_t read_uint16();
    uint32_t read_uint32();
    uint64_t read_uint64();
    int8_t read_int8();
    int16_t read_int16();
    int32_t read_int32();
    int64_t read_int64();
    float read_float();
    double read_double();
    bool read_bool();

    /**
     * @brief Read raw bytes
     * @param data Pointer to destination
     * @param size Number of bytes to read
     * @return true if read succeeded
     */
    bool read_bytes(void* data, size_t size);

    /**
     * @brief Read a string
     * @return The read string
     */
    std::string read_string();

    /**
     * @brief Read a variable-length integer
     * @return The read value
     */
    uint64_t read_varint();

private:
    const uint8_t* data_;
    size_t size_;
    size_t position_;

    bool check_available(size_t bytes) const;
};

/**
 * @class JsonSerializer
 * @brief JSON serialization for human-readable data
 * @details Provides JSON serialization for configuration and debugging
 */
class JsonSerializer {
public:
    /**
     * @brief Construct a JSON serializer
     */
    JsonSerializer();

    /**
     * @brief Get the current JSON string
     * @return Current JSON string
     */
    const std::string& str() const { return json_; }

    /**
     * @brief Clear the JSON string
     */
    void clear();

    /**
     * @brief Write a key-value pair
     * @param key Key name
     * @param value Value
     */
    void write_string(const std::string& key, const std::string& value);
    void write_int(const std::string& key, int64_t value);
    void write_uint(const std::string& key, uint64_t value);
    void write_float(const std::string& key, double value);
    void write_bool(const std::string& key, bool value);
    void write_null(const std::string& key);

    /**
     * @brief Start an object
     * @param key Key name (empty for root object)
     */
    void start_object(const std::string& key = "");

    /**
     * @brief End the current object
     */
    void end_object();

    /**
     * @brief Start an array
     * @param key Key name
     */
    void start_array(const std::string& key);

    /**
     * @brief End the current array
     */
    void end_array();

private:
    std::string json_;
    int indent_level_;
    bool need_comma_;

    void write_indent();
    void write_comma();
};

/**
 * @class JsonDeserializer
 * @brief JSON deserialization
 * @details Provides JSON parsing for configuration and debugging
 */
class JsonDeserializer {
public:
    /**
     * @brief Construct a JSON deserializer
     * @param json JSON string to parse
     */
    explicit JsonDeserializer(const std::string& json);

    /**
     * @brief Check if parsing succeeded
     * @return true if parsing succeeded
     */
    bool is_valid() const { return valid_; }

    /**
     * @brief Get the error message
     * @return Error message if parsing failed
     */
    const std::string& error() const { return error_; }

    /**
     * @brief Read a string value
     * @param key Key name
     * @param value Output value
     * @return true if read succeeded
     */
    bool read_string(const std::string& key, std::string& value);

    /**
     * @brief Read an integer value
     * @param key Key name
     * @param value Output value
     * @return true if read succeeded
     */
    bool read_int(const std::string& key, int64_t& value);

    /**
     * @brief Read an unsigned integer value
     * @param key Key name
     * @param value Output value
     * @return true if read succeeded
     */
    bool read_uint(const std::string& key, uint64_t& value);

    /**
     * @brief Read a float value
     * @param key Key name
     * @param value Output value
     * @return true if read succeeded
     */
    bool read_float(const std::string& key, double& value);

    /**
     * @brief Read a boolean value
     * @param key Key name
     * @param value Output value
     * @return true if read succeeded
     */
    bool read_bool(const std::string& key, bool& value);

private:
    std::string json_;
    bool valid_;
    std::string error_;

    // TODO: Implement proper JSON parsing
    // For now, this is a placeholder
};

} // namespace core
} // namespace poko
