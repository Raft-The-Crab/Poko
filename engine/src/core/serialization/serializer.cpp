/**
 * @file serializer.cpp
 * @brief Implementation of core serialization primitives
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "core/serialization/serializer.h"
#include <cstring>
#include <algorithm>

namespace poko {
namespace core {

// BinaryWriter implementation
BinaryWriter::BinaryWriter(size_t initial_capacity)
    : buffer_(initial_capacity), position_(0) {}

void BinaryWriter::set_position(size_t pos) {
    if (pos > buffer_.size()) {
        ensure_capacity(pos - buffer_.size());
    }
    position_ = pos;
}

void BinaryWriter::clear() {
    buffer_.clear();
    position_ = 0;
}

void BinaryWriter::reserve(size_t capacity) {
    buffer_.reserve(capacity);
}

void BinaryWriter::ensure_capacity(size_t additional) {
    if (position_ + additional > buffer_.size()) {
        buffer_.resize(position_ + additional);
    }
}

void BinaryWriter::write_uint8(uint8_t value) {
    ensure_capacity(1);
    buffer_[position_++] = value;
}

void BinaryWriter::write_uint16(uint16_t value) {
    ensure_capacity(2);
    // Write little-endian
    buffer_[position_++] = value & 0xFF;
    buffer_[position_++] = (value >> 8) & 0xFF;
}

void BinaryWriter::write_uint32(uint32_t value) {
    ensure_capacity(4);
    // Write little-endian
    buffer_[position_++] = value & 0xFF;
    buffer_[position_++] = (value >> 8) & 0xFF;
    buffer_[position_++] = (value >> 16) & 0xFF;
    buffer_[position_++] = (value >> 24) & 0xFF;
}

void BinaryWriter::write_uint64(uint64_t value) {
    ensure_capacity(8);
    // Write little-endian
    buffer_[position_++] = value & 0xFF;
    buffer_[position_++] = (value >> 8) & 0xFF;
    buffer_[position_++] = (value >> 16) & 0xFF;
    buffer_[position_++] = (value >> 24) & 0xFF;
    buffer_[position_++] = (value >> 32) & 0xFF;
    buffer_[position_++] = (value >> 40) & 0xFF;
    buffer_[position_++] = (value >> 48) & 0xFF;
    buffer_[position_++] = (value >> 56) & 0xFF;
}

void BinaryWriter::write_int8(int8_t value) {
    write_uint8(static_cast<uint8_t>(value));
}

void BinaryWriter::write_int16(int16_t value) {
    write_uint16(static_cast<uint16_t>(value));
}

void BinaryWriter::write_int32(int32_t value) {
    write_uint32(static_cast<uint32_t>(value));
}

void BinaryWriter::write_int64(int64_t value) {
    write_uint64(static_cast<uint64_t>(value));
}

void BinaryWriter::write_float(float value) {
    static_assert(sizeof(float) == 4, "Float must be 4 bytes");
    uint32_t value_bits;
    std::memcpy(&value_bits, &value, sizeof(float));
    write_uint32(value_bits);
}

void BinaryWriter::write_double(double value) {
    static_assert(sizeof(double) == 8, "Double must be 8 bytes");
    uint64_t value_bits;
    std::memcpy(&value_bits, &value, sizeof(double));
    write_uint64(value_bits);
}

void BinaryWriter::write_bool(bool value) {
    write_uint8(value ? 1 : 0);
}

void BinaryWriter::write_bytes(const void* data, size_t size) {
    ensure_capacity(size);
    std::memcpy(buffer_.data() + position_, data, size);
    position_ += size;
}

void BinaryWriter::write_string(const std::string& str) {
    write_uint32(static_cast<uint32_t>(str.size()));
    write_bytes(str.data(), str.size());
}

void BinaryWriter::write_varint(uint64_t value) {
    // Variable-length integer encoding (similar to Protocol Buffers)
    while (value >= 0x80) {
        write_uint8(static_cast<uint8_t>((value & 0x7F) | 0x80));
        value >>= 7;
    }
    write_uint8(static_cast<uint8_t>(value));
}

// BinaryReader implementation
BinaryReader::BinaryReader(const void* data, size_t size)
    : data_(static_cast<const uint8_t*>(data)), size_(size), position_(0) {}

BinaryReader::BinaryReader(const std::vector<uint8_t>& data)
    : data_(data.data()), size_(data.size()), position_(0) {}

void BinaryReader::set_position(size_t pos) {
    if (pos > size_) {
        position_ = size_;
    } else {
        position_ = pos;
    }
}

bool BinaryReader::check_available(size_t bytes) const {
    return position_ + bytes <= size_;
}

uint8_t BinaryReader::read_uint8() {
    if (!check_available(1)) return 0;
    return data_[position_++];
}

uint16_t BinaryReader::read_uint16() {
    if (!check_available(2)) return 0;
    uint16_t value = data_[position_];
    value |= static_cast<uint16_t>(data_[position_ + 1]) << 8;
    position_ += 2;
    return value;
}

uint32_t BinaryReader::read_uint32() {
    if (!check_available(4)) return 0;
    uint32_t value = data_[position_];
    value |= static_cast<uint32_t>(data_[position_ + 1]) << 8;
    value |= static_cast<uint32_t>(data_[position_ + 2]) << 16;
    value |= static_cast<uint32_t>(data_[position_ + 3]) << 24;
    position_ += 4;
    return value;
}

uint64_t BinaryReader::read_uint64() {
    if (!check_available(8)) return 0;
    uint64_t value = data_[position_];
    value |= static_cast<uint64_t>(data_[position_ + 1]) << 8;
    value |= static_cast<uint64_t>(data_[position_ + 2]) << 16;
    value |= static_cast<uint64_t>(data_[position_ + 3]) << 24;
    value |= static_cast<uint64_t>(data_[position_ + 4]) << 32;
    value |= static_cast<uint64_t>(data_[position_ + 5]) << 40;
    value |= static_cast<uint64_t>(data_[position_ + 6]) << 48;
    value |= static_cast<uint64_t>(data_[position_ + 7]) << 56;
    position_ += 8;
    return value;
}

int8_t BinaryReader::read_int8() {
    return static_cast<int8_t>(read_uint8());
}

int16_t BinaryReader::read_int16() {
    return static_cast<int16_t>(read_uint16());
}

int32_t BinaryReader::read_int32() {
    return static_cast<int32_t>(read_uint32());
}

int64_t BinaryReader::read_int64() {
    return static_cast<int64_t>(read_uint64());
}

float BinaryReader::read_float() {
    uint32_t value_bits = read_uint32();
    float value;
    std::memcpy(&value, &value_bits, sizeof(float));
    return value;
}

double BinaryReader::read_double() {
    uint64_t value_bits = read_uint64();
    double value;
    std::memcpy(&value, &value_bits, sizeof(double));
    return value;
}

bool BinaryReader::read_bool() {
    return read_uint8() != 0;
}

bool BinaryReader::read_bytes(void* data, size_t size) {
    if (!check_available(size)) return false;
    std::memcpy(data, data_ + position_, size);
    position_ += size;
    return true;
}

std::string BinaryReader::read_string() {
    uint32_t length = read_uint32();
    if (!check_available(length)) return "";
    std::string str(reinterpret_cast<const char*>(data_ + position_), length);
    position_ += length;
    return str;
}

uint64_t BinaryReader::read_varint() {
    uint64_t value = 0;
    int shift = 0;
    uint8_t byte;

    do {
        if (eof()) return value;
        byte = read_uint8();
        value |= static_cast<uint64_t>(byte & 0x7F) << shift;
        shift += 7;
    } while (byte & 0x80);

    return value;
}

// JsonSerializer implementation
JsonSerializer::JsonSerializer() : indent_level_(0), need_comma_(false) {}

void JsonSerializer::clear() {
    json_.clear();
    indent_level_ = 0;
    need_comma_ = false;
}

void JsonSerializer::write_indent() {
    for (int i = 0; i < indent_level_; ++i) {
        json_ += "  ";
    }
}

void JsonSerializer::write_comma() {
    if (need_comma_) {
        json_ += ",\n";
    } else {
        json_ += "\n";
    }
    need_comma_ = false;
}

void JsonSerializer::write_string(const std::string& key, const std::string& value) {
    write_comma();
    write_indent();
    json_ += "\"" + key + "\": \"" + value + "\"";
    need_comma_ = true;
}

void JsonSerializer::write_int(const std::string& key, int64_t value) {
    write_comma();
    write_indent();
    json_ += "\"" + key + "\": " + std::to_string(value);
    need_comma_ = true;
}

void JsonSerializer::write_uint(const std::string& key, uint64_t value) {
    write_comma();
    write_indent();
    json_ += "\"" + key + "\": " + std::to_string(value);
    need_comma_ = true;
}

void JsonSerializer::write_float(const std::string& key, double value) {
    write_comma();
    write_indent();
    json_ += "\"" + key + "\": " + std::to_string(value);
    need_comma_ = true;
}

void JsonSerializer::write_bool(const std::string& key, bool value) {
    write_comma();
    write_indent();
    json_ += "\"" + key + "\": " + (value ? "true" : "false");
    need_comma_ = true;
}

void JsonSerializer::write_null(const std::string& key) {
    write_comma();
    write_indent();
    json_ += "\"" + key + "\": null";
    need_comma_ = true;
}

void JsonSerializer::start_object(const std::string& key) {
    write_comma();
    write_indent();
    if (!key.empty()) {
        json_ += "\"" + key + "\": ";
    }
    json_ += "{";
    indent_level_++;
    need_comma_ = false;
}

void JsonSerializer::end_object() {
    indent_level_--;
    json_ += "\n";
    write_indent();
    json_ += "}";
    need_comma_ = true;
}

void JsonSerializer::start_array(const std::string& key) {
    write_comma();
    write_indent();
    json_ += "\"" + key + "\": [";
    indent_level_++;
    need_comma_ = false;
}

void JsonSerializer::end_array() {
    indent_level_--;
    json_ += "\n";
    write_indent();
    json_ += "]";
    need_comma_ = true;
}

// JsonDeserializer implementation
JsonDeserializer::JsonDeserializer(const std::string& json)
    : json_(json), valid_(false), error_("Not implemented") {
    // TODO: Implement proper JSON parsing
    // For now, this is a placeholder
}

bool JsonDeserializer::read_string(const std::string& key, std::string& value) {
    (void)key;
    (void)value;
    return false;
}

bool JsonDeserializer::read_int(const std::string& key, int64_t& value) {
    (void)key;
    (void)value;
    return false;
}

bool JsonDeserializer::read_uint(const std::string& key, uint64_t& value) {
    (void)key;
    (void)value;
    return false;
}

bool JsonDeserializer::read_float(const std::string& key, double& value) {
    (void)key;
    (void)value;
    return false;
}

bool JsonDeserializer::read_bool(const std::string& key, bool& value) {
    (void)key;
    (void)value;
    return false;
}

} // namespace core
} // namespace poko
